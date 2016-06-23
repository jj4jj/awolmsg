#include <hiredis/hiredis.h>

#include "dcpots/base/logger.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/base/dcseqnum.hpp"
#include "dcpots/base/msg_proto.hpp"
#include "dcpots/base/dcutils.hpp"
#include "dcpots/base/app.hpp"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/server/dcsrpc.h"
#include "dcpots/utility/redis/dcredis.h"
#include "dcpots/utility/mysql/dcmysqlc_pool.h"

#include "../proto/awolmsg.pb.h"
#include "../core/awol_msg.h"

using namespace std;
//using namespace google::protobuf;
using namespace dcsutil;
using namespace dcrpc;
using namespace awolmsg;
struct MsgActorState {
    MsgActor    actor;
    int         id;
    uint32_t    expired;
};
using namespace dcsutil;
typedef msgproto_t<awolmsg::MsgStoreCallBack>   MysqlCallBack;
typedef msgproto_t<awolmsg::Msg>	MsgOPT;

#define NOTIFY_EVENT_CHANNEL "msg/event/set"
struct MsgService : public RpcService {
    RedisAsyncAgent * redis{ nullptr };
    RpcServer *       svr{ nullptr };
    std::unordered_map<MsgActor, MsgActorState*>    actor_map_fds;
    std::multimap<int, MsgActorState*>              fd_map_actors;
    msg_buffer_t                                    tmp_msgbuff;
    bool                                            no_register{ true };
    mysqlclient_pool_t                            * mysql_pool{ nullptr };
    /////////////////////////////////////////////////////////////////////////////////////////////////
public:
    MsgService(RedisAsyncAgent * r, RpcServer * svr_, mysqlclient_pool_t * p) : RpcService("msg", true), redis(r), svr(svr_), mysql_pool(){
        tmp_msgbuff.create(1024 * 1024);
	}
    ~MsgService(){
        actor_state_clear();
        tmp_msgbuff.destroy();
    }
    void    actor_state_update(const MsgActor & actor, int fromid){
        auto it = actor_map_fds.find(actor);
        if (it != actor_map_fds.end()){
            it->second->expired = dcsutil::time_unixtime_s() + 300;
            it->second->id = fromid;
        }
        else {
            MsgActorState * state = new MsgActorState();
            state->id = fromid;
            state->actor = actor;
            state->expired = dcsutil::time_unixtime_s() + 300;
            actor_map_fds.insert(std::make_pair(state->actor, state));
            fd_map_actors.insert(std::make_pair(fromid, state));
        }
    }
    void    actor_state_tick_check(){
        //todo split time
        auto it = actor_map_fds.begin();
        auto tnow = dcsutil::time_unixtime_s();
        while (it != actor_map_fds.end()){
            auto cit = it++;
            MsgActorState * state = cit->second;
            if (state->expired <= tnow){
                fd_map_actors.erase(state->id);
                delete state;
                actor_map_fds.erase(cit);
            }
        }
    }
    void    update(){
        actor_state_tick_check();
        if (redis->ready() && no_register){
            no_register = false;
            //listen actor type id
            GLOG_DBG("initilizing ... subscribe " NOTIFY_EVENT_CHANNEL);
            redis->subscribe([this](int ret, redisReply * reply){
                if (ret){
                    GLOG_ERR("command subscribe chagne error ret:%d", ret);
                    return;
                }
                if (reply->type == REDIS_REPLY_ERROR){
                    GLOG_ERR("redis subscribe chagne error :%s", reply->str);
                    return;
                }
                if (reply->elements != 3){
                    GLOG_ERR("redis subscribe result format error list n:%d", reply->elements);
                    return;
                }
                GLOG_DBG("subscribe type:%s channel:%s",
                    reply->element[0]->str, reply->element[1]->str);
                if (strcasecmp("subscribe", reply->element[0]->str) == 0){
                    GLOG_IFO("subscriber number :%d", reply->element[2]->integer);
                }
                if (strcasecmp("message", reply->element[0]->str) == 0){
                    GLOG_DBG("recv (subscribe) notify msg len:%d", reply->element[2]->len);
                    msgproto_t<awolmsg::NotifyMsg> notifymsg;
                    if (!notifymsg.Unpack(reply->element[2]->str, reply->element[2]->len)){
                        GLOG_ERR("unpack msg error recv notify msg:%s lenth:%d",
                            reply->element[2]->str, reply->element[2]->len);
                        return;
                    }
                    MsgActor actor(notifymsg.actor().type(), notifymsg.actor().id());
                    this->recv_notify_actor_msg(actor, notifymsg.data(), notifymsg.clientsrc());
                }
            }, NOTIFY_EVENT_CHANNEL);
        }
    }
    void    actor_state_clear(){
        auto it = actor_map_fds.begin();
        while (it != actor_map_fds.end()){
            auto cit = it++;
            MsgActorState * state = cit->second;
            fd_map_actors.erase(state->id);
            delete state;
            actor_map_fds.erase(cit);
        }
    }
    void    clientclosed(int fromid){
        auto it = actor_map_fds.begin();
        while (it != actor_map_fds.end()){
            auto cit = it++;
            MsgActorState * state = cit->second;
            if (state->id == fromid){
                fd_map_actors.erase(state->id);
                delete state;
                actor_map_fds.erase(cit);
            }
        }
    }
    int recv_notify_actor_msg(const MsgActor & actor, const string & replymsg, int clientsrc){
		auto it = actor_map_fds.find(actor);
        if (it != actor_map_fds.end()){
            int clientfd = it->second->id;
            if (clientfd != clientsrc){
                GLOG_DBG("notify actor :%d-%lu new msg from clientsrc:%d", actor.type(), actor.id(), clientsrc);
                //notify msg
                RpcValues v;
                v.addb(replymsg.data(), replymsg.length());
                svr->push(name(), clientfd, v);
                return 0;
            }
        }
		else {
			GLOG_DBG("not found actor:%s clientfd:%d", actor.ShortDebugString().c_str(), clientsrc);
		}
        return -1;
    }
    void send_notify_actor_msg(const MsgActor & actor, const string & replymsg, int clientsrc){		
		if (recv_notify_actor_msg(actor, replymsg, clientsrc) == 0){
            return;
        }
        //notify other server 
        msgproto_t<awolmsg::NotifyMsg> notifymsg;
        notifymsg.mutable_actor()->CopyFrom(actor);
        notifymsg.set_clientsrc(clientsrc);
        notifymsg.set_data(replymsg.data(), replymsg.length());
        if (!notifymsg.Pack(tmp_msgbuff)){
            GLOG_ERR("pack notify msg error !");
            return;
        }
        GLOG_DBG("publish notify event - send notify actor msg len:%d", tmp_msgbuff.valid_size);
        redis->command([this](int ret, redisReply * reply){
            if (ret || reply->type == REDIS_REPLY_ERROR){
                GLOG_ERR("publish msg ret:%d", ret);
            }
        }, "PUBLISH " NOTIFY_EVENT_CHANNEL " %b", tmp_msgbuff.buffer,
            tmp_msgbuff.valid_size);
    }
	const string mergekey(int actor_type, uint64_t actor_id, int type){
		string keyname;
		strnprintf(keyname, 64, "msg/%d/%d:%lu", actor_type, actor_id, type);
		return keyname;
	}
    int dispatch_redis_store_command(uint64_t cookie, const MsgOPT & msg, const string & msgdata, std::string & error, int clientid);
    int dispatch_mysql_store_command(uint64_t cookie, const MsgOPT & msg, const string & msgdata, std::string & error, int clientid);
	virtual int yield(uint64_t cookie, const RpcValues & args, std::string & error, int clientid){
		const string & msgdata = args.getb();
		MsgOPT msg;
		if (!msg.Unpack(msgdata.data(), msgdata.length())){
			GLOG_ERR("unpack msg error ! %d", msgdata.length());
			return -1;
		}
        int ret = 0;
        if (msg.flag().store() & MSG_OPT_STORE_MYSQL){
            ret = dispatch_mysql_store_command(cookie, msg, msgdata, error, clientid);
        }
        else if (msg.flag().store() & MSG_OPT_STORE_MYSQL){
            ret = dispatch_redis_store_command(cookie, msg, msgdata, error, clientid);
        }
		return ret;
	}
};
#define MSGSVR_VERSION	("0.0.1")
int main(int argc,const char ** argv){
    struct MsgServer : public dcsutil::App {
        dcrpc::RpcServer	rpc;
        RedisAsyncAgent     redis;
        mysqlclient_pool_t  mysql;
        /////////////////////////////////////////////
        MsgServer() : dcsutil::App(MSGSVR_VERSION){}
        string options(){
            return ""
            "db:r:d:mysql database name:test;"
                "db-user:r::mysql user name:test;"
                "db-pwd:r::mysql password:123456;"
                "db-host:r::mysql host:127.0.0.1;"
                "listen:r:l:rpc listen address (tcp):127.0.0.1:8888;"
                "redis:r::redis address:127.0.0.1:6379;";
        }
        int on_init(const char * configfile){
            if (rpc.init(cmdopt().getoptstr("listen"))){
                return -1;
            }

            if (redis.init(cmdopt().getoptstr("redis"))){
                return -2;
            }

            mysqlclient_t::cnnx_conf_t config;
            config.ip = cmdopt().getoptstr("db-host");
            config.uname = cmdopt().getoptstr("db-user");
            config.passwd = cmdopt().getoptstr("db-pwd");
            config.dbname = cmdopt().getoptstr("db");
            if (mysql.init(config, 2)){
                return -3;
            }
            rpc.regis(new MsgService(&redis, &rpc, &mysql));
            return 0;
        }
        int on_loop(){
            int ret = rpc.update();
            ret += redis.update();
            ret += mysql.poll();
            return ret;
        }
    };

    MsgServer msgsvr;
    if (msgsvr.init(argc, argv)){
        return -1;
    }
    return msgsvr.run();
}




int MsgService::dispatch_redis_store_command(uint64_t cookie, const MsgOPT & msg,
    const string & msgdata, std::string & error, int clientid){
    auto hsetkey = mergekey(msg.actor().type(),
        msg.actor().id(), msg.type());
    MsgActor actor(msg.actor().type(), msg.actor().id());
    //int msgtype = msg.type();
    GLOG_TRA("redis command key :%s op:%d", hsetkey.c_str(), msg.op());
    if (msg.op() == MSG_OP_LIST){
        actor_state_update(actor, clientid);
        //////////////////////////////////////////////////////////
        redis->command([this, cookie](int ret, redisReply * reply){
            RpcValues result;
            if (ret != 0){
                GLOG_ERR("redis command error result ret:%d", ret);
                this->resume(cookie, result, -1, "internal error");
            }
            if (reply->type == REDIS_REPLY_ERROR){
                this->resume(cookie, result, -2, reply->str);
            }
            for (size_t i = 0; i < reply->elements; ++i){
                result.addb(reply->element[i]->str, reply->element[i]->len);
            }
            this->resume(cookie, result);
        }, "HVALS %s", hsetkey.data());
    }
    else if (msg.op() == MSG_OP_SET){
        char msgbuff[384 * 1024];
        auto mid = msg.data(0).id();
        if (!msg.data(0).SerializeToArray(msgbuff, sizeof(msgbuff))){
            GLOG_ERR("set value msg pack error ! size:%d", msg.data(0).ByteSize());
        }
        redis->command([this, cookie, actor, msgdata, clientid](int ret, redisReply * reply){
            RpcValues result;
            if (ret != 0){
                GLOG_ERR("redis command error result ret:%d", ret);
                this->resume(cookie, result, -1, "internal error");
            }
            if (reply->type == REDIS_REPLY_ERROR){
                this->resume(cookie, result, -2, reply->str);
            }
            if (reply->type != REDIS_REPLY_INTEGER){
                GLOG_ERR("reply type not string error type:%d", reply->type);
                this->resume(cookie, result, -3, "db value type error");
            }
            result.addi(reply->integer);
            result.setb(msgdata.data(), msgdata.length());
            this->resume(cookie, result);
        }, "HSET %s %s %b", hsetkey.data(),
            std::to_string(mid).c_str(), msgbuff, msg.data(0).ByteSize());
    }
    else if (msg.op() == MSG_OP_INSERT){
        char msgbuff[384 * 1024];
        auto mid = msg.data(0).id();
        if (!msg.data(0).SerializeToArray(msgbuff, sizeof(msgbuff))){
            GLOG_ERR("set value msg pack error ! size:%d", msg.data(0).ByteSize());
        }
        redis->command([this, cookie, actor, msgdata, clientid](int ret, redisReply * reply){
            RpcValues result;
            if (ret != 0){
                GLOG_ERR("redis command error result ret:%d", ret);
                this->resume(cookie, result, -1, "internal error");
            }
            if (reply->type == REDIS_REPLY_ERROR){
                this->resume(cookie, result, -2, reply->str);
            }
            if (reply->type != REDIS_REPLY_INTEGER){
                GLOG_ERR("reply type not string error type:%d", reply->type);
                this->resume(cookie, result, -3, "db value type error");
            }
            send_notify_actor_msg(actor, msgdata, clientid);
            result.addi(reply->integer);
            result.setb(msgdata.data(), msgdata.length());
            this->resume(cookie, result);
        }, "HSETNX %s %s %b", hsetkey.data(),
            std::to_string(mid).c_str(), msgbuff, msg.data(0).ByteSize());
    }
    else if (msg.op() == MSG_OP_RM){
        actor_state_update(actor, clientid);
        auto mid = msg.data(0).id();
        auto rmcb = [this, cookie](int ret, redisReply * reply){
            RpcValues result;
            if (ret != 0){
                GLOG_ERR("redis command error result ret:%d", ret);
                this->resume(cookie, result, -1, "internal error");
            }
            if (reply->type == REDIS_REPLY_ERROR){
                this->resume(cookie, result, -2, reply->str);
            }
            if (reply->type != REDIS_REPLY_INTEGER){
                GLOG_ERR("reply type not string error type:%d", reply->type);
                this->resume(cookie, result, -3, "db value type error");
            }
            result.addi(reply->integer);
            this->resume(cookie, result);
        };
        if (mid > 0){
            redis->command(rmcb, "HDEL %s %s", hsetkey.data(), std::to_string(mid).c_str());
        }
        else {
            redis->command(rmcb, "DEL %s", hsetkey.data());
        }
    }
    else if (msg.op() == MSG_OP_GET){
        actor_state_update(actor, clientid);
        auto mid = msg.data(0).id();
        redis->command([this, mid, cookie](int ret, redisReply * reply){
            RpcValues result;
            if (ret != 0){
                GLOG_ERR("redis command error result ret:%d", ret);
                this->resume(cookie, result, -1, "internal error");
            }
            if (reply->type == REDIS_REPLY_ERROR){
                this->resume(cookie, result, -2, reply->str);
            }
            if (reply->type == REDIS_REPLY_NIL){
                this->resume(cookie, result, 0, "nil value");
            }
            if (reply->type != REDIS_REPLY_STRING){
                GLOG_ERR("reply type not string error type:%d", reply->type);
                this->resume(cookie, result, -3, "db value type error");
            }
            result.addi(mid);
            result.setb(reply->str, reply->len);
            this->resume(cookie, result);
        }, "HGET %s %s", hsetkey.data(), std::to_string(mid).c_str());
    }
    else {
        return -3;
    }
	return 0;
}

typedef dcsutil::mysqlclient_pool_t::command_t  command_t;
typedef dcsutil::mysqlclient_pool_t::result_t   result_t;
static void mysql_command_dispatch(void *ud, const result_t & res, const command_t & cmd){
    MsgService * service = (MsgService*)ud;
    //cmd.cbdata;
    MysqlCallBack cb;
    if (!cb.Unpack(cmd.cbdata)){
        GLOG_ERR("callback unpack error ! cbdata size:%d", cmd.cbdata.valid_size);
        return;
    }
    const_cast<msg_buffer_t&>(cmd.cbdata).destroy();

    const MsgOPT & msg = (const MsgOPT &)cb.msg();
    MsgActor actor(msg.actor().type(), msg.actor().id());
    //int msgtype = msg.type();
    string msgdata = msg.SerializeAsString();

    RpcValues result;
    if (res.status != 0){
        result.addi(res.status);
        result.addi(res.err_no);
        result.adds("mysql error !");
        GLOG_ERR("mysql status status:%d error:%d error:%s",
            res.status, res.err_no, res.error.c_str());
        service->resume(cb.cookie(), result, res.status, "mysql error !");
        return;
    }
    if (msg.op() == MSG_OP_LIST){
        for (int i = 0; i < (int)res.fetched_results.size(); ++i){
            result.addb(res.fetched_results[i][0].second.buffer,
                res.fetched_results[i][0].second.valid_size);
        }
    }
    else if (msg.op() == MSG_OP_SET){
        result.addi(res.affects);
        result.setb(msgdata.data(), msgdata.length());
    }
    else if (msg.op() == MSG_OP_INSERT){
        service->send_notify_actor_msg(actor, msgdata, cb.clientid());
        result.addi(res.affects);
        result.setb(msgdata);
    }
    else if (msg.op() == MSG_OP_RM){
        service->actor_state_update(actor, cb.clientid());
        result.addi(res.affects);
    }
    else if (msg.op() == MSG_OP_GET){
        service->actor_state_update(actor, cb.clientid());
        //auto mid = msg.data(0).id();
        if (res.fetched_results.size() == 1){
            result.setb(res.fetched_results[0][0].second.buffer,
                res.fetched_results[0][0].second.valid_size);
        }
    }
    else {
        GLOG_ERR("not found op type:%d", msg.op());
        return;
    }
    service->resume(cb.cookie(), result);
}

int MsgService::dispatch_mysql_store_command(uint64_t cookie, const MsgOPT & msg, 
    const std::string & msgdata, std::string & error, int clientid){
    string strescape;
    MsgActor actor(msg.actor().type(), msg.actor().id());
    int msgtype = msg.type();
    command_t cmd;
    cmd.cbdata.create(128+msg.ByteSize());
    MysqlCallBack cb;
    cb.set_cookie(cookie);
    cb.set_clientid(clientid);
    cb.mutable_msg()->CopyFrom(msg);
    if (msg.op() == MSG_OP_LIST){
        actor_state_update(actor, clientid);
        ///////////////////////////////////////////////////////////
        strnprintf(cmd.sql, 128, "SELECT msg FROM msg WHERE type=%d AND actor='%d:%lu' ORDER BY msgid DESC LIMIT %d OFFSET 0;",
			msgtype, msg.actor().type(), msg.actor().id(), MAX_MSG_LIST_SIZE);
        cmd.need_result = true;
        if (!cb.Pack(cmd.cbdata)){
            GLOG_ERR("msg error pack callback length: %d", cb.ByteSize());
            cmd.cbdata.destroy();
            return -2;
        }
        return mysql_pool->execute(cmd, mysql_command_dispatch, this);
    }
    else if (msg.op() == MSG_OP_INSERT){        
        if (!msg.data(0).SerializeToArray(this->tmp_msgbuff.buffer, this->tmp_msgbuff.max_size)){
            GLOG_ERR("msg pack error ! bytesize:%d", msg.data(0).ByteSize());
            cmd.cbdata.destroy();
            return -3;
        }
        strnprintf(cmd.sql, 1024 + msg.ByteSize(), "INSERT INTO msg SET type=%d, actor='%d:%lu', msgid=%lu, msg='%s';",
            msgtype, msg.actor().type(), msg.actor().id(), msg.data(0).id(),
            mysql_pool->mysql()->escape(strescape, this->tmp_msgbuff.buffer,
            msg.data(0).ByteSize()));
        if (!cb.Pack(cmd.cbdata)){
            GLOG_ERR("msg error pack callback length: %d", cb.ByteSize());
            cmd.cbdata.destroy();
            return -2;
        }
        return mysql_pool->execute(cmd, mysql_command_dispatch, this);
    }
    else if (msg.op() == MSG_OP_SET){
        if (!msg.data(0).SerializeToArray(this->tmp_msgbuff.buffer, this->tmp_msgbuff.max_size)){
            GLOG_ERR("msg pack error ! bytesize:%d", msg.data(0).ByteSize());
            cmd.cbdata.destroy();
            return -1;
        }
		int version = msg.data(0).ext().version();
		if ( version < 1){
			GLOG_ERR("update msg version error :%d ", version);
            cmd.cbdata.destroy();
            return -2;
		}
        strnprintf(cmd.sql, 1024 + msg.ByteSize(),
			"UPDATE msg SET msg='%s',version=%d WHERE type=%d  AND actor='%d:%lu' AND msgid=%lu AND version=%d;",
            mysql_pool->mysql()->escape(strescape, this->tmp_msgbuff.buffer,
            msg.data(0).ByteSize()), version, 
            msgtype, msg.actor().type(), msg.actor().id(),
            msg.data(0).id(), version-1);
        if (!cb.Pack(cmd.cbdata)){
            GLOG_ERR("msg error pack callback length: %d", cb.ByteSize());
            cmd.cbdata.destroy();
            return -3;
        }
        return mysql_pool->execute(cmd, mysql_command_dispatch, this);
    }
    else if (msg.op() == MSG_OP_RM){
        actor_state_update(actor, clientid);
        if (msg.data(0).id() > 0){
            strnprintf(cmd.sql, 1024, "DELETE FROM msg WHERE type=%d AND actor='%d:%lu' AND msgid=%lu AND version=%d;",
                msgtype, msg.actor().type(), msg.actor().id(), msg.data(0).id(), msg.data(0).ext().version());
        }
        else {
            strnprintf(cmd.sql, 1024, "DELETE FROM msg WHERE type=%d AND actor='%d:%lu';",
                msgtype, msg.actor().type(), msg.actor().id());
        }
        if (!cb.Pack(cmd.cbdata)){
            GLOG_ERR("msg error pack callback length: %d", cb.ByteSize());
            cmd.cbdata.destroy();
            return -2;
        }
        return mysql_pool->execute(cmd, mysql_command_dispatch, this);
    }
    else if (msg.op() == MSG_OP_GET){
        actor_state_update(actor, clientid);
        strnprintf(cmd.sql, 1024 , "SELECT msg FROM msg WHERE type=%d AND actor='%d:%lu' AND msgid=%lu;",
            msgtype, msg.actor().type(),msg.actor().id(), msg.data(0).id());
        cmd.need_result = true;
        if (!cb.Pack(cmd.cbdata)){
            GLOG_ERR("msg error pack callback length: %d", cb.ByteSize());
            cmd.cbdata.destroy();
            return -2;
        }
        return mysql_pool->execute(cmd, mysql_command_dispatch, this);
    }
    else {
        cmd.cbdata.destroy();
        return -3;
    }
    cmd.cbdata.destroy();
    return 0;
}

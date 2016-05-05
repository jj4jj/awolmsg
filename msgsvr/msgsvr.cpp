#include <hiredis/hiredis.h>
#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/server/dcsrpc.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/utility/redis/dcredis.h"
#include "dcpots/base/dcseqnum.hpp"
#include "dcpots/base/msg_proto.hpp"
#include "dcpots/base/dcutils.hpp"
#include "../proto/awolmsg.pb.h"
#include "../awolmsg/awol_msg.h"
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

#define NOTIFY_EVENT_CHANNEL "msg$event$set"
struct MsgService : public RpcService {
    RedisAsyncAgent * redis{ nullptr };
    RpcServer *       svr{ nullptr };
    std::unordered_map<MsgActor, MsgActorState*>    actor_map_fds;
    std::multimap<int, MsgActorState*>              fd_map_actors;
    typedef msgproto_t<awolmsg::Msg>	MsgOPT;
    msg_buffer_t                                    tmp_msgbuff;
    bool                                            no_register{ true };
    /////////////////////////////////////////////////////////////////////////////////////////////////
public:
    MsgService(RedisAsyncAgent * r, RpcServer * svr_) : RpcService("msg", true), redis(r), svr(svr_){    
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
                    GLOG_DBG("recv (subscribe) notify msg len:%d", reply->element[2]->str, reply->element[2]->len);
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
                GLOG_DBG("notify actor :%d-%lu new msg from clientsrc:%d", actor.type, actor.id, clientsrc);
                //notify msg
                RpcValues v;
                v.addb(replymsg.data(), replymsg.length());
                svr->push(name(), clientfd, v);
                return 0;
            }
        }
        return -1;
    }
    void send_notify_actor_msg(const MsgActor & actor, const string & replymsg, int clientsrc){
        if (recv_notify_actor_msg(actor, replymsg, clientsrc) == 0){
            return;
        }
        //notify other server 
        msgproto_t<awolmsg::NotifyMsg> notifymsg;
        notifymsg.mutable_actor()->set_type(actor.type);
        notifymsg.mutable_actor()->set_id(actor.id);
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
		strnprintf(keyname, 64, "msg$%d:%lu$%d", actor_type, actor_id, type);
		return keyname;
	}
	virtual int yield(uint64_t cookie, const RpcValues & args, std::string & error, int clientid){
		const string & msgdata = args.getb();
		MsgOPT msg;
		if (!msg.Unpack(msgdata.data(), msgdata.length())){
			GLOG_ERR("unpack msg error ! %d", msgdata.length());
			return -1;
		}
		auto hsetkey = mergekey(msg.actor().type(),
			msg.actor().id(), msg.type());
        MsgActor actor(msg.actor().type(), msg.actor().id());
        int msgtype = msg.type();
        GLOG_TRA("redis command key :%s op:%d", hsetkey.c_str(), msg.op());
        if (msg.op() == MSG_OP_LIST){
            actor_state_update(actor, clientid);
            //////////////////////////////////////////////////////////
			redis->command([this, cookie](int ret, redisReply * reply){
				RpcValues result;
				if (ret != 0){
					GLOG_ERR("redis command error result ret:%d", ret);
					return this->resume(cookie, result, -1, "internal error");
				}
				if (reply->type == REDIS_REPLY_ERROR){
					return this->resume(cookie, result, -2, reply->str);
				}
				for (int i = 0; i < reply->elements; ++i){					
					result.addb(reply->element[i]->str, reply->element[i]->len);
				}
				return this->resume(cookie, result);
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
					return this->resume(cookie, result, -1, "internal error");
				}
				if (reply->type == REDIS_REPLY_ERROR){
					return this->resume(cookie, result, -2, reply->str);
				}
				if (reply->type != REDIS_REPLY_INTEGER){
					GLOG_ERR("reply type not string error type:%d", reply->type);
					return this->resume(cookie, result, -3, "db value type error");
				}
                send_notify_actor_msg(actor, msgdata, clientid);
				result.addi(reply->integer);
                result.setb(msgdata.data(), msgdata.length());
				return this->resume(cookie, result);
			}, "HSET %s %s %b", hsetkey.data(),
				std::to_string(mid).c_str(), msgbuff, msg.data(0).ByteSize());
		}
		else if (msg.op() == MSG_OP_RM){
            actor_state_update(actor, clientid);
            auto mid = msg.data(0).id();
            auto rmcb = [this, cookie](int ret, redisReply * reply){
                RpcValues result;
                if (ret != 0){
                    GLOG_ERR("redis command error result ret:%d", ret);
                    return this->resume(cookie, result, -1, "internal error");
                }
                if (reply->type == REDIS_REPLY_ERROR){
                    return this->resume(cookie, result, -2, reply->str);
                }
                if (reply->type != REDIS_REPLY_INTEGER){
                    GLOG_ERR("reply type not string error type:%d", reply->type);
                    return this->resume(cookie, result, -3, "db value type error");
                }
                result.addi(reply->integer);
                return this->resume(cookie, result);
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
					return this->resume(cookie, result, -1, "internal error");
				}
				if (reply->type == REDIS_REPLY_ERROR){
					return this->resume(cookie, result, -2, reply->str);
				}
				if (reply->type == REDIS_REPLY_NIL){
					return this->resume(cookie, result, 0, "nil value");
				}
				if (reply->type != REDIS_REPLY_STRING){
					GLOG_ERR("reply type not string error type:%d", reply->type);
					return this->resume(cookie, result, -3, "db value type error");
				}
				result.addb(reply->str, reply->len);
				result.seti(mid);
				return this->resume(cookie, result);
			}, "HGET %s %s", hsetkey.data(), std::to_string(mid).c_str());
		}
		else {
			return -3;
		}
		return 0;
	}
};

int main(int argc, char ** argv){
	cmdline_opt_t cmdline(argc, argv);

    //todo addres tobe configuration
	dcrpc::RpcServer	rpc;
	if (rpc.init("127.0.0.1:8888")){
		return -1;
	}

	RedisAsyncAgent redis;
	if (redis.init("127.0.0.1:6379")){
		return -2;
	}

	MsgService msgsvc(&redis, &rpc);

	rpc.regis(&msgsvc);

	while (true){
        rpc.update();
        redis.update();
	}

	return 0;
}

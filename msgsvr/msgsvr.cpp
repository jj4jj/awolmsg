#include <hiredis/hiredis.h>
#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/server/dcsrpc.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/utility/redis/dcredis.h"
#include "dcpots/base/dcseqnum.hpp"
#include "dcpots/base/msg_proto.hpp"
#include "../proto/awolmsg.pb.h"
using namespace std;
//using namespace google::protobuf;
using namespace dcsutil;
using namespace dcrpc;
using namespace awolmsg;
struct MsgService : public RpcService {
	RedisAsyncAgent * redis{ nullptr };
	MsgService(RedisAsyncAgent * r) : RpcService("msg", true), redis(r){
	}
	const string mergekey(int actor_type, uint64_t actor_id, int type){
		string keyname;
		strnprintf(keyname, 64, "msg$%d:%lu$%d", actor_type, actor_id, type);
		return keyname;
	}
	virtual int yield(uint64_t cookie, const RpcValues & args, std::string & error){
		//list(actor, type), add(actor, type, msg), rm(actor, type, msg_id),
		//update(actor, type, msg_id, msg)
		const string & msgdata = args.getb();
		typedef msgproto_t<awolmsg::Msg>	MsgOPT;
		MsgOPT msg;
		if (!msg.Unpack(msgdata.data(), msgdata.length())){
			GLOG_ERR("unpack msg error ! %d", msgdata.length());
			return -1;
		}
		auto hsetkey = mergekey(msg.actor().type(),
			msg.actor().id(), msg.type());

		GLOG_TRA("redis command key :%s", hsetkey.c_str());
		if (msg.op() == MSG_OP_LIST){
			redis->command([this, cookie](int ret, redisReply * reply){
				RpcValues result;
				if (ret != 0){
					GLOG_ERR("redis command error result ret:%d", ret);
					return this->resume(cookie, result, -1);// , "internal error");
				}
				if (reply->type == REDIS_REPLY_ERROR){
					return this->resume(cookie, result, -2);// , reply->str);
				}
				for (int i = 0; i < reply->elements; ++i){					
					result.addb(reply->element[i]->str, reply->element[i]->len);
				}
				return this->resume(cookie, result);
			}, "HVALS %s", hsetkey.data());
		}
		else if (msg.op() == MSG_OP_SET){
			auto mid = msg.data(0).id();
			auto mdata = msg.data(0).SerializeAsString();
			redis->command([this, cookie](int ret, redisReply * reply){
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
			}, "HSET %s %s %b", hsetkey.data(),
					std::to_string(mid).c_str(), mdata.data(), mdata.length());
		}
		else if (msg.op() == MSG_OP_RM){
			auto mid = msg.data(0).id();
			redis->command([this, cookie](int ret, redisReply * reply){
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
			}, "HDEL %s %s", hsetkey.data(),std::to_string(mid).c_str());
		}
		else if (msg.op() == MSG_OP_GET){
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

	dcrpc::RpcServer	rpcsvr;
	if (rpcsvr.init("127.0.0.1:8888")){
		return -1;
	}

	RedisAsyncAgent raa;
	if (raa.init("127.0.0.1:6379")){
		return -2;
	}

	MsgService msgsvc(&raa);

	rpcsvr.regis(&msgsvc);

	while (true){
		raa.update();
		rpcsvr.update();
	}

	return 0;
}

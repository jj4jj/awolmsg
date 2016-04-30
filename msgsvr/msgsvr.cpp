#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/server/dcsrpc.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/utility/redis/dcredis.h"
#include <hiredis/hiredis.h>
using namespace std;
//using namespace google::protobuf;
using namespace dcsutil;
using namespace dcrpc;

struct MsgService : public RpcService {
	RedisAsyncAgent * redis{ nullptr };
	MsgService(RedisAsyncAgent * r) : RpcService("msg"), redis(r){
	}
	const string mergekey(int actor_type, uint64_t actor_id, int type){
		string keyname;
		strnprintf(keyname, 64, "msg$%d$:%lu$%d", actor_type, actor_id, type);
		return keyname;
	}
	virtual int call(RpcValues & result, const RpcValues & args, std::string * error){
		//list(actor, type), add(actor, type, msg), rm(actor, type, msg_id),
		//update(actor, type, msg_id, msg)
		const string & name = args.gets();
		auto actor_type = args.geti(1);
		auto actor_id = args.geti(2);
		auto type = args.geti(3);
		if (name == "list"){
			redis->command([&result, &error](int ret, redisReply * reply){
				if (ret != 0){
					GLOG_ERR("redis command error result ret:%d", ret);
					return -1; 
				}
				if (reply->type == REDIS_REPLY_ERROR){
					error->copy(reply->str, reply->len);
					return -2;
				}
				if (reply->type == REDIS_REPLY_NIL){
					return 0;
				}
				for (int i = 0; i < reply->elements; ++i){					
					result.addb(reply->element[i]->str, reply->element[i]->len);
				}
				return 0;
			}, "hvals %s", mergekey(actor_type, actor_id, type).c_str());
		}
		else if (args.gets() == "add"){
		}
		else if (args.gets() == "update"){
		}
		else if (args.gets() == "rm"){
		}
		return -3;
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

#pragma once
#include "dcpots/base/noncopyable.h"
namespace dcrpc {
	struct RpcClient;
};
struct msg_buffer_t;

namespace awolmsg {
    struct MsgActor;
	struct MsgSvrImpl;
	struct MsgPortal;
	class MsgSvr : noncopyable {
	public:
		static MsgSvr & instance();
	public:
		int init(const std::string & svraddr);
		int destory();
		int update();
        bool ready();
		int regis(MsgPortal * portal);
        void unregis(MsgPortal * portal);
        MsgPortal * find(const MsgActor & actor, int type);
		dcrpc::RpcClient * rpc();
        msg_buffer_t *     msg_buffer();
	public:
		~MsgSvr();
	private:
		MsgSvr(){}
		MsgSvrImpl * impl{ nullptr };
	};
}
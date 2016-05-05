#pragma once
namespace dcrpc {
	struct RpcClient;
};
struct msg_buffer_t;
namespace awolmsg {
	struct MsgSvrImpl;
	struct MsgPortal;
	class MsgSvr {
	public:
		static MsgSvr & instance();
	public:
		int init(const std::string & svraddr);
		int destory();
		int update();
        bool ready();
		int regis(MsgPortal * portal);
		dcrpc::RpcClient * rpc();
        msg_buffer_t *     msg_buffer();
	public:
		~MsgSvr();
	private:
		MsgSvr(){}
		MsgSvrImpl * impl{ nullptr };
	};
}
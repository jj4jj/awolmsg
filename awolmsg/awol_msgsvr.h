#pragma once
namespace dcrpc {
	struct RpcClient;
};
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
		int regis(MsgPortal * portal);
		dcrpc::RpcClient * rpc();
	public:
		~MsgSvr();
	private:
		MsgSvr(){}
		MsgSvrImpl * impl{ nullptr };
	};
}
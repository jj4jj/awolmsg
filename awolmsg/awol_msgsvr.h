#pragma once
#include "awol_msg.h"
#include "awol_msgportal.h"
#include "awol_messenger.h"
namespace awolmsg {
	struct MsgSvrImpl;
	struct MsgPortal;
	struct RpcClient;
	class MsgSvr {
	public:
		static MsgSvr & instance();
	public:
		int init();
		int destory();
		int update();
		int regis(MsgPortal * portal);
		RpcClient * rpc();
	public:
		~MsgSvr();
	private:
		MsgSvrImpl * impl{ nullptr };
	};
}
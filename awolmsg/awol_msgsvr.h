#pragma once
#include "awol_msg.h"
namespace awolmsg {
	struct MsgSvrImpl;
	class MsgSvr {
	public:
		int init();
		int destory();
		template<class MsgPortalT>
		int regis(const MsgOptions &);
		int update();
	private:
		MsgSvrImpl * impl{ nullptr };
	};
}
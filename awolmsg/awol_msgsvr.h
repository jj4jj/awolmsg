#pragma once
#include "awol_msg.h"
#include "awol_msgportal.h"
#include "awol_messenger.h"
namespace awolmsg {
	struct MsgSvrImpl;
	class MsgSvr {
	public:
		static MsgSvr & instance();
	public:
		int init();
		int destory();
		int regis(int type, const MsgOptions & mo);
#if 0
		template<class MsgPortalT>
		int regis(const MsgOptions & mo){
			return regis(MsgPortalT::type(), mo);
		}
#else
		int regis(MsgPortal * portal){
			return regis(portal->type(), portal->options());
		}
		Messenger * messenger(MsgPortal * portal);
#endif
		int update();
	public:
		~MsgSvr();
	private:
		MsgSvrImpl * impl{ nullptr };
	};
}
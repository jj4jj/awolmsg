#pragma once

namespace awolmsg {
	struct MsgActor {
		int type{ 0 };
		uint64_t id{ 0 };
		MsgActor(int t_, uint64_t id_) :type(t_), id(id_){}
	};
	struct MsgOptions {
		enum {
			MSG_OPT_OWN_BO = 0,//both
			MSG_OPT_OWN_SO = 1,//server only
			MSG_OPT_OWN_CO = 2,//client only
		};
		enum {
			MSG_OPT_STORE_NONE = 0,//expiration ?
			MSG_OPT_STORE_TTL = 1,//expiration ?
			MSG_OPT_STORE_SHIFT = 2,//shift when full
		};
		enum {
			MSG_OPT_CPERM_NONE = 0,//flags
			MSG_OPT_CPERM_SEND = 1,//flags
			MSG_OPT_CPERM_REMOVE = 2,//flags
			MSG_OPT_CPERM_LIST = 4,//flags
			MSG_OPT_CPERM_UPDATE = 8,//flags
			MSG_OPT_CPERM_ALL = 0xF, //PERM
		};
		int owner{ 0 }; //server only ? client only ? both ?
		int store{ 0 }; //list ? hash ? set ?
		int cperm{ 0 }; //other ext
		MsgOptions(int own = 0, int store = 0, int perm = 0) : owner(own), store(store), cperm(perm){
		}
	};
}
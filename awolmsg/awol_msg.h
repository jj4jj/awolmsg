#pragma once
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include "awolmsg.pb.h"
namespace awolmsg {
struct MsgKeyData {
    uint64_t id;
    std::string   data;
    MsgKeyData(uint64_t i, const std::string & sd) :id(i), data(sd){}
};
typedef std::vector<MsgKeyData>	MsgList;
struct MsgActor {
	int type{ 0 };
	uint64_t id{ 0 };
	MsgActor(int t_ = 0, uint64_t id_ = 0) :type(t_), id(id_){}
	static const MsgActor empty;
};

struct MsgKey {
	MsgActor actor;
	int		 type;
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
	bool check(bool fromc, int flag = 0) const {
		if (fromc && owner == MSG_OPT_OWN_SO){
			return false;
		}
		if (fromc && flag > 0 && cperm > 0 && (cperm & flag) == 0){
			return false;
		}
		return true;
	}
};
};

namespace std {
    //key
	template<>
	struct hash<awolmsg::MsgKey> {
		size_t operator ()(const awolmsg::MsgKey & k) const {
			return k.actor.id;
		}
	};
	template<>
	struct equal_to<awolmsg::MsgKey> {
		bool operator ()(const awolmsg::MsgKey & k1, const awolmsg::MsgKey & k2) const {
			return k1.actor.type == k2.actor.type &&
				k1.actor.id == k2.actor.id &&
				k1.type == k2.type;
		}
	};
    //actor
    template<>
    struct hash<awolmsg::MsgActor> {
        size_t operator ()(const awolmsg::MsgActor & a) const {
            return a.id;
        }
    };
    template<>
    struct equal_to<awolmsg::MsgActor> {
        bool operator ()(const awolmsg::MsgActor & a1, const awolmsg::MsgActor & a2) const {
            return a1.type == a2.type &&
                a1.id == a2.id;
        }
    };

};

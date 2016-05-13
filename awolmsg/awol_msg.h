#pragma once
#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>
#include "awolmsg.pb.h"
namespace awolmsg {
struct MsgKeyData {
    uint64_t      id;
    uint32_t      version;
    std::string   data;
    MsgKeyData(uint64_t i, const std::string & sd, uint32_t v) :id(i), data(sd), version(v){}
};
typedef std::vector<MsgKeyData>	MsgList;
struct MsgActor : public MsgAgent {
	MsgActor(int t_ = 0, uint64_t id_ = 0){
        set_type(t_);
        set_id(id_);
    }
    MsgActor(const MsgAgent & agt) :MsgAgent(agt){}
	static const MsgActor empty;
};
struct MsgKey {
	MsgActor actor;
	int		 type;
};
struct MsgOptions : public MsgFlag {	
	MsgOptions(int own = 0, int store = 0, int perm = 0){
        set_owner((MsgFlagOwnType)own);
        set_store(store);
        set_cperm(perm);
	}
	bool check(bool fromc, int flag = 0) const {
		if (fromc && owner() == MSG_OPT_OWN_SO){
			return false;
		}
        if (fromc && flag > 0 && cperm() > 0 && (cperm() & flag) == 0){
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
            return k.actor.id();
		}
	};
	template<>
	struct equal_to<awolmsg::MsgKey> {
		bool operator ()(const awolmsg::MsgKey & k1, const awolmsg::MsgKey & k2) const {
            return k1.actor.type() == k2.actor.type() &&
                k1.actor.id() == k2.actor.id() &&
				k1.type == k2.type;
		}
	};
    //actor
    template<>
    struct hash<awolmsg::MsgActor> {
        size_t operator ()(const awolmsg::MsgActor & a) const {
            return a.id();
        }
    };
    template<>
    struct equal_to<awolmsg::MsgActor> {
        bool operator ()(const awolmsg::MsgActor & a1, const awolmsg::MsgActor & a2) const {
            return a1.type() == a2.type() &&
                a1.id() == a2.id();
        }
    };

};

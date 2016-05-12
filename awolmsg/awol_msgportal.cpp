#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "awol_msgsvr.h"
#include "awol_error.h"
#include "awol_msgportal.h"

NS_BEGIN(awolmsg)


enum PortalOpType {
    PORTAL_OP_SEND = 0,
    PORTAL_OP_PUT = 1,
    PORTAL_OP_SYNC = 2,
    PORTAL_OP_RM = 3,
    PORTAL_OP_GET = 4,
};
static void _safe_portal_dispatch_onlist(const MsgActor & actor, int type, bool fromc, int ret, const MsgList & vms){
    MsgPortal * portal = MsgSvr::instance().find(actor, type);
    if (!portal){
        GLOG_ERR("msg portal not found when dispatching .... actor(%d:%d) type(%d)", actor.type(), actor.id(), type);
        return;
    }
    return portal->onlist(ret, vms, fromc);
}
static void _safe_portal_dispatch_onsend(const MsgActor & actor, int type, const MsgActor & actorto, bool fromc, int ret, uint64_t id, const string & msg){
    MsgPortal * portal = MsgSvr::instance().find(actor, type);
    if (!portal){
        GLOG_ERR("msg portal not found when dispatching .... actor(%d:%d) type(%d)", actor.type(), actor.id(), type);
        return;
    }
    return portal->onsend(ret, actorto, id, msg, fromc);
}
static void _safe_portal_dispatch_onput(const MsgActor & actor, int type, int ret, uint64_t id, const string & msg){
    MsgPortal * portal = MsgSvr::instance().find(actor, type);
    if (!portal){
        GLOG_ERR("msg portal not found when dispatching .... actor(%d:%d) type(%d)", actor.type(), actor.id(), type);
        return;
    }
    return portal->onput(ret, id, msg);
}
static void _safe_portal_dispatch_onsync(const MsgActor & actor, int type, int op, int ret, uint64_t id, const string & msg){
    MsgPortal * portal = MsgSvr::instance().find(actor, type);
    if (!portal){
        GLOG_ERR("msg portal not found when dispatching .... actor(%d:%d) type(%d)", actor.type(), actor.id(), type);
        return;
    }
    return portal->onsync(ret, id, msg, op);
}
static void _safe_portal_dispatch_onrm(const MsgActor & actor, int type, bool fromc, int ret, uint64_t id){
    MsgPortal * portal = MsgSvr::instance().find(actor, type);
    if (!portal){
        GLOG_ERR("msg portal not found when dispatching .... actor(%d:%d) type(%d)", actor.type(), actor.id(), type);
        return;
    }
    return portal->onremove(ret, id, fromc);
}
static void _safe_portal_dispatch_onget(const MsgActor & actor, int type, bool fromc, int ret, uint64_t id, const string & msg){
    MsgPortal * portal = MsgSvr::instance().find(actor, type);
    if (!portal){
        GLOG_ERR("msg portal not found when dispatching .... actor(%d:%d) type(%d)", actor.type(), actor.id(), type);
        return;
    }
    return portal->onget(ret, id, msg, fromc);
}


int MsgPortal::send(const MsgActor & actorto, const std::string & m, bool fromc){
	if (options().check(fromc)){
        auto cb = std::bind(_safe_portal_dispatch_onsend,
                    this->actor(), this->type(), actorto, fromc,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return msgbox_.send(actorto, m, cb);
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::put(const std::string & m) {//server
    auto cb = std::bind(_safe_portal_dispatch_onput,
        this->actor(), this->type(),
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    return msgbox_.put(m, cb);
}
int MsgPortal::sync(uint64_t id, const std::string & m, int op){
    auto cb = std::bind(_safe_portal_dispatch_onsync,
        this->actor(), this->type(), op,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    return msgbox_.sync(id, m, cb);
}
int MsgPortal::list(bool fromc){
	if (options().check(fromc, MSG_OPT_CPERM_LIST)){
        auto cb = std::bind(_safe_portal_dispatch_onlist,
            this->actor(), this->type(), fromc,
            std::placeholders::_1, std::placeholders::_2);
        return msgbox_.list(cb);
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::remove(uint64_t id, bool fromc){//client or server
	if (options().check(fromc, MSG_OPT_CPERM_REMOVE)){
        auto cb = std::bind(_safe_portal_dispatch_onrm,
            this->actor(), this->type(), fromc,
            std::placeholders::_1, std::placeholders::_2);
        return msgbox_.remove(id, cb);
    }
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::get(uint64_t id, bool fromc){
    auto cb = std::bind(_safe_portal_dispatch_onget,
        this->actor(), this->type(), fromc,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    return msgbox_.get(id, cb);
}

void MsgPortal::onremove(int ret, uint64_t id, bool fromc){
	//remove response to client
	GLOG_DBG("... ret:%d id:%lu fromc:%d", ret, id, fromc);
}
void MsgPortal::onsync(int ret, uint64_t id, const string & msg, int op){
	//update response to client
	GLOG_DBG("... update ret:%d id:%lu op:%d", ret, id, op);
}
void MsgPortal::onlist(int ret, const MsgList & vms, bool fromc){
	//list response to client
	GLOG_DBG("... ret:%d fromc:%d vms.size:%zd", ret, fromc, vms.size());
}
void MsgPortal::onnotify(uint64_t id, const std::string & msg){
	//new msg response to client
	GLOG_DBG("...notify msg id:%lu msg.length:%d", id, msg.length());
}
void MsgPortal::onsend(int ret, const MsgActor & actorto, uint64_t id, const std::string & msg, bool fromc){
	//new msg response to client
	GLOG_DBG("... ret:%d fromc:%d", ret, fromc);

}
void MsgPortal::onput(int ret, uint64_t id, const std::string & msg){
	//new msg response to client
	GLOG_DBG("... ret:%d msg.length:%d", ret, msg.length());
}
void MsgPortal::onget(int ret, uint64_t id, const std::string & msg, bool fromc){
    GLOG_DBG("... ret:%d id:%lu msg.length:%d", ret, id, msg.length());
}

MsgPortal::MsgPortal(const MsgActor & actor, int type): msgbox_(actor, type){
	MsgSvr::instance().regis(this);
}
MsgPortal::~MsgPortal(){
    MsgSvr::instance().unregis(this);
}

NS_END()

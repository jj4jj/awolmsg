#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "awol_msgsvr.h"
#include "awol_error.h"
#include "awol_msgportal.h"

NS_BEGIN(awolmsg)
int MsgPortal::sendto(const MsgActor & actorto, const std::string & m, bool fromc){
	if (options().check(fromc)){
        auto cb = std::bind(&MsgPortal::onsend, this, std::placeholders::_1, actorto, std::placeholders::_2, std::placeholders::_3, fromc);
        return msgbox_.send(actorto, m, cb);
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::put(const std::string & m) {//server
    auto cb = std::bind(&MsgPortal::onput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
	return msgbox_.put(m, cb);
}
int MsgPortal::sync(uint64_t id, const std::string & m, int op){
    auto cb = std::bind(&MsgPortal::onsync, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, op);
	return msgbox_.syncto(id, m, cb);
}
int MsgPortal::list(bool fromc){
	if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_LIST)){
        auto cb = std::bind(&MsgPortal::onlist, this, std::placeholders::_1, std::placeholders::_2, fromc);
		return msgbox_.list(cb);
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::remove(uint64_t id, bool fromc){//client or server
	if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_REMOVE)){
		if (fromc){
			auto cb = std::bind(&MsgPortal::onremove, this, std::placeholders::_1, id, fromc);
			return msgbox_.remove(id, cb);
		}
		else {
			auto cb = std::bind(&MsgPortal::onremove, this, std::placeholders::_1, id, fromc);
			return msgbox_.remove(id, cb);
		}
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::get(uint64_t id){
    return msgbox_.syncfrom(id, std::bind(&MsgPortal::onget, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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
void MsgPortal::onget(int ret, uint64_t id, const std::string & msg){
    GLOG_DBG("... ret:%d id:%lu msg.length:%d", ret, id, msg.length());
}

MsgPortal::MsgPortal(const MsgActor & actor, int type): msgbox_(actor, type){
	MsgSvr::instance().regis(this);
}
MsgPortal::~MsgPortal(){
}

NS_END()

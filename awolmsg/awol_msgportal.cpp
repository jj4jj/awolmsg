
#include "../base/stdinc.h"
#include "../base/logger.h"

#include "awol_msgportal.h"
#include "awol_msgsvr.h"


NS_BEGIN(awolmsg)
int MsgPortal::sendto(const MsgActor & actor, const std::string & m, bool fromc){
	if (options().check(fromc)){
		return msgbox_.send(actor, m, std::bind(&MsgPortal::onsend, this, std::placeholders::_1, fromc));
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::put(const std::string & m) {//server
	return msgbox_.put(m, std::bind(&MsgPortal::onput, this));
}
int MsgPortal::update(uint64_t id, const std::string & m, int op = 0){
	return msgbox_.update(id, m,
		std::bind(&MsgPortal::onupdate, this, std::placeholders::_1, op));
}
int MsgPortal::list(bool fromc = true){
	if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_LIST)){
		return msgbox_.list(std::bind(&MsgPortal::onlist, this));
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::remove(uint64_t id, bool fromc = true){//client or server
	if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_REMOVE)){
		if (fromc){
			return msgbox_.remove(id, std::bind(&MsgPortal::onremove, this));
		}
		else {
			return msgbox_.remove(id, std::bind(&MsgPortal::onremove, this));
		}
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
void MsgPortal::onremove(int ret, uint64_t id){
	//remove response to client
	GLOG_DBG("...");
}
void MsgPortal::onupdate(int ret, uint64_t id, int op){
	//update response to client
	GLOG_DBG("...");
}
void MsgPortal::onlist(int ret, bool fromc, const MsgList & vms){
	//list response to client
	GLOG_DBG("...");
}
void MsgPortal::onnotify(uint64_t id, const std::string & msg){
	//new msg response to client
	GLOG_DBG("...");
}

MsgPortal::MsgPortal(const MsgActor & actor, int type): msgbox_(actor, type){
	MsgSvr::instance().regis(this);
}
MsgPortal::~MsgPortal(){
}

NS_END()

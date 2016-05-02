#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "awol_msgsvr.h"
#include "awol_error.h"
#include "awol_msgportal.h"

NS_BEGIN(awolmsg)
int MsgPortal::sendto(const MsgActor & actor, const std::string & m, bool fromc){
	if (options().check(fromc)){
		auto cb = std::bind(&MsgPortal::onsend, this, std::placeholders::_1, fromc);
		return msgbox_.send(actor, m, cb);
	}
	else {
		return ErrorCode::AWOL_EC_NO_PERM;
	}
}
int MsgPortal::put(const std::string & m) {//server
	auto cb = std::bind(&MsgPortal::onput, this, std::placeholders::_1);
	return msgbox_.put(m, cb);
}
int MsgPortal::update(uint64_t id, const std::string & m, int op){
	auto cb = std::bind(&MsgPortal::onupdate, this, std::placeholders::_1, id, op);
	return msgbox_.update(id, m, cb);
}
int MsgPortal::list(bool fromc){
	if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_LIST)){
		auto cb = std::bind(&MsgPortal::onlist, this, std::placeholders::_1, fromc, std::placeholders::_2);
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
void MsgPortal::onremove(int ret, uint64_t id, bool fromc){
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
void MsgPortal::onsend(int ret, bool fromc){
	//new msg response to client
	GLOG_DBG("...");

}
void MsgPortal::onput(int ret){
	//new msg response to client
	GLOG_DBG("...");
}

MsgPortal::MsgPortal(const MsgActor & actor, int type): msgbox_(actor, type){
	MsgSvr::instance().regis(this);
}
MsgPortal::~MsgPortal(){
}

NS_END()

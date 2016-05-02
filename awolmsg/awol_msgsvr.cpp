#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "awol_msgsvr.h"

struct MsgSvrImpl {
	std::unordered_map<MsgKey, MsgPortal*>	recievers;
};
MsgSvr::~MsgSvr(){
	destory();
}

int MsgSvr::init(){
	if (impl){
		return -1;
	}
	impl = new MsgSvrImpl();
	/////////////////////////

	return 0;
}
int MsgSvr::destory(){
	if (impl){
		delete impl;
		impl = nullptr;
	}
	return 0;
}
int MsgSvr::regis(MsgPortal * portal){
	MsgKey k;
	k.actor.type = portal->msgbox().actor().type;
	k.actor.id = portal->msgbox().actor().id;
	k.type = portal->type();
	recievers[k] = portal;
	return 0;
}
int MsgSvr::update(){
#warning "todo"
	return 0;
}
#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/client/dccrpc.h"

#include "awol_msgportal.h"
#include "awol_msgsvr.h"

using dcrpc::RpcClient;
NS_BEGIN(awolmsg)
struct MsgSvrImpl {
	std::unordered_map<MsgKey, MsgPortal*>	recievers;
	dcrpc::RpcClient						rpc;
};
MsgSvr::~MsgSvr(){
	destory();
}

int MsgSvr::init(const string & svraddr){
	if (impl){
		return -1;
	}
	impl = new MsgSvrImpl();
	/////////////////////////
	impl->rpc.init(svraddr);
	return 0;
}
int MsgSvr::destory(){
	if (impl){
		impl->rpc.destroy();
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
	impl->recievers[k] = portal;
	return 0;
}
int MsgSvr::update(){
	rpc()->update();
	return 0;
}
RpcClient * MsgSvr::rpc(){
	return &impl->rpc;
}

NS_END()
#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "dcpots/base/msg_proto.hpp"
#include "dcpots/base/msg_buffer.hpp"
#include "dcpots/base/msg_buffer.hpp"
#include "dcpots/dcrpc/client/dccrpc.h"
#include "awol_msgportal.h"
#include "awol_msgsvr.h"

using dcrpc::RpcClient;
typedef msgproto_t<awolmsg::Msg>    MsgOPT;
using namespace dcsutil;

NS_BEGIN(awolmsg)
struct MsgSvrImpl {
	std::unordered_map<MsgKey, MsgPortal*>	recievers;
	dcrpc::RpcClient						rpc;
    msg_buffer_t                            msgbuff;
    std::vector<MsgPortal*>                 protals;
};
MsgSvr::~MsgSvr(){
	destory();
}
MsgSvr & MsgSvr::instance(){
	static MsgSvr msgsvr;
	return msgsvr;
}
//typedef std::function<void(int ret, const dcrpc::RpcValues &)>   RpcCallNotify;
int MsgSvr::init(const string & svraddr){
	if (impl){
		return -1;
	}
	impl = new MsgSvrImpl();
	/////////////////////////
	int ret = impl->rpc.init(svraddr);
    if (ret){
        return -2;
    }
    impl->msgbuff.create(1024 * 1024);
    //////////////////////////////////////////////////////////////////////
    impl->rpc.notify("msg", [this](int ret, const dcrpc::RpcValues & vals){
		if (ret){
			GLOG_ERR("msg notify ret :%d", ret);
			return;
		}
        MsgOPT msgop;
        if (!msgop.Unpack(vals.getb().data(), vals.getb().length())){
            GLOG_ERR("msg notify unpack error ! size:%d", vals.getb().length());
            return;
        }
		this->dispatch(msgop);
    });
    impl->protals.resize(awolmsg::MAX_MSG_TYPE_NUM);
	return 0;
}
void	MsgSvr::dispatch(const Msg & msgop){
	if (msgop.data_size() == 0){
		GLOG_ERR("msg notify data list is empty !");
		return;
	}
	MsgKey mk;
	mk.actor = msgop.actor();
	mk.type = msgop.type();
	auto it = this->impl->recievers.find(mk);
	if (it != this->impl->recievers.end()){
		MsgPortal * mp = it->second;
		mp->onnotify(msgop.data(0).id(), msgop.data(0).ext().version(), msgop.data(0).data());
	}
}

int MsgSvr::destory(){
	if (impl){
		impl->rpc.destroy();
        impl->msgbuff.destroy();
		delete impl;
		impl = nullptr;
	}
	return 0;
}
bool MsgSvr::ready(){
    return rpc()->ready();
}
MsgPortal * MsgSvr::find(const MsgActor & actor, int type){
    MsgKey k;
    k.actor = actor;
    k.type = type;
    auto it = impl->recievers.find(k);
    if (it != impl->recievers.end()){
        return it->second;
    }
    return nullptr;
}
void MsgSvr::unregis(MsgPortal * portal){
    GLOG_DBG("unregister msg portal actor(%d:%lu) type:%d pointer:%p",
        portal->msgbox().actor().type(),
        portal->msgbox().actor().id(), portal->type(), portal);
    MsgKey k;
    k.actor = portal->msgbox().actor();
    k.type = portal->type();
    impl->recievers.erase(k);
}

int MsgSvr::regis(MsgPortal * portal){
    GLOG_DBG("register msg portal actor(%d:%lu) type:%d pointer:%p",
        portal->msgbox().actor().type(),
        portal->msgbox().actor().id(), portal->type(), portal);
    MsgKey k;
	k.actor = portal->msgbox().actor();
	k.type = portal->type();
	impl->recievers[k] = portal;
    assert(k.type < awolmsg::MAX_MSG_TYPE_NUM);
    impl->protals[k.type] = portal;
	return 0;
}
int MsgSvr::update(){
	rpc()->update();
	return 0;
}
RpcClient * MsgSvr::rpc(){
	return &impl->rpc;
}
msg_buffer_t * MsgSvr::msg_buffer(){
    return &impl->msgbuff;
}
const MsgOptions & MsgSvr::options(int type){
    return impl->protals[type]->options();
}


NS_END()
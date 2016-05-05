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
        //SET MSG
        MsgOPT msgop;
        if (!msgop.Unpack(vals.getb().data(), vals.getb().length())){
            GLOG_ERR("msg notify unpack error ! size:%d", vals.getb().length());
            return;
        }
        if (msgop.data_size() == 0){
            GLOG_ERR("msg notify data list is empty !");
            return;
        }
        MsgKey mk;
        mk.actor.type = msgop.actor().type();
        mk.actor.id = msgop.actor().id();
        mk.type = msgop.type();

        auto it = this->impl->recievers.find(mk);
        if (it != this->impl->recievers.end()){
            MsgPortal * mp = it->second;
            mp->onnotify(msgop.data(0).id(), msgop.data(0).data());
        }

    });
	return 0;
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
void MsgSvr::unregis(MsgPortal * portal){
    GLOG_DBG("unregister msg portal actor(%d:%lu) type:%d pointer:%p",
        portal->msgbox().actor().type,
        portal->msgbox().actor().id, portal->type(), portal);
    MsgKey k;
    k.actor.type = portal->msgbox().actor().type;
    k.actor.id = portal->msgbox().actor().id;
    k.type = portal->type();
    impl->recievers.erase(k);
}

int MsgSvr::regis(MsgPortal * portal){
    GLOG_DBG("register msg portal actor(%d:%lu) type:%d pointer:%p",
        portal->msgbox().actor().type,
        portal->msgbox().actor().id, portal->type(), portal);
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
msg_buffer_t * MsgSvr::msg_buffer(){
    return &impl->msgbuff;
}

NS_END()
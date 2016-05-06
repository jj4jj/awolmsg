#include "dcpots/base/stdinc.h"
#include "dcpots/base/msg_buffer.hpp"
#include "dcpots/base/logger.h"
////////////////////////////////////////
#include "../awolmsg/awol_msgportal.h"
////////////////////////////////////////
#include "awolapp.pb.h"
#include "mailbox.h"
using namespace awolmsg;
NS_BEGIN(awolapp)
//using namespace std;




typedef awolmsg::MsgPortalT<Mail, MSG_TYPE_MAIL>  MailPortal;
struct MailBoxImpl : public MailPortal {
    MailBox * mailbox{ nullptr };
    MailBoxImpl(const MsgActor & actor, MailBox * forwarder) :MailPortal(actor), mailbox(forwarder){
    }
    const MsgOptions & options() const {
        static MsgOptions mo(MsgOptions::MSG_OPT_OWN_BO, MsgOptions::MSG_OPT_STORE_NONE,
            MsgOptions::MSG_OPT_CPERM_REMOVE | MsgOptions::MSG_OPT_CPERM_LIST |
            MsgOptions::MSG_OPT_CPERM_UPDATE);
        return mo;
    }
    int op(uint64_t id, MailOpCode op){
        auto it = msg_cache.find(id);
        if (it == msg_cache.end()){
            GLOG_ERR("op mail id:%lu not exist with op:%d", id, op);
            return ErrorCode::AWOL_EC_MSG_NOT_FOUND;
        }
        Mail & m = it->second;
        if (op == MAIL_OP_READ && m.state() == MAIL_STATE_INIT){
            m.set_state(MAIL_STATE_READ);
        }
        else if (op == MAIL_OP_FETCH && (m.state() == MAIL_STATE_READ || m.state() == MAIL_STATE_INIT)){
            m.set_state(MAIL_STATE_FETCH);
        }
        else {
            return ErrorCode::AWOL_EC_MSG_UPDATE_STATE_ERROR;
        }
        return MailPortal::sync(id, m, op);
    }
    void onsync(int ret, uint64_t id, int op){
        GLOG_DBG("update mail id:%lu op:%d error :%d", id, op, ret);
        if (ret){
            GLOG_ERR("update mail id:%lu op:%d error :%d", id, op, ret);
            return;
        }
        Mail * mail = getmsg(id);
        if (!mail){
            GLOG_ERR("msg:%lu not found ...", id);
            return;
        }
        if (op == MAIL_OP_READ){
            return onfetch(id, *mail);
        }
        else if (op == MAIL_OP_READ){
            return onread(id, *mail);
        }
    }

    void onremove(uint64_t id, const Mail & msg, bool fromc){
        GLOG_DBG("on mail remove id:%ld fromc:%d", id, fromc);
        mailbox->onremove(id, fromc);
    }
    virtual void onread(uint64_t id, const Mail & mail){
        GLOG_DBG("on mail read id:%id", id);
        mailbox->onread(id, mail);
    }
    virtual void onfetch(uint64_t id, const Mail & mail){
        GLOG_DBG("on mail fetch id:%id", id);
        mailbox->onfetch(id, mail);
    }    
    virtual void onnotify(uint64_t id, const Mail & mail){
        GLOG_DBG("notify new mail id:%id", id);
        mailbox->onnotify(id, mail);
    }
    virtual void onlist(bool fromc){
        GLOG_DBG("on mail list fromc:%d", fromc);
        mailbox->onlist(fromc);
        //send to client        
    }
    int request(const string & str){
        CSAwalMsg   msg;
        if (!msg.ParseFromString(str.data())){
            GLOG_ERR("parse from arrray error !");
            return -1;
        }
        auto & req = msg.request();
        const MsgAgent & magt = req.sendto();
        auto & mail = req.msg().mail();
        uint64_t mid = req.id();
        int opcode = req.opcode();
        switch (msg.cmd()){
        case CSAwalMsg::MSG_CMD_LIST:
            return list(true);
            break;
        case CSAwalMsg::MSG_CMD_SENDTO:
            return send(MsgActor(magt.type(), magt.id()), mail, true);
            break;
        case CSAwalMsg::MSG_CMD_UPDATE:
            return op(mid, (MailOpCode)opcode);
            break;
        case CSAwalMsg::MSG_CMD_REMOVE:
            return remove(mid, true);
            break;
#if 0
        case CSAwalMsg::MSG_OP_GET:
            return get(mid, true);
            break;
#endif
        default:
            return ErrorCode::AWOL_EC_MSG_ERROR_CMD;
        }
    }
};


///////////////////////////////////////////////////////////////////////////////////
MailBox::MailBox(const MsgActor & actor){
    impl = new MailBoxImpl(actor, this);
}
MailBox::~MailBox(){
    if (impl){
        delete impl;
    }
}
int MailBox::list(){
    return impl->list(false);
}
int MailBox::remove(uint64_t id){
    return impl->remove(id);
}
int MailBox::insert(const Mail & mail){
    return impl->put(mail);
}
int MailBox::request(const string & req){    
    return impl->request(req);
}
void MailBox::response(const string & resp){ //send to client actor
}
void MailBox::onremove(uint64_t id, bool fromc){
}
void MailBox::onread(uint64_t id, const Mail & mail){
}
void MailBox::onfetch(uint64_t id, const Mail & mail){
}
void MailBox::onnotify(uint64_t id, const Mail & mail){
    //send to client
    CSAwalMsg csmsg;
    csmsg.set_cmd(CSAwalMsg::MSG_CMD_NOTIFY);
    AwolMsgItem & msg = *csmsg.mutable_notify()->add_msglist();
    msg.set_id(id);
    msg.mutable_msg()->mutable_mail()->CopyFrom(mail);
    response(csmsg.SerializeAsString());
}
void MailBox::onlist(bool fromc){
    if (fromc){
        CSAwalMsg csmsg;
        csmsg.set_cmd(CSAwalMsg::MSG_CMD_LIST);
        auto it = impl->msg_cache.begin();
        while (it != impl->msg_cache.end()){
            AwolMsgItem & msg = *csmsg.mutable_response()->add_msglist();
            msg.set_id(it->first);
            msg.mutable_msg()->mutable_mail()->CopyFrom(it->second);
            ++it;
        }
        return response(csmsg.SerializeAsString());
    }
}

NS_END()
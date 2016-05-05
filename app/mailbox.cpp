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
    enum MailOpType {
        MAIL_OP_READ = 1,
        MAIL_OP_FETCH = 2,
    };
    const MsgOptions & options() const {
        static MsgOptions mo(MsgOptions::MSG_OPT_OWN_BO, MsgOptions::MSG_OPT_STORE_NONE,
            MsgOptions::MSG_OPT_CPERM_REMOVE | MsgOptions::MSG_OPT_CPERM_LIST |
            MsgOptions::MSG_OPT_CPERM_UPDATE);
        return mo;
    }
    int op(uint64_t id, MailOpType op){
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

    void onremove(int ret, uint64_t id, bool fromc){
        GLOG_DBG("on mail remove id:%ld ret:%d fromc:%d", id, ret, fromc);
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
int MailBox::list(bool fromc){
    return impl->list(fromc);
}
int MailBox::remove(uint64_t id){
    return impl->remove(id);
}
int MailBox::read(uint64_t id){
    return impl->op(id, MailBoxImpl::MAIL_OP_READ);
}
int MailBox::fetch(uint64_t id){
    return impl->op(id, MailBoxImpl::MAIL_OP_FETCH);
}
int MailBox::insert(const Mail & mail){
    return impl->put(mail);
}
void MailBox::onremove(uint64_t id, bool fromc){
}
void MailBox::onread(uint64_t id, const Mail & mail){
}
void MailBox::onfetch(uint64_t id, const Mail & mail){
}
void MailBox::onnotify(uint64_t id, const Mail & mail){
}

NS_END()
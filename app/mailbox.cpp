#include "dcpots/base/stdinc.h"
#include "dcpots/base/msg_buffer.hpp"
#include "dcpots/base/logger.h"
////////////////////////////////////////
#include "../awolmsg/awol_msgportal.h"
////////////////////////////////////////
#include "awolapp.pb.h"
#include "awolappi.hpp"

#include "mailbox.h"
using namespace awolmsg;

NS_BEGIN(awolapp)
//using namespace std;
typedef AwolAppImpl<Mail, MSG_TYPE_MAIL, MailBox>   AwolAppMailBoxImpl;
struct MailBoxImpl : public AwolAppMailBoxImpl {
    MailBoxImpl(const MsgActor & actor, MailBox * mb) :AwolAppMailBoxImpl(actor, mb){}
    const MsgOptions & options() const {
        static MsgOptions mo(MsgOptions::MSG_OPT_OWN_BO, MsgOptions::MSG_OPT_STORE_NONE,
            MsgOptions::MSG_OPT_CPERM_REMOVE | MsgOptions::MSG_OPT_CPERM_LIST |
            MsgOptions::MSG_OPT_CPERM_UPDATE);
        return mo;
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
    GLOG_IFO("recv client request msg: [%s]", req.c_str());
    return impl->request(req);
}
void MailBox::response(const string & resp){ //send to client actor
    GLOG_IFO("send to client response msg:[%s]", resp.c_str());
}
void MailBox::onread(uint64_t id, const Mail & mail){
}
void MailBox::onfetch(uint64_t id, const Mail & mail){
}
void MailBox::onop_did(uint64_t id, const Mail & mail, int op){
    if (op == MAIL_OP_READ){
        GLOG_DBG("on mail fetch id:%id", id);
        this->onfetch(id, mail);
    }
    else if (op == MAIL_OP_READ){
        GLOG_DBG("on mail read id:%id", id);
        this->onread(id, mail);
    }
}
int MailBox::onop_do(uint64_t id, Mail & m, int op){
    if (op == MAIL_OP_READ && m.state() == MAIL_STATE_INIT){
        m.set_state(MAIL_STATE_READ);
    }
    else if (op == MAIL_OP_FETCH && (m.state() == MAIL_STATE_READ || m.state() == MAIL_STATE_INIT)){
        m.set_state(MAIL_STATE_FETCH);
    }
    else {
        return ErrorCode::AWOL_EC_MSG_UPDATE_STATE_ERROR;
    }
    return 0;
}
void MailBox::set_msg(AwolMsg & msg, const Mail & mail){
    msg.mutable_mail()->CopyFrom(mail);
}
const Mail & MailBox::get_msg(const AwolMsg & msg){
    return msg.mail();
}
void  MailBox::onnotify(uint64_t id, const Mail & mail){

}


NS_END()
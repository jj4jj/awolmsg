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
        static MsgOptions mo(MSG_OPT_OWN_BO, MSG_OPT_STORE_MYSQL,
            MSG_OPT_CPERM_REMOVE | MSG_OPT_CPERM_LIST |
            MSG_OPT_CPERM_UPDATE);
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
    return impl->list();
}
int MailBox::remove(uint64_t id){
    return impl->remove(id);
}
int MailBox::insert(const Mail & mail){
    return impl->put(mail);
}
int MailBox::sendto(const MsgActor & actor, const awolapp::Mail & mail){
	return MailBoxImpl::sendto(actor, mail);
}
int MailBox::request(const string & req){    
    return impl->request(req);
}
void MailBox::response(const string & resp){ //send to client actor
}
void MailBox::onread(uint64_t id, const Mail & mail){
}
void MailBox::onfetch(uint64_t id, const Mail & mail){
}
void MailBox::onmsgop(uint64_t id, const Mail & mail, int op){
    if (op == MAIL_OP_READ){
        GLOG_DBG("on mail fetch id:%id", id);
        this->onfetch(id, mail);
    }
    else if (op == MAIL_OP_READ){
        GLOG_DBG("on mail read id:%id", id);
        this->onread(id, mail);
    }
}
int MailBox::checkop(uint64_t id, Mail & m, int op){
    if (op == MAIL_OP_READ && m.state() == MAIL_STATE_INIT){
        m.set_state(MAIL_STATE_READ);
    }
    else if (op == MAIL_OP_FETCH && 
			(m.state() == MAIL_STATE_READ || m.state() == MAIL_STATE_INIT)){
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
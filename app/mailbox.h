#pragma  once
#include "dcpots/base/msg_buffer.hpp"
#include "../include/awol_msgportal.h"
#include "app.pb.h"
namespace awolapp {
    typedef awolmsg::MsgPortalT<Mail, MSG_TYPE_MAIL>  MailPortal;
    struct MailBox : public MailPortal {
        MailBox(const MsgActor & actor) :MailPortal(actor){
        }
        enum {
            UPDATE_READ = 1,
            UPDATE_FETCH = 2,
        };
        const MsgOptions & options() const {
            static MsgOptions mo(MsgOptions::MSG_OPT_OWN_BO, MsgOptions::MSG_OPT_STORE_NONE,
                MsgOptions::MSG_OPT_CPERM_REMOVE | MsgOptions::MSG_OPT_CPERM_LIST | 
                MsgOptions::MSG_OPT_CPERM_UPDATE);
            return mo;
        }
        int sync(uint64_t id, int op){
            auto it = msg_cache.find(id);
            if (it == msg_cache.end()){
                return ErrorCode::AWOL_EC_MSG_NOT_FOUND;
            }
            Mail & m = it->second;
            if (op == UPDATE_READ && m.state() == MAIL_STATE_INIT){
                m.set_state(MAIL_STATE_READ);
            }
            else if (op == UPDATE_FETCH && (m.state() == MAIL_STATE_READ || m.state() == MAIL_STATE_INIT)){
                m.set_state(MAIL_STATE_FETCH);
            }
            else {
                return ErrorCode::AWOL_EC_MSG_UPDATE_STATE_ERROR;
            }
            return MailPortal::sync(id, m, op);
        }
        void onremove(int ret, uint64_t id, bool fromc /* = true */){
            if (!ret){
                msg_cache.erase(id);
            }
        }
        virtual void onread(uint64_t id, const Mail & mail){
            GLOG_DBG("on mail read id:%id", id);
        }
        virtual void onfetch(uint64_t id, const Mail & mail){
            GLOG_DBG("on mail fetch id:%id", id);
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
            if (op == UPDATE_READ){
                return onfetch(id, *mail);
            }
            else if (op == UPDATE_READ){
                return onread(id, *mail);
            }
        }

    };

}
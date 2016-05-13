#pragma once
//an awolapp implementation frame .. just using in compiling unit (.cpp)
#include "dcpots/base/stdinc.h"
#include "dcpots/base/msg_buffer.hpp"
#include "dcpots/base/logger.h"
////////////////////////////////////////
#include "../awolmsg/awol_msgportal.h"
////////////////////////////////////////
#include "awolapp.pb.h"
using namespace awolmsg;
NS_BEGIN(awolapp)
//using namespace std;


template<class AppMsg, int AppMsgType, class App>
struct AwolAppImpl : public awolmsg::MsgPortalT<AppMsg, AppMsgType> {
    enum {
        AWOL_MAX_MSG_BUFF_SIZE = 256 * 1024,
    };
    typedef awolmsg::MsgPortalT<AppMsg, AppMsgType>  AppPortal;
    App * app_{ nullptr };
    msg_buffer_t    msgbuff_;
    AwolAppImpl(const MsgActor & actor, App * forwarder) :AppPortal(actor), app_(forwarder){
    }
    virtual ~AwolAppImpl(){
        msgbuff_.destroy();
    }
    void onsync(int ret, uint64_t id, int op){
        GLOG_DBG("update app msg id:%lu op:%d error :%d", id, op, ret);
        if (ret){
            GLOG_ERR("update app msg id:%lu op:%d error :%d", id, op, ret);
            return;
        }
        AppMsg * appmsg = this->getmsg(id);
        if (!appmsg){
            GLOG_ERR("app type(%d) msg:%lu not found ...", AppMsgType, id);
            return;
        }
        app_->onop_did(id, *appmsg, op);
    }
    int onsynop(uint64_t id, AppMsg & m, int op){
        return app_->onop_do(id, m, op);
    }
    int request(const string & str){
        CSAwalMsg   msg;
        if (!msg.ParseFromString(str.data())){
            GLOG_ERR("parse from arrray error !");
            return -1;
        }
        auto & req = msg.request();
        const MsgAgent & magt = req.sendto();
        auto & appmsg = app_->get_msg(req.msg());
        uint64_t mid = req.id();
        int opcode = req.opcode();
        switch (msg.cmd()){
        case CSAwalMsg::MSG_CMD_LIST:
            return AppPortal::list(true);
        case CSAwalMsg::MSG_CMD_SENDTO:
            return AppPortal::send(MsgActor(magt.type(), magt.id()), appmsg, true);
        case CSAwalMsg::MSG_CMD_UPDATE:
            return AppPortal::sync(mid, (MailOpCode)opcode);
        case CSAwalMsg::MSG_CMD_REMOVE:
            return AppPortal::remove(mid, true);
        case CSAwalMsg::MSG_CMD_GET:
            return AppPortal::get(mid, true);
        default:
            return ErrorCode::AWOL_EC_MSG_ERROR_CMD;
        }
    }
    void pushmsg(const CSAwalMsg & msg){
        if (!msgbuff_.buffer){
            msgbuff_.create(AWOL_MAX_MSG_BUFF_SIZE);//256K
        }
        if (!msg.SerializeToArray(msgbuff_.buffer, msgbuff_.max_size)){
            GLOG_ERR("pack msg error msg size:%d", msg.ByteSize());
        }
        return app_->response(string(msgbuff_.buffer, msg.ByteSize()));
    }
    void onremove(uint64_t id, const AppMsg & msg, bool fromc){
        GLOG_DBG("on app msg remove id:%ld fromc:%d", id, fromc);
        if (this->options().owner() != MSG_OPT_OWN_SO){
            CSAwalMsg csmsg;
            csmsg.set_cmd(CSAwalMsg::MSG_CMD_NOTIFY);
            csmsg.mutable_notify()->set_sync(CSAwalMsg::MSG_SYNC_REMOVE);
            AwolMsgItem & msg = *csmsg.mutable_notify()->add_msglist();
            msg.set_id(id);
            pushmsg(csmsg);
        }
    }
    virtual void onnotify(uint64_t id, uint32_t version, const AppMsg & msg){
        GLOG_DBG("notify new app msg id:%id", id);
        if (this->options().owner() != MSG_OPT_OWN_SO){
            app_->onnotify(id, msg);
            CSAwalMsg csmsg;
            csmsg.set_cmd(CSAwalMsg::MSG_CMD_NOTIFY);
            csmsg.mutable_notify()->set_sync(CSAwalMsg::MSG_SYNC_RECV);
            AwolMsgItem & awolmsgitem = *csmsg.mutable_notify()->add_msglist();
            awolmsgitem.set_id(id);
            app_->set_msg(*awolmsgitem.mutable_msg(), msg);
            pushmsg(csmsg);
        }
    }
    virtual void onlist(bool fromc){
        GLOG_DBG("on app msg list fromc:%d", fromc);
        if (fromc){
            CSAwalMsg csmsg;
            csmsg.set_cmd(CSAwalMsg::MSG_CMD_LIST);
            auto it = this->msg_cache.begin();
            while (it != this->msg_cache.end()){
                AwolMsgItem & msg = *csmsg.mutable_response()->add_msglist();
                msg.set_id(it->first);
                app_->set_msg(*msg.mutable_msg(), it->second.second);
                ++it;
            }
            pushmsg(csmsg);
        }
    }
};


NS_END()
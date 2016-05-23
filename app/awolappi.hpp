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
	int	list_msg_limit{ 0 };
	int	list_msg_offset{ 0 };
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
		if (op > 0 && this->options().owner() != MSG_OPT_OWN_SO){
			response_msg(CSAwolMsg::MSG_CMD_UPDATE, id, *appmsg, op);
		}
		app_->onmsgop(id, *appmsg, op);
    }
    int checkop(uint64_t id, AppMsg & m, int op){
		return app_->checkop(id, m, op);
    }
	int list(){
		return AppPortal::list(false);
	}
	int list(int limit , int offset){
		list_msg_limit = limit;
		list_msg_offset = offset;
		if (AppPortal::msg_cache.empty()){
			return AppPortal::list(true);			
		}
		else {
			onlist(true);
		}
		return 0;
	}
    int request(const string & str){
        CSAwolMsg   msg;
        if (!msg.ParseFromString(str)){
            GLOG_ERR("request parse from arrray error !");
            return -1;
        }
		GLOG_DBG("actor(%s) request msg:[%s]", AppPortal::actor().ShortDebugString().c_str(),
			msg.ShortDebugString().c_str());
        auto & req = msg.request();
        const MsgAgent & magt = req.sendto();
        auto & appmsg = app_->get_msg(req.msg());
        uint64_t mid = req.id();
        int opcode = req.opcode();
		int ret = 0;
        switch (msg.cmd()){
		case CSAwolMsg::MSG_CMD_LIST:
			ret = list(msg.cookie().limit(), msg.cookie().offset());
			break;
        case CSAwolMsg::MSG_CMD_SENDTO:
			ret = AppPortal::send(MsgActor(magt.type(), magt.id()), appmsg, true);
			break;
		case CSAwolMsg::MSG_CMD_UPDATE:
			ret = AppPortal::sync(mid, (MailOpCode)opcode);
			break;
		case CSAwolMsg::MSG_CMD_REMOVE:
			ret = AppPortal::remove(mid, true);
			break;
		case CSAwolMsg::MSG_CMD_GET:
			ret = AppPortal::get(mid, true);
			break;
		default:
			ret = ErrorCode::AWOL_EC_MSG_ERROR_CMD;
			break;
		}
		if (ret){//response error
			response_msg(ret, msg.cmd(), mid);
		}
		return ret;
    }
    void push_msg(const CSAwolMsg & msg){
        if (!msgbuff_.buffer){
            msgbuff_.create(AWOL_MAX_MSG_BUFF_SIZE);//256K
        }
        if (!msg.SerializeToArray(msgbuff_.buffer, msgbuff_.max_size)){
            GLOG_ERR("pack msg error msg size:%d", msg.ByteSize());
			return;
        }
		GLOG_DBG("actor(%s) response msg:[%s]", 
			AppPortal::actor().ShortDebugString().c_str(), msg.ShortDebugString().c_str());
        return app_->response(string(msgbuff_.buffer, msg.ByteSize()));
    }
    void onremove(uint64_t id, const AppMsg & msg, bool fromc){
        GLOG_DBG("on app msg remove id:%ld fromc:%d", id, fromc);
		if (this->options().owner() != MSG_OPT_OWN_SO){
			if (!fromc){
				notify_msg(CSAwolMsg::MSG_SYNC_REMOVE, id, &msg);
			}
			else {
				response_msg(0, CSAwolMsg::MSG_CMD_REMOVE, id, &msg);
			}
        }
    }
	void onsend(const MsgActor & actorto, uint64_t id, const AppMsg & msg){
		response_msg(0, CSAwolMsg::MSG_CMD_SENDTO, id, &msg);
	}
    virtual void onput(uint64_t id, const AppMsg & msg){
        GLOG_DBG("onput new app msg id:%ld", id);
        app_->onnotify(id, msg);
        if (this->options().owner() != MSG_OPT_OWN_SO){
            notify_msg(CSAwolMsg::MSG_SYNC_RECV, id, &msg);
        }
    }
    virtual void onnotify(uint64_t id, const AppMsg & msg){
        GLOG_DBG("notify new app msg id:%ld", id);
        app_->onnotify(id, msg);
        if (this->options().owner() != MSG_OPT_OWN_SO){
			notify_msg(CSAwolMsg::MSG_SYNC_RECV, id, &msg);
        }
    }
	virtual void onget(uint64_t id, const AppMsg & msg, bool fromc){
		if (this->options().owner() != MSG_OPT_OWN_SO){
			if (!fromc){
				notify_msg(CSAwolMsg::MSG_SYNC_RECV, id, &msg);
			}
			else {
				response_msg(0, CSAwolMsg::MSG_CMD_GET, id, &msg);
			}
		}
	}
	void response_msg(int ret, CSAwolMsg::MsgCMD cmd, uint64_t id, const AppMsg * appmsg = nullptr, int op = 0){
		CSAwolMsg csmsg;
		csmsg.set_cmd(cmd);
		csmsg.mutable_response()->set_ret(ret);
		AwolMsgItem & awolmsgitem = *csmsg.mutable_response()->add_msglist();
		awolmsgitem.set_id(id);
		if (appmsg){
			app_->set_msg(*awolmsgitem.mutable_msg(), *appmsg);
		}
		csmsg.mutable_response()->set_op(op);
		push_msg(csmsg);
	}
	void notify_msg(CSAwolMsg::AwolMsgSyncCode code, uint64_t id, const AppMsg * appmsg = nullptr){
		CSAwolMsg csmsg;
		csmsg.set_cmd(CSAwolMsg::MSG_CMD_NOTIFY);
		csmsg.mutable_notify()->set_sync(code);
		AwolMsgItem & awolmsgitem = *csmsg.mutable_notify()->add_msglist();
		awolmsgitem.set_id(id);
		if (appmsg){
            app_->set_msg(*awolmsgitem.mutable_msg(), *appmsg);
		}
		push_msg(csmsg);
	}
    virtual void onlist(bool fromc){
        GLOG_DBG("on app msg list fromc:%d", fromc);
        if (fromc){
            CSAwolMsg csmsg;
            csmsg.set_cmd(CSAwolMsg::MSG_CMD_LIST);
			csmsg.mutable_response()->set_ret(0);
            auto it = this->msg_cache.begin();
			int list_msg_idx = 0;
			int list_msg_cnt = 0;
            while (it != this->msg_cache.end()){
				if (list_msg_idx >= list_msg_offset){
					if (list_msg_cnt < list_msg_limit || list_msg_limit == 0){
						list_msg_cnt++;
						AwolMsgItem & msg = *csmsg.mutable_response()->add_msglist();
						msg.set_id(it->first);
                        app_->set_msg(*msg.mutable_msg(), it->second.second);
					}
					else {
						break;
					}
				}
                ++it;
				++list_msg_idx;
            }
            push_msg(csmsg);
        }
    }
};


NS_END()
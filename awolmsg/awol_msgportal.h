#pragma once
#include "awol_msgbox.h"
namespace awolmsg {
	typedef ::google::protobuf::Message  MsgT;
    struct MsgPortal {
		virtual const MsgOptions & options() const = 0;
		int sendto(const MsgActor & actor, const MsgT & m, bool fromc = true){
			if (options().check(fromc)){
				return msgbox_.send(actor,  m).result(
					[](int ret){
					GLOG_DBG("msg send ok  : %d ...", ret);
				}).error([](int ret){
					GLOG_ERR("msg send error :%d ...", ret);
				}).ret;
			}
			else {
				return ErrorCode::AWOL_EC_NO_PERM;
			}
		}
		virtual int put(const MsgT & m) {//server
			string ms = m.SerializeAsString();
			return msgbox_.put(ms);
		}
		virtual int update(uint64_t id, const MsgT & m, int op = 0){
			string ms = m.SerializeAsString();
			return msgbox_.update(id, ms).result([this, id, op](int ret){
				this->onupdate(ret, id, op);
			}).ret;
		}
		virtual int list(bool fromc = true){
			if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_LIST)){
				return msgbox_.list().result(
					[this, fromc](int ret, std::vector<std::string> & vs){
					std::vector<MsgT> vm;
					for (int i = 0; i < vs.size(); ++i){
						MsgT m;
						m.ParseFromString(vs[i]);
						vm.push_back(m)
					}
					this->onlist(ret, vm, fromc);
				}).ret;
			}
			else {
				return ErrorCode::AWOL_EC_NO_PERM;
			}
		}
		virtual int remove(uint64_t id, bool fromc = true){//client or server
			if (options().check(fromc, MsgOptions::MSG_OPT_CPERM_REMOVE)){
				if (fromc){
					return msgbox_.remove(id).result(
						[this, id](int ret){
						this->onremove(ret, id);
					}).ret;
				}
				else {
					return msgbox_.remove(id).result(
							[this, id](int ret){this->onremove(ret, id);}
						).ret;
				}
			}
			else {
				return ErrorCode::AWOL_EC_NO_PERM;
			}
		}
	public:
		virtual void onremove(int ret, uint64_t id){
			//remove response to client
		}
		virtual void onupdate(int ret, uint64_t id, int op){
			//update response to client
		}
		virtual void onlist(int ret, bool fromc, const std::vector<MsgT> * vms){
			//list response to client
		}
		virtual void onnotify(uint64_t id, const MsgT & msg){
			//new msg response to client
		}

	public:
		MsgPortal(const MsgActor & actor, int type) :msgbox_(actor, type){
			msgbox_.recv().result(this->onnotify);
		}
		int type() const { return msgbox_.type(); };
		virtual ~MsgPortal(){}
		MsgBox &  msgbox();

    private:
		MsgBox    msgbox_;
    };
}

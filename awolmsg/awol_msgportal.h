#pragma once
#include <string>
#include "dcpots/base/noncopyable.h"
#include "dcpots/base/msg_buffer.hpp"
#include "awol_msgbox.h"
#include "awol_msgsvr.h"
#include "awol_error.h"

namespace awolmsg {

struct MsgPortal : noncopyable {
	virtual const MsgOptions & options() const = 0;
public:
	virtual int send(const MsgActor & actor, const std::string & m, bool fromc = true);
	virtual int put(const std::string & m);//server
    virtual int sync(uint64_t id, uint32_t verison, const std::string & m, int op = 0);
	virtual int list(bool fromc = true);
    virtual int remove(uint64_t id, uint32_t verison, bool fromc = true);//client or server
    virtual int get(uint64_t id, bool fromc = true);
    virtual int clean();
    virtual int request(const std::string & str);
public:
	static  int sendto(const MsgActor & actorto, int type, const std::string & m);
public:
    virtual void onget(int ret, uint64_t id, uint32_t verison, const string & msg, bool fromc);
    virtual void onremove(int ret, uint64_t id, bool fromc);
    virtual void onsync(int ret, uint64_t id, const string & msg, int op);
    virtual void onlist(int ret, const MsgList & vms, bool fromc);
    virtual void onnotify(uint64_t id, uint32_t verison, const std::string & msg);
    virtual void onsend(int ret, const MsgActor & actorto, uint64_t id, const std::string & msg, bool fromc);
    virtual void onput(int ret, uint64_t id, const std::string & msg);
    virtual void onclean(int ret);

public:
	MsgPortal(const MsgActor & actor, int type);
	int type() const { return msgbox_.type(); };
	const MsgActor & actor() const { return msgbox_.actor(); };
	virtual ~MsgPortal();
	MsgBox &  msgbox(){ return msgbox_; }
protected:

private:
	MsgBox    msgbox_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<class MsgT, int MsgTypeV>
struct MsgPortalT : public MsgPortal {
    typedef std::pair<uint64_t, MsgT>   VMsgT;
    typedef std::vector<VMsgT>          VMsgTList;
public:
	static int 	sendto(const MsgActor & actorto, const MsgT & m){
		msg_buffer_t & msgbuff = *MsgSvr::instance().msg_buffer();
		if (!m.SerializeToArray(msgbuff.buffer, msgbuff.max_size)){
			GLOG_ERR("send to msg pack error ! size:%d", m.ByteSize());
			return ErrorCode::AWOL_EC_ERROR_PACK;
		}
		return MsgPortal::sendto(actorto, MsgTypeV, std::string(msgbuff.buffer, m.ByteSize()));
	}
	bool	lock(uint64_t id){
		uint32_t time_now = dcsutil::time_unixtime_s();
		auto it = msg_locking.find(id);
		static const int MSG_LOCKING_TIME = 5;
		if (it == msg_locking.end()){
			msg_locking[id] = time_now + MSG_LOCKING_TIME;
			return true;
		}
		else if (time_now > it->second){
			it->second = time_now + MSG_LOCKING_TIME;
			return true;
		}
		return false;
	}
	void	unlock(uint64_t id){
		msg_locking.erase(id);
	}
    const MsgT * find_msg(uint64_t msgid){
        auto it0 = msg_cache.find(msgid);
        if (it0 == msg_cache.end()){
            return nullptr;
        }
        return &it0->second.second;
    }
    uint64_t     get_msg_id_by_idx(int idx){
        auto it0 = msg_cache.begin();
        std::advance(it0, idx);
        if (it0 == msg_cache.end()){
            return 0;
        }
        return it0->first;
    }
    const MsgT * get_msg_by_idx(int idx){
        auto it0 = msg_cache.begin();
        std::advance(it0, idx);
        if (it0 == msg_cache.end()){
            return nullptr;
        }
        return &it0->second.second;
    }
    int          get_msg_count(){
        return msg_cache.size();
    }

    virtual int remove(uint64_t id, bool fromc = true){//client or server
        auto it = msg_cache.find(id);
        if (it == msg_cache.end()){
            GLOG_ERR("remove msg id:%lu not exist with fromc:%d", id, fromc);
            return ErrorCode::AWOL_EC_MSG_NOT_FOUND;
        }
		if (!lock(id)){
			return ErrorCode::AWOL_EC_MSG_OP_RETRY_LATER;
		}
        return MsgPortal::remove(id, it->second.first, fromc);
    }
    virtual int send(const MsgActor & actor, const MsgT & m, bool fromc = true){
        msg_buffer_t & msgbuff = *MsgSvr::instance().msg_buffer();
        if (!m.SerializeToArray(msgbuff.buffer, msgbuff.max_size)){
            GLOG_ERR("send to msg pack error ! size:%d", m.ByteSize());
            return ErrorCode::AWOL_EC_ERROR_PACK;
        }
        return MsgPortal::send(actor, std::string(msgbuff.buffer, m.ByteSize()), fromc);
    }
    virtual int put(const MsgT & m){//server
        msg_buffer_t & msgbuff = *MsgSvr::instance().msg_buffer();
        if (!m.SerializeToArray(msgbuff.buffer, msgbuff.max_size)){
            GLOG_ERR("send to msg pack error ! size:%d", m.ByteSize());
            return ErrorCode::AWOL_EC_ERROR_PACK;
        }
        return MsgPortal::put(std::string(msgbuff.buffer, m.ByteSize()));
    }
    virtual int sync(uint64_t id, int op = 0){
        auto it = msg_cache.find(id);
        if (it == msg_cache.end()){
            GLOG_ERR("op mail id:%lu not exist with op:%d", id, op);
            return ErrorCode::AWOL_EC_MSG_NOT_FOUND;
        }
		if (!lock(id)){
			return ErrorCode::AWOL_EC_MSG_OP_RETRY_LATER;
		}
        uint32_t ver = it->second.first;
        MsgT & m = it->second.second;
        int ret = checkop(id, m, op);
        if (ret){
            GLOG_ERR("check op:%d error ret:%d", op, ret);
            return ret;
        }
        msg_buffer_t & msgbuff = *MsgSvr::instance().msg_buffer();
        if (!m.SerializeToArray(msgbuff.buffer, msgbuff.max_size)){
            GLOG_ERR("send to msg pack error ! size:%d", m.ByteSize());
            return ErrorCode::AWOL_EC_ERROR_PACK;
        }
        return MsgPortal::sync(id, ver+1, std::string(msgbuff.buffer, m.ByteSize()), op);
    }
public:
    virtual void onget(uint64_t id, const MsgT & msg, bool fromc){
        GLOG_DBG("on get msg id:%lu [%s]", id, msg.ShortDebugString().c_str());
    }
    virtual void onsend(const MsgActor & actorto, uint64_t id, const MsgT & msg){
        GLOG_DBG("on send to actor(%d:%d) msg id:%lu [%s]",
            actorto.type(), actorto.id(), id, msg.ShortDebugString().c_str());
    }
    virtual void onput(uint64_t id, const MsgT & msg){
        GLOG_DBG("on put msg id:%lu [%s]", id, msg.ShortDebugString().c_str());
    }
    virtual void onnotify(uint64_t id, const MsgT & msg){
        GLOG_DBG("on notify msg id:%lu [%s]!", id, msg.ShortDebugString().c_str());
    }
    virtual void onlist(bool fromc){
        GLOG_DBG("on list msg size:%zd !", msg_cache.size());
    }
    virtual void onremove(uint64_t id, const MsgT & msg, bool fromc){
        GLOG_DBG("on remove msg id:%d [%s]", id, msg.ShortDebugString().c_str());
    }
    virtual void onsync(uint64_t id, const MsgT & msg, int op){
        GLOG_DBG("on update msg id:%d op:%d [%s]", id, op, msg.ShortDebugString().c_str());
    }
    virtual int  checkop(uint64_t id, MsgT & msg, int op){
        if (op != 0){
            GLOG_ERR("op = %d id:%lu not permitted !", op, id);
            return ErrorCode::AWOL_EC_MSG_UPDATE_OP_ERROR;
        }
        return 0;
    }
public:
    virtual void onclean(int ret){
        GLOG_DBG("on clean ret", ret);
        if (ret){
            GLOG_ERR("onclean msg error :%d", ret);
            return;
        }
        msg_cache.clear();
    }
    virtual void onget(int ret, uint64_t id, uint32_t version, const string & msg, bool fromc){
        if (ret){
            GLOG_ERR("onget msg error :%d", ret);
            return;
        }
		if (msg.empty()){
			GLOG_IFO("get data id = %lu is empty fromc:%d !", id, fromc);
			return;
		}
        MsgT mm;
        if (!mm.ParseFromArray(msg.data(), msg.length())){
            GLOG_ERR("parse from array error ! length:%d", msg.length());
			return;
        }
        msg_cache[id] = std::make_pair(version, mm);
        onget(id, mm, fromc);
    }
    virtual void onsend(int ret, const MsgActor & actorto, uint64_t id, const std::string & msg){
        if (ret){
            GLOG_ERR("onput msg error :%d", ret);
            return;
        }
        MsgT mm;
        if (!mm.ParseFromArray(msg.data(), msg.length())){
            GLOG_ERR("parse from array error ! length:%d", msg.length());
			return;
        }
		onsend(actorto, id, mm);
    }
    virtual void onsync(int ret, uint64_t id, const std::string & msg, int op){
		unlock(id);
		if (ret){
            GLOG_ERR("onsync msg error :%d", ret);
            return;
        }
        auto it = msg_cache.find(id);
        if (it == msg_cache.end()){
            GLOG_ERR("onsync msg not found id = %lu on sync op = %d", id, op);
            return;
        }
        //update version
        it->second.first = it->second.first + 1;
        MsgT mm;
        if (!mm.ParseFromArray(msg.data(), msg.length())){
            GLOG_ERR("onsync parse from array error ! length:%d", msg.length());
			return;
        }
        onsync(id, mm, op);
    }
    virtual void onput(int ret, uint64_t id, const std::string & msg){
        if (ret){
            GLOG_ERR("onput msg error :%d", ret);
            return;
        }
        MsgT mm;
        if (!mm.ParseFromArray(msg.data(), msg.length())){
            GLOG_ERR("parse from array error ! length:%d", msg.length());
			return;
        }
        msg_cache[id] = std::make_pair(0, mm);
        onput(id, mm);
    }
    virtual void onlist(int ret, const MsgList & vms, bool fromc){
        if (ret){
            GLOG_ERR("onlist error ret:%d", ret);
            return;
        }
        msg_cache.clear();
        for (int i = 0; i < vms.size(); ++i){
            const MsgKeyData & msg = vms.at(i);
            MsgT mm;
            if (!mm.ParseFromArray(msg.data.data(), msg.data.length())){
                GLOG_ERR("parse from array error ! length:%d", msg.data.length());
				continue;
            }
            assert(msg.id > 0);
            msg_cache[msg.id] = std::make_pair(msg.version, mm);
            GLOG_DBG("msg actor:(%d:%lu)insert msg id:%lu version:%d",
                actor().type(), actor().id(), msg.id, msg.version);
        }
        onlist(fromc);
    }
    virtual void onremove(int ret, uint64_t id, bool fromc = true){
		unlock(id);
		GLOG_DBG("on remove msg ret:%d id:%lu fromc:%d", ret, id, fromc);
        if (ret){
            GLOG_ERR("on remove msg ret:%d id:%lu fromc:%d", ret, id, fromc);
            return;
        }
        auto pmsg = getmsg(id);
        if (!pmsg){
            GLOG_ERR("on remove msg not found ret:%d id:%lu fromc:%d", ret, id, fromc);
            return;
        }
        onremove(id, *pmsg, fromc);
        msg_cache.erase(id);
    }
    virtual void onnotify(uint64_t id, uint32_t version, const string & msg){
        MsgT mm;
        if (!mm.ParseFromArray(msg.data(), msg.length())){
            GLOG_ERR("notify msg id:%lu unpack erorr len:%zd !", id, msg.length());
            return;
        }
        msg_cache[id] = std::make_pair(version ,mm);
        return onnotify(id, mm);
    }

    MsgT * getmsg(uint64_t id){
        auto it = msg_cache.find(id);
        if (it == msg_cache.end()){
            return nullptr;
        }
        return &(it->second.second);
    }
public:
    MsgPortalT(const MsgActor & actor) :MsgPortal(actor, MsgTypeV){}
	virtual ~MsgPortalT(){ msg_cache.clear(); msg_locking.clear();}
public:
    std::unordered_map<uint64_t, std::pair<uint32_t, MsgT> >   msg_cache;
	std::unordered_map<uint64_t, uint32_t>					   msg_locking;
};


}

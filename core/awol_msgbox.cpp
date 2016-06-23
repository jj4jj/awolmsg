#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "dcpots/base/dcseqnum.hpp"
#include "dcpots/base/msg_proto.hpp"
////////////////////////////////////////
#include "dcpots/dcrpc/client/dccrpc.h"
////////////////////////////////////////
#include "awol_error.h"
#include "awol_msgsvr.h"

#include "awol_msgbox.h"

using namespace std;
using namespace google::protobuf;
using dcrpc::RpcClient;
using dcrpc::RpcValues;

typedef msgproto_t<awolmsg::Msg> MsgOPT;
#define RPC		(MsgSvr::instance().rpc())
#define OPTIONS (MsgSvr::instance().options(this->type()))

NS_BEGIN(awolmsg)
MsgBox::MsgBox(const MsgActor & actor, int t_):actor_(actor),type_(t_){}
typedef dcsutil::sequence_number_t<8,24> msgsn;

static inline int construct_simple_MsgOPT(char * msgbuff, int msgbufflen, const MsgOptions & options, MsgOP op,
    const MsgActor & actor_to, int type, uint64_t  msgid = 0){
    MsgOPT msg;
    msg.mutable_flag()->CopyFrom(options);
    msg.mutable_actor()->CopyFrom(actor_to);
    msg.set_type(type);
    if (msgid > 0){
        auto rmsg = msg.add_data();
        rmsg->set_id(msgid);
    }
    msg.set_op(op);
    if (!msg.Pack(msgbuff, msgbufflen)){
        GLOG_ERR("pack message error ! msg size:%d buff size:%d",
            msg.ByteSize(), msgbufflen);
        return -1;
    }
    return msgbufflen;
}

static inline int construct_mmsg_MsgOPT(char * msgbuff, int msgbufflen, const MsgOptions & options, MsgOP op,
    const MsgActor & actor_to, int type, const string & m_ = "", uint64_t  msgid = 0,
    uint32_t version = 0, const MsgActor & actor_from = MsgActor::empty){
	MsgOPT msg;
    msg.mutable_flag()->CopyFrom(options);
    msg.mutable_actor()->CopyFrom(actor_to);
	msg.set_type(type);
    auto rmsg = msg.add_data();
    if (msgid == 0){
        rmsg->set_id(msgsn::next());
    }
    else {
        rmsg->set_id(msgid);
    }
    rmsg->set_data(m_.data(), m_.length());
    if (actor_from.type() > 0 || actor_from.id() > 0){
        rmsg->mutable_ext()->mutable_from()->CopyFrom(actor_from);
    }
    if (version > 0){
        rmsg->mutable_ext()->set_version(version);
    }
	msg.set_op(op);
    if (!msg.Pack(msgbuff, msgbufflen)){
		GLOG_ERR("pack message error ! msg size:%d buff size:%d",
			msg.ByteSize(), msgbufflen);
		return -1;
	}
    return msgbufflen;
}
int MsgBox::list(MsgListCallBack lcb){
	dcrpc::RpcValues args;
	char buff[256];
	int ibuff = construct_simple_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_LIST, actor_, type_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	return RPC->call("msg", args, [lcb, this](int ret, const RpcValues & result){
		GLOG_DBG("list ret:%d result length:%d", ret, result.length());
		if (!lcb){
			return;
		}
		MsgList msglist;
		if (ret == 0){
			awolmsg::Msg::MsgData mdata;
			for (int i = 0; i < result.length(); ++i){
				if (mdata.ParseFromArray(result.getb(i).data(), result.getb(i).length())){
					msglist.push_back(MsgKeyData(mdata.id(), mdata.data(), mdata.ext().version()));
				}
				else {
                    GLOG_ERR("actor(%s) onlist unpack msg error idx:%d data length:%zu!", 
                             actor_.ShortDebugString().c_str(), i, result.getb(i).length());
				}
			}
		}
		lcb(ret, msglist);
	});
}
int	MsgBox::get(uint64_t uid, MsgGetCallBack cb){
    dcrpc::RpcValues args;
    char buff[256];
    int ibuff = construct_simple_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_GET, actor_, type_, uid);
    if (ibuff < 0){
        GLOG_ERR("actor(%s) msg pack error !", actor_.ShortDebugString().c_str());
        return -1;
    }
    args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid, this](int ret, const RpcValues & result) {
        GLOG_DBG("actor(%s) get ret:%d result length:%d", actor_.ShortDebugString().c_str(), ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("actor(%s) sync from error :%d result length:%d", actor_.ShortDebugString().c_str(), ret, result.length());
            return cb(-1, MsgKeyData(uid, "", 0));
        }
        else {
            if (result.length() == 0){
                return cb(0, MsgKeyData(uid, "", 0));
            }
            else {
                awolmsg::Msg::MsgData mdata;
                if (!mdata.ParseFromArray(result.getb().data(), result.getb().length())){
                    GLOG_ERR("actor(%s) unpack msg data error ! ", actor_.ShortDebugString().c_str());
                    return cb(-2, MsgKeyData(uid, "", 0));
                }
                return cb(0, MsgKeyData(mdata.id(), mdata.data(), mdata.ext().version()));
            }
        }
    });
}
int MsgBox::sendto(const MsgActor & sendto, int type, const std::string & m_){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
	int ibuff = construct_mmsg_MsgOPT(buff, sizeof(buff), MsgSvr::instance().options(type), MSG_OP_INSERT, sendto, type, m_);
	if (ibuff < 0){
		GLOG_ERR("sendto actor(%s) type:%d msg pack error msg length:%d!",
			sendto.ShortDebugString().c_str(), type, m_.length());
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [](int ret, const RpcValues & result) {
		GLOG_DBG("sendto rpc call ret:%d result length:%d", ret, result.length());
		if (ret){
			GLOG_ERR("rpc sendto msg error ! ret:%d !", ret);
			return;
		}
		else {
			awolmsg::Msg recvmsg;
			if (!recvmsg.ParseFromString(result.getb())){
				GLOG_ERR("unpack msg error ! data :%d", result.getb().length());
				return;
			}
			if (recvmsg.data_size() == 0){
				GLOG_ERR("sendto msg reply msg length error !");
				return;
			}
			else { //send to msgsvr dispatching
				GLOG_DBG("sendto msg success msgsvr dispatching ...");
				MsgSvr::instance().dispatch(recvmsg);
			}
		}
	});
}
int	MsgBox::send(const MsgActor & sendto, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256*1024];
    int ibuff = construct_mmsg_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_INSERT, sendto, type_, m_, 0, 0, actor_);
	if (ibuff < 0){
        GLOG_ERR("actor(%s) msg pack error !", actor_.ShortDebugString().c_str());
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, this](int ret, const RpcValues & result) {
        GLOG_DBG("actor(%s) send ret:%d result length:%d", actor_.ShortDebugString().c_str(), ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("actor(%s) rpc set msg error ! ret:%d !", actor_.ShortDebugString().c_str(), ret);
            cb(-1, 0, "");
            return;
        }
        else {
            MsgOPT recvmsg;
            if (!recvmsg.Unpack(result.getb().data(), result.getb().length())){
                GLOG_ERR("actor(%s) unpack msg error ! data :%d", actor_.ShortDebugString().c_str(), result.getb().length());
                cb(-2, 0, "");
                return;
            }
            if (recvmsg.data_size() == 0){
                cb(-3, 0, "");
                return;
            }
            else {
                cb(0, recvmsg.data(0).id(), recvmsg.data(0).data());
            }
        }
	});
}
int	MsgBox::put(const string & m, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
    int ibuff = construct_mmsg_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_INSERT, actor_, type_, m, 0, 0, actor_);
	if (ibuff < 0){
        GLOG_ERR("actor(%s) msg pack error !", actor_.ShortDebugString().c_str());
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, this](int ret, const RpcValues & result) {
        GLOG_DBG("actor(%s) put ret:%d result length:%d", actor_.ShortDebugString().c_str(), ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("actor(%s) rpc set msg error ! ret:%d !", actor_.ShortDebugString().c_str(), ret);
            cb(-1, 0, "");
            return;
        }
        else {
            MsgOPT recvmsg;
            if (!recvmsg.Unpack(result.getb().data(), result.getb().length())){
                GLOG_ERR("actor(%s) unpack msg error ! data :%d", actor_.ShortDebugString().c_str(), result.getb().length());
                cb(-2, 0, "");
                return;
            }
            if (recvmsg.data_size() == 0){
                cb(-3, 0, "");
                return;
            }
            else {
                cb(0, recvmsg.data(0).id(), recvmsg.data(0).data());
            }
        }
	});
}
int	MsgBox::remove(uint64_t uid, uint32_t version, MsgOPCallBack cb){ //remove one msg
	dcrpc::RpcValues args;
	char buff[256];
    int ibuff = construct_mmsg_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_RM, actor_, type_, "", uid , version);
	if (ibuff < 0){
        GLOG_ERR("actor(%s) msg pack error !", actor_.ShortDebugString().c_str());
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid, this](int ret, const RpcValues & result) {
        GLOG_DBG("actor(%s) remove ret:%d result length:%d", actor_.ShortDebugString().c_str(), ret, result.length());
		if (!cb){
			return;
		}
        if (ret || result.geti() <= 0){
            GLOG_ERR("actor(%s) rpc set msg error ! ret:%d geti:%d error:%s", actor_.ShortDebugString().c_str(),
                     ret, result.geti(), result.gets().c_str());
            cb(-1, uid, "");
        }
        else {
            cb(0, uid,  "");
        }
	});
	return 0;
}

int	MsgBox::sync(uint64_t uid, uint32_t version, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
    int ibuff = construct_mmsg_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_SET, actor_, type_, m_, uid, version);
	if (ibuff < 0){
        GLOG_ERR("actor(%s) msg pack error !", actor_.ShortDebugString().c_str());
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid, m_, this](int ret, const RpcValues & result) {
        GLOG_DBG("sync ret:%d result length:%d", ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("rpc set msg error ! ret:%d !", ret);
            cb(-1, 0, "");
            return;
        }
        else {
            if (result.geti() <= 0){
                cb(-2, uid, m_);
                return;
            }
            else {
                cb(0, uid, m_);
                return;
            }
        }
	});
}
int	MsgBox::clean(MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256];
    int ibuff = construct_simple_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_RM, actor_, type_);
	if (ibuff < 0){
        GLOG_ERR("actor(%s) msg pack error !", actor_.ShortDebugString().c_str());
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb](int ret, const RpcValues & result){
		GLOG_DBG("clean ret:%d result length:%d", ret, result.length());
        if (!cb){
            return;
        }
        if (ret || result.geti() <= 0){
            GLOG_ERR("rpc set msg error ! ret:%d geti:%d error:%s", ret, result.geti(), result.gets().c_str());
            cb(-1, 0, "");
        }
        else {
            cb(0, 0, "");
        }
	});
}

NS_END()

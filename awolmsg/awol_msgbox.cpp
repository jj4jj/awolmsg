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
static inline int construct_MsgOPT(char * msgbuff, int msgbufflen, const MsgOptions & options, MsgOP op,
    const MsgActor & actor, int type, const string & m_ = "",
    uint64_t  msgid = 0, const MsgActor & actor_from = MsgActor::empty){
	MsgOPT msg;
    msg.mutable_flag()->CopyFrom(options);
	msg.mutable_actor()->set_type(actor.type());
    msg.mutable_actor()->set_id(actor.id());
	msg.set_type(type);
	if (m_.length() > 0 || msgid > 0){
		auto rmsg = msg.add_data();
		if (msgid == 0){
			rmsg->set_id(msgsn::next());
		}
		else {
			rmsg->set_id(msgid);
		}
		if (m_.length() > 0){
			rmsg->set_data(m_.data(), m_.length());
		}
        if (actor_from.type() > 0 || actor.id() > 0){
            rmsg->mutable_ext()->mutable_from()->set_type(actor_from.type());
            rmsg->mutable_ext()->mutable_from()->set_id(actor_from.id());
		}
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
	int ret = 0;
	dcrpc::RpcValues args;
	char buff[256];
	int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_LIST, actor_, type_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	return RPC->call("msg", args, [lcb](int ret, const RpcValues & result){
		GLOG_DBG("list ret:%d result length:%d", ret, result.length());
		if (!lcb){
			return;
		}
		MsgList msglist;
		awolmsg::Msg::MsgData mdata;
		for (int i = 0; i < result.length(); ++i){
			mdata.ParseFromArray(result.getb(i).data(), result.getb(i).length());
			msglist.push_back(MsgKeyData(mdata.id(), mdata.data()));
		}
		lcb(ret, msglist);
	});
}
int	MsgBox::get(uint64_t uid, MsgOPCallBack cb){
    dcrpc::RpcValues args;
    char buff[256];
    int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_GET, actor_,
        type_, "", uid);
    if (ibuff < 0){
        GLOG_ERR("msg pack error !");
        return -1;
    }
    args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid](int ret, const RpcValues & result){
        GLOG_DBG("get ret:%d result length:%d", ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("sync from error :%d result length:%d", ret, result.length());
            return cb(-1, uid, "");
        }
        else {
            if (result.length() == 0){
                return cb(0, uid, "");
            }
            else {
                return cb(0, uid, result.getb());
            }
        }
    });
}
int	MsgBox::send(const MsgActor & sendto, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256*1024];
    int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_INSERT, sendto,
		type_, m_, 0, actor_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb](int ret, const RpcValues & result){
        GLOG_DBG("send ret:%d result length:%d", ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("rpc set msg error ! ret:%d !", ret);
            cb(-1, 0, "");
            return;
        }
        else {
            MsgOPT recvmsg;
            if (!recvmsg.Unpack(result.getb().data(), result.getb().length())){
                GLOG_ERR("unpack msg error ! data :%d", result.getb().length());
                cb(-2, 0, "");
                return;
            }
            if (recvmsg.data_size() == 0){
                cb(-3, 0, "");
                return;
            }
            if (recvmsg.data(0).id() == 0){
                cb(-4, 0, "");
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
    int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_INSERT, actor_,
		type_, m, 0, actor_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb](int ret, const RpcValues & result){
        GLOG_DBG("put ret:%d result length:%d", ret, result.length());
        if (!cb){
            return;
        }
        if (ret){
            GLOG_ERR("rpc set msg error ! ret:%d !", ret);
            cb(-1, 0, "");
            return;
        }
        else {
            MsgOPT recvmsg;
            if (!recvmsg.Unpack(result.getb().data(), result.getb().length())){
                GLOG_ERR("unpack msg error ! data :%d", result.getb().length());
                cb(-2, 0, "");
                return;
            }
            if (recvmsg.data_size() == 0){
                cb(-3, 0, "");
                return;
            }
            if (recvmsg.data(0).id() == 0){
                cb(-4, 0, "");
            }
            else {
                cb(0, recvmsg.data(0).id(), recvmsg.data(0).data());
            }
        }
	});
}
int	MsgBox::remove(uint64_t uid, MsgOPCallBack cb){ //remove one msg
	dcrpc::RpcValues args;
	char buff[256];
    int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_RM, actor_,
		type_, "", uid);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid](int ret, const RpcValues & result){
        GLOG_DBG("remove ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
        if (ret || result.geti() <= 0){
            GLOG_ERR("rpc set msg error ! ret:%d geti:%d error:%s", ret, result.geti(), result.gets().c_str());
            cb(-1, uid, "");
        }
        else {
            cb(0, uid,  "");
        }
	});
	return 0;
}

int	MsgBox::sync(uint64_t uid, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
    int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_SET, actor_,
		type_, m_, uid);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid, m_](int ret, const RpcValues & result){
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
    int ibuff = construct_MsgOPT(buff, sizeof(buff), OPTIONS, MSG_OP_RM, actor_,
		type_, "", 0);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
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

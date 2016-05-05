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

NS_BEGIN(awolmsg)
MsgBox::MsgBox(const MsgActor & actor, int t_):actor_(actor),type_(t_){}
typedef dcsutil::sequence_number_t<8,24> msgsn;
static inline int constructMsgOpT(char * msgbuff, int msgbufflen, MsgOP op,
	const MsgActor & actor, int type, const string & m_="",
	uint64_t  msgid=0,const MsgActor & actor_from=MsgActor::empty){
	MsgOPT msg;
	msg.mutable_actor()->set_type(actor.type);
	msg.mutable_actor()->set_id(actor.id);
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
		if (actor_from.type > 0 || actor.id > 0){
			rmsg->mutable_ext()->mutable_from()->set_type(actor_from.type);
			rmsg->mutable_ext()->mutable_from()->set_id(actor_from.id);
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
	int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_LIST, actor_, type_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	return RPC->call("msg", args, [lcb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
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
int	MsgBox::send(const MsgActor & sendto, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256*1024];
    int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_SET, sendto,
		type_, m_, 0, actor_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
        MsgOPT recvmsg;
        if (!recvmsg.Unpack(result.getb().data(), result.getb().length())){
            GLOG_ERR("unpack msg error ! ");
        }
        cb(ret, recvmsg.data(0).id(), recvmsg.data(0).data());
	});
}
int	MsgBox::put(const string & m, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
    int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_SET, actor_,
		type_, m, 0, actor_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
		if (result.geti() <= 0){
			GLOG_ERR("set msg error ! ");
		}
        MsgOPT recvmsg;
        if (!recvmsg.Unpack(result.getb().data(), result.getb().length())){
            GLOG_ERR("unpack msg error ! ");
        }
		cb(ret, recvmsg.data(0).id(), recvmsg.data(0).data());
	});
}
int	MsgBox::remove(uint64_t uid, MsgOPCallBack cb){ //remove one msg
	dcrpc::RpcValues args;
	char buff[256];
    int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_RM, actor_,
		type_, "", uid);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
		cb(ret, uid, "");
	});
	return 0;
}
int	MsgBox::syncfrom(uint64_t uid, MsgOPCallBack cb){
    dcrpc::RpcValues args;
    char buff[256];
    int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_GET, actor_,
        type_, "", uid);
    if (ibuff < 0){
        GLOG_ERR("msg pack error !");
        return -1;
    }
    args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid](int ret, const RpcValues & result){
        GLOG_IFO("ret:%d result length:%d", ret, result.length());
        if (ret){
            GLOG_ERR("sync from error :%d result length:%d", ret, result.length());
        }
        if (!cb){
            return;
        }
        if (result.length() == 0){
            return cb(ret, uid, "");
        }
        else {
            return cb(ret, uid, result.getb());
        }
    });
}
int	MsgBox::syncto(uint64_t uid, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
    int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_SET, actor_,
		type_, m_, uid);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb, uid, m_](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
		if (!cb){
			return;
		}
		cb(ret, uid, m_);
	});
}
int	MsgBox::clean(MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256];
    int ibuff = constructMsgOpT(buff, sizeof(buff), MSG_OP_RM, actor_,
		type_, "", 0);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
    return RPC->call("msg", args, [cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
		if (!cb){
			return;
		}
		cb(ret, 0, "");
	});
}

NS_END()

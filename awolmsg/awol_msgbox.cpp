#pragma  once
#include <vector>
#include "awol_msg.h"
#include "awol_error.h"
#include "awol_msgsvr.h"
#include "dcpots/base/dcseqnum.hpp"
///////////////////////////////////
#include "../dcrpc/client/dccrpc.h"
///////////////////////////////////
#include "awol_msgbox.h"

using namespace std;
using namespace google::protobuf;
using namespace dcsutil::dcrpc;

typedef msgproto_t<awolmsg::Msg> MsgOPT;
#define RPC		MsgSvr::instance().rpc()

NS_BEGIN(awolmsg)
MsgBox::MsgBox(const MsgActor & actor, int t_):actor_(actor),
		type_(t_){}{
}
typedef dcsutil::sequence_number_t msgsn;
static inline int constructMsgOpT(char * msgbuff, int msgbufflen, MsgOP op,
	const MsgActor & actor, int type, int subtype=0, const string & m_="",
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
		rmsg->set_subtype(subtype);
		if (m_.length() > 0){
			rmsg->set_data(m_.data(), m_.length());
		}
		if (actor_from.type > 0 || actor.id > 0){
			rmsg->mutable_ext()->mutable_from()->set_type(actor_from.type);
			rmsg->mutable_ext()->mutable_from()->set_id(actor_from.id);
		}
	}
	msg.set_op(op);
	if (!msg.Pack(msgbuff, sizeof(msgbufflen))){
		GLOG_ERR("pack message error ! msg size:%d buff size:%d",
			msg.ByteSize(), msgbufflen);
		return -1;
	}
	return msg.ByteSize();
}
int MsgBox::list(MsgListCallBack lcb){
	int ret = 0;
	dcrpc::RpcValues args;
	char buff[256];
	int ibuff = constructMsgOpT(buff, sizeof(ibuff), MSG_OP_LIST, actor_, type_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	RPC->call("msg", args, [this, lcb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!lcb){
			return;
		}
		std::vector<awolmsg::Msg::MsgData>	msg_datas;
		awolmsg::Msg::MsgData mdata;
		for (int i = 0; i < result.length; ++i){
			mdata.Clear();
			mdata.ParseFromArray(result.getb(i).data(), result.getb(i).length());
			msg_datas.push_back(mdata);
		}
		lcb(ret, msg_datas);
	});
}
int	MsgBox::send(const MsgActor & sendto, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256*1024];
	int ibuff = constructMsgOpT(buff, sizeof(ibuff), MSG_OP_SET, sendto,
		type_, 0, m_, 0, actor_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	RPC->call("msg", args, [this, cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
		cb(ret);
	});
}
int	MsgBox::put(const string & m, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
	int ibuff = constructMsgOpT(buff, sizeof(ibuff), MSG_OP_SET, actor_,
		type_, 0, m_, 0, actor_);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	RPC->call("msg", args, [this](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
		cb(ret);
	});
}
int	MsgBox::remove(uint64_t uid, MsgOPCallBack cb){ //remove one msg
	dcrpc::RpcValues args;
	char buff[256];
	int ibuff = constructMsgOpT(buff, sizeof(ibuff), MSG_OP_RM, actor_,
		type_, 0, "", uid);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	RPC->call("msg", args, [this, cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (!cb){
			return;
		}
		cb(ret);
	});
	return 0;
}
int	MsgBox::update(uint64_t uid, const string & m_, MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256 * 1024];
	int ibuff = constructMsgOpT(buff, sizeof(ibuff), MSG_OP_SET, actor_,
		type_, 0, m_, uid);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	RPC->call("msg", args, [this, cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
		if (!cb){
			return;
		}
		cb(ret);
	});
}
int	MsgBox::clean(MsgOPCallBack cb){
	dcrpc::RpcValues args;
	char buff[256];
	int ibuff = constructMsgOpT(buff, sizeof(ibuff), MSG_OP_RM, actor_,
		type_, 0, "", 0);
	if (ibuff < 0){
		GLOG_ERR("msg pack error !");
		return -1;
	}
	args.addb(buff, ibuff);
	RPC->call("msg", args, [this, cb](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
		if (result.geti() <= 0){
			GLOG_ERR("set msg error");
		}
		if (!cb){
			return;
		}
		cb(ret);
	});
}

NS_END()

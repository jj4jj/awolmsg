#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/client/dccrpc.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/base/dcseqnum.hpp"
#include "../proto/awolmsg.pb.h"
#include "dcpots/base/msg_buffer.hpp"
#include "dcpots/base/msg_proto.hpp"
using namespace std;
//using namespace google::protobuf;
using namespace dcsutil;
using namespace dcrpc;
using namespace awolmsg;
int main(){
	RpcClient cli;
	if (cli.init("127.0.0.1:8888")){
		return -1;
	}

	RpcValues args;
	while (!cli.ready()){
		cli.update();
	}


	typedef msgproto_t<awolmsg::Msg> MsgOPT;
	MsgOPT msg;
	msg_buffer_t buff;
	buff.create(1024 * 4);
	msg.mutable_actor()->set_type(1);
	msg.mutable_actor()->set_id(1234);
	msg.set_type(1223);
	auto rmsg = msg.add_data();
	rmsg->set_subtype(23);
	rmsg->set_id(232407236 + rand());
	rmsg->set_data("Hello,World!");

	msg.set_op(MSG_OP_LIST);
	msg.Pack(buff);
	args.addb(buff.buffer, buff.valid_size);

	cli.call("msg", args, [](int ret,  const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
	});

	msg.set_op(MSG_OP_SET);
	msg.Pack(buff);
	args.setb(buff.buffer, buff.valid_size);
	cli.call("msg", args, [](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
	});

	msg.set_op(MSG_OP_GET);
	msg.Pack(buff);
	args.setb(buff.buffer, buff.valid_size);
	cli.call("msg", args, [](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
	});


	msg.set_op(MSG_OP_RM);
	msg.Pack(buff);
	args.setb(buff.buffer, buff.valid_size);
	cli.call("msg", args, [](int ret, const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
	});

	while (true){
		cli.update();
		usleep(100000);
	}
	return 0;
}

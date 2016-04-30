#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/client/dccrpc.h"
#include "dcpots/base/cmdline_opt.h"
using namespace std;
//using namespace google::protobuf;
using namespace dcsutil;
using namespace dcrpc;

int main(){
	RpcClient cli;
	if (cli.init("127.0.0.1:8888")){
		return -1;
	}

	RpcValues args;
	args.adds("list");
	args.addi(1);
	args.addi(1);
	args.addi(1);

	while (!cli.ready()){
		cli.update();
	}

	cli.call("msg", args, [](int ret,  const RpcValues & result){
		GLOG_IFO("ret:%d result length:%d", ret, result.length());
	});
	

	return 0;
}
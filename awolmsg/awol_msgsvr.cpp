#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "awol_msgsvr.h"
struct MsgSvrImpl {
	std::unordered_map<int, MsgOptions>	register_msg_options;
};
MsgSvr::~MsgSvr(){
	destory();
}

int MsgSvr::init(){
	if (impl){
		return -1;
	}
	impl = new MsgSvrImpl();
	/////////////////////////

	return 0;
}
int MsgSvr::destory(){
	if (impl){
		delete impl;
		impl = nullptr;
	}
	return 0;
}
int MsgSvr::regis(int type, const MsgOptions & mo){
	auto it = impl->register_msg_options.find(type);
	if (it == impl->register_msg_options.end()){
		impl->register_msg_options[type] = mo;
	}
	else{
		return -1;
	}
	return 0;
}
int MsgSvr::update(){
#warning "todo"
	return 0;
}
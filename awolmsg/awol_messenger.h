#pragma once
#include "awol_msg.h"
namespace awolmsg {
struct Msg;
struct MessengerImpl;
class Messenger {
public:
    virtual ~Messenger();
public:
    int init(int type, const MsgOptions & opts);
    int type();
public:
    //server real do (batch mode todo)
    int send(const MsgActor & to, const Msg & msg, const MsgExt * ext=nullptr);
    int remove(const MsgActor & actor, uint64_t msg_id);
    int update(const MsgActor & actor, uint64_t msg_id, const Msg & msg, const MsgExt * ext=nullptr );
    int list(const MsgActor & actor, int offset=0, int limit=0);
public:
    //server side response or notify
    virtual int onSNotify(const MsgActor & actor, Msg & msg);
    virtual int onSSend(int ret, const MsgActor & actor,const Msg & msg);
    virtual int onSRemove(int ret, const MsgActor & actor, const MsgActor & msg);
    virtual int onSList(int ret, const MsgActor & actor, std::vector<Msg> & msg_list);
    virtual int onSUpdate(int ret, const MsgActor & actor, Msg & msg);
public:
    //client side request
    virtual int onCList(const MsgActor & actor);
    virtual int onCSend(const MsgActor & actor, const Msg & msg);
    virtual int onCRemove(const MsgActor & actor, uint64_t msg_id);
    virtual int onCUpdate(const MsgActor & actor, uint64_t msg_id, int op);

public:
    MessengerImpl* impl;
};




}

#pragma once

namespace awolmssg {
struct MsgActor {
    int type{0};
    uint64_t id{0};
};

sturct Msg;
struct MsgProcessorOptions {
    enum {
        MSG_OPT_OWN_BO=0,//both
        MSG_OPT_OWN_SO=1,//server only
        MSG_OPT_OWN_CO=2,//client only
    };
    enum {
        MSG_OPT_STORE_HASH=0,//hash
    };
    enum {
        MSG_OPT_FLAG_NONE=0,//flags
    };
    int owner{0}; //server only ? client only ? both ?
    int store{0}; //list ? hash ? set ?
    int flags{0}; //other ext
 };
struct MessengerImpl;
class Messenger {
public:
    Messenger(int type);
    virtual ~Messenger();
public:
    int init();
    int type();
public:
    //server real do (batch mode todo)
    int send(const MsgActor & to, int type, const Msg & msg, const MsgExt * ext=nullptr);
    int remove(const MsgActor & actor, int type, uint64_t msg_id);
    int update(const MsgActor & actor, int type, uint64_t msg_id, const Msg & msg, const MsgExt * ext=nullptr );
    int list(const MsgActor & actor, int type, int offset=0, int limit=0);
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

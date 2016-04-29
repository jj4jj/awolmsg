#pragma once

namespace awolmsg {
struct MsgActor {
    int type{0};
    uint64_t id{0};
};

struct Msg;
struct MsgOptions {
    enum {
        MSG_OPT_OWN_BO=0,//both
        MSG_OPT_OWN_SO=1,//server only
        MSG_OPT_OWN_CO=2,//client only
    };
    enum {
        MSG_OPT_STORE_NONE = 0,//expiration ?
        MSG_OPT_STORE_TTL = 1,//expiration ?
        MSG_OPT_STORE_SHIFT = 2,//shift when full
    };
    enum {
        MSG_OPT_CPERM_NONE = 0,//flags
        MSG_OPT_CPERM_SEND = 1,//flags
        MSG_OPT_CPERM_REMOVE = 2,//flags
        MSG_OPT_CPERM_LIST = 4,//flags
        MSG_OPT_CPERM_UPDATE = 8,//flags
        MSG_OPT_CPERM_ALL=0xF, //PERM
    };
    int owner{0}; //server only ? client only ? both ?
    int store{0}; //list ? hash ? set ?
    int cperm{0}; //other ext
    MsgOptions(int own = 0, int store = 0, int perm = 0) : owner(own), store(store), cperm(perm){
    }
 };
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

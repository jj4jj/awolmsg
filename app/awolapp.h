#pragma once
template<class AppMsg, int AppMsgType>
struct AwolApp {
    typedef AwolAppImpl<AppMsg, AppMsgType, AwolApp>    AwolAppImplT;
    AwolAppImplT * impl{ nullptr };
    AwolApp(const MsgActor & actor){ impl = new AwolAppImplT(actor, this); }
    virtual ~AwolApp(){ if (impl){ delete impl; } }
public:
    int list(){return impl->list(false);}
    int remove(uint64_t id){return impl->remove(id);}
    int insert(const AppMsg & msg){ return impl->put(msg); }
    int send(const MsgActor & actorto, const AppMsg & appmsg){ return impl->send(actorto, appmsg, false); }
    int update(uint64_t id, int op){ return impl->sync(id, op); }
    int pull(uint64_t id){ return impl->get(id, false); }

public:
    int request(const string & req){GLOG_TRA("recv client request msg: [%d]", req.length ()); return impl->request(req);}
public:
    virtual void onop(AppMsg & msg, int op);
	virtual void checkop(AppMsg & msg, int op);
	virtual void response(const string & resp) = 0; //send to client actor
    virtual void set_msg(AwolMsg & msg, const AppMsg & appmsg) = 0;
    virtual const AppMsg & get_msg(const AwolMsg & msg) = 0;
};

NS_END()


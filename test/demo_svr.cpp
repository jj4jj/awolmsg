#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "../awolmsg/awol_msg.h"
#include "../awolmsg/awol_msgsvr.h"
using namespace awolmsg;
using namespace std;
#if 0
#include "test.pb.h"

enum  AwolMsgType {
    MSGT_MAILBOX = 1,
};
//protobuf adaptor
struct MailBox : public awolmsg::MsgPortal {
	MailBox(const MsgActor & actor) :MsgPortal(actor, MSGT_MAILBOX){
	}
	const MsgOptions & options() const {
		static MsgOptions mo;
		return mo;
	}
    int sendto(const MsgActor & toactor, const MailMsg & m){
        char buffer[255];
        if (!m.SerializeToArray(buffer, sizeof(buffer))){
            GLOG_ERR("serilaize error !");
            return -1;
        }
        return MsgPortal::sendto(toactor, string(buffer, m.ByteSize()), true);
    }
    int put(const MailMsg & m){
        char buffer[255];
        if (!m.SerializeToArray(buffer, sizeof(buffer))){
            GLOG_ERR("serilaize error !");
            return -1;
        }
        return MsgPortal::put(string(buffer, m.ByteSize()));
    }
	int update(uint64_t id, int op){
        auto it = mail_list.find(id);
        if (it == mail_list.end()){
            GLOG_ERR("mail :%lu is not exist !", id);
            return -1;
        }
        MailMsg & mm = it->second;
        if (op == 0){ //fetch
            //actor.award(msg);
            mm.set_content("update op fetch");
        }
        else { //read
            mm.set_content("update op read");
        }
        char buffer[255];
        if (!mm.SerializeToArray(buffer, sizeof(buffer))){
            GLOG_ERR("serilaize error !");
            return -1;
        }
        return MsgPortal::update(id, string(buffer, mm.ByteSize()), op);
    }
    enum {
        UPDATE_READ=1,
        UPDATE_FETCH=2,
    };
    void onlist(int ret, bool fromc, const MsgList & vms){
        mail_list.clear();
        GLOG_IFO("list refresh .... ");
        for (int i = 0; i < vms.size(); ++i){
            auto & msg = vms.at(i);
            MailMsg mm;
            mm.ParseFromArray(msg.data().data(), msg.data().length());
            mail_list[msg.id()]=mm;
            GLOG_IFO("insert mail:%s", mm.DebugString().c_str());
        }
    }
    void onremove(int ret, uint64_t id, bool fromc /* = true */){
        if (!ret){
            mail_list.erase(id);
        }
    }
    void onupdate(int ret, uint64_t id, int op){
        GLOG_IFO("update mail op:%d", op);
    }
    void onnotify(uint64_t id, const std::string & msg){
        //send to client
        MailMsg mm;
        mm.ParseFromArray(msg.data(), msg.length());
        mail_list[id] = mm;
    }
    std::unordered_map<uint64_t, MailMsg>   mail_list;
};


int main(){

    MsgSvr & msgsvr = MsgSvr::instance();
    msgsvr.init("127.0.0.1:8888");
    ///////////////////////////////////////////////////////////
    MsgActor ma(1, 2);
    MailBox mb(ma);

    while (!msgsvr.ready()){
        msgsvr.update();
    }


    int ret = mb.list();
    GLOG_IFO("mailbox list :%d", ret);

    MailMsg mm;
    mm.set_sender("sfffffffff");
    mm.set_from(1235421425);
    mm.set_content("vsfsgsgsafag");
    mm.set_subject("sfsfsgsf");
    //1. send mail to 3 from 2
    mb.sendto(MsgActor(1, 3), mm);
    //generate id
    //send a msg to msgsvr
    //recv send result
    //mb recv a msg (notify)


    //2.send mail to 2 from system
    //mb.put(mm);
    //3.client set mail read
    //mb.update(1, MailBox::UPDATE_READ);
    //mb.update(1, MailBox::UPDATE_FETCH);
    //(award)
    //set update data
    //send response to client

    //4.delete
    //mb.remove(1);
    //delele from msgsvr
    //send to client

    //send a request to msgsvr
    //recv from msgsvr response , refresh msg

    while (true){
        msgsvr.update();
    }

    msgsvr.destory();
	return 0;
}

#endif

#include "../app/mailbox.h"
#include "../app/awolapp.pb.h"
using namespace awolapp;

struct TestMailBox : public awolapp::MailBox {
    TestMailBox(const MsgActor & ma) : MailBox(ma){}
    virtual void onread(uint64_t id, const Mail & mail){

    }
    virtual void onfetch(uint64_t id, const Mail & mail){

    }
	virtual void response(const string & resp){
		//test client
		CSAwolMsg msg;
		msg.ParseFromString(resp);
		if (msg.cmd() == CSAwolMsg_MsgCMD_MSG_CMD_LIST){
			//rm6284694626259435521
			CSAwolMsg req;
			req.set_type(MSG_TYPE_MAIL);
			req.mutable_request()->set_id(6284599651647619073);

			req.set_cmd(CSAwolMsg_MsgCMD_MSG_CMD_UPDATE);
			req.mutable_request()->set_opcode(MailOpCode::MAIL_OP_READ);
			request(req.SerializeAsString());

			req.mutable_request()->set_opcode(MailOpCode::MAIL_OP_FETCH);
			request(req.SerializeAsString());

			req.mutable_request()->set_opcode(MailOpCode::MAIL_OP_READ);
			request(req.SerializeAsString());

			req.set_cmd(CSAwolMsg_MsgCMD_MSG_CMD_REMOVE);
			request(req.SerializeAsString());

			req.set_cmd(CSAwolMsg_MsgCMD_MSG_CMD_REMOVE);
			request(req.SerializeAsString());
		}

	}

};

int main(int argc, char * argv[]){
    MsgSvr & msgsvr = MsgSvr::instance();
    msgsvr.init("127.0.0.1:8888");
    //while (!msgsvr.ready()){
    //    msgsvr.update();
    //}

    MsgActor ma1(ActorType::ACTOR_TYPE_PLAYER, 1); //P1
    MsgActor ma2(ActorType::ACTOR_TYPE_PLAYER, 2); //P2
    MsgActor ma(ActorType::ACTOR_TYPE_SERVER, 1); //S1

    Mail mail;
    mail.set_sender("from ma1");
    mail.set_subject("put self");
    mail.mutable_content()->set_type((MailContentType)1);
    mail.mutable_content()->set_data("test content");

    //send to ma2
    TestMailBox mb1(ma1);
	//mb.insert(mail);
    //mb.insert(mail);
    //send to ma2

    //mb.insert(mail);;

    TestMailBox mb2(ma2);
	//mb2.list();
    mail.set_subject("send to ma2");
    //mb2.insert(mail);;
	//TestMailBox::sendto(ma2, mail);

    CSAwolMsg request;
	request.set_type(MSG_TYPE_MAIL);
    request.set_cmd(CSAwolMsg_MsgCMD_MSG_CMD_LIST);
	//mb.request(request.SerializeAsString());
    mb2.request(request.SerializeAsString());


	TestMailBox mb(ma);
	mb.insert(mail);

	//checking



    while (true){
        msgsvr.update();
    }



    return 0;
}





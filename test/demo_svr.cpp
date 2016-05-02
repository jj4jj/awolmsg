#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "../awolmsg/awol_msgportal.h"
#include "../awolmsg/awol_msgsvr.h"
#include "test.pb.h"

using namespace awolmsg;
using namespace std;
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
	int update(uint64_t id, int op){
        if (op == 0){ //fetch
            //actor.award(msg);
        }
        else { //read
        }
        //return error msg
        //update to mailserver
        //response to client
        return 0;
    }
    enum {
        UPDATE_READ=1,
        UPDATE_FETCH=2,
    };
};


int main(){

	MsgSvr	msgsvr;
	msgsvr.init("127.0.0.1:8888");
	///////////////////////////////////////////////////////////
    MsgActor ma(1, 2);
    MailBox mb(ma);

    std::string msg = "test msg";
    //1. send mail to 3 from 2
	mb.sendto(MsgActor(1, 3), msg);
        //generate id
        //send a msg to msgsvr
        //recv send result
        //mb recv a msg (notify)


    //2.send mail to 2 from system
	mb.put(msg);
    //3.client set mail read
    mb.update(1, MailBox::UPDATE_READ);
    mb.update(1, MailBox::UPDATE_FETCH);
        //(award)
        //set update data
        //send response to client

    //4.delete
    mb.remove(1);
        //delele from msgsvr
        //send to client

    mb.list();
        //send a request to msgsvr
        //recv from msgsvr response , refresh msg

	msgsvr.destory();

	return 0;
}
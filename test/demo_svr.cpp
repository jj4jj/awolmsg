#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "../awolmsg/Messenger.h"
#include "../proto/awolmsg.pb.h"
#include "../awolmsg/MsgPortal.h"
#include "test.pb.h"

using namespace awolmsg;

enum  AwolMsgType {
    MSGT_MAILBOX = 1,
};

//protobuf adaptor
struct MailBox : public MsgPortal<MailMsg> {
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

    Messenger<MailBox>.init(MSGT_MAILBOX, MsgOptions());
        

    MsgActor ma;
    ma.type = 1;
    ma.id = 2;
    MailBox mb(ma);

    MailMsg m;
    //1. send mail to 3 from 2
    mb.send(MsgActor(1, 3), m);
        //generate id
        //send a msg to msgsvr
        //recv send result
        //mb recv a msg (notify)


    //2.send mail to 3 from system
    MailBox::send(MsgActor(1, 3), m);

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


    return 0;
}
#pragma  once
#include "dcpots/base/noncopyable.h"

namespace awolmsg {
    struct MsgActor;
};
namespace awolapp {
    struct ChatBoxImpl;
    class ChatMsg;
    struct ChatBox : noncopyable {
        int pull();
        int push(const ChatMsg & msg);
        int send(const awolmsg::MsgActor & actorto, const ChatMsg & msg);
        virtual void onpull();
        virtual void onrecv(const ChatMsg & msg);
    public:
        ChatBox(const awolmsg::MsgActor & actor);
        virtual ~ChatBox();
    private:
        ChatBoxImpl * impl{ nullptr };
    };

}
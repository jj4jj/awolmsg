#pragma  once
#include "dcpots/base/noncopyable.h"

namespace awolmsg {
    struct MsgActor;
};
namespace awolapp {
    struct NotifierImpl;
    class Notice;
    struct Notifier : noncopyable {
        int pull();
        int push(const Notice & msg);
        virtual void onpull();
        virtual void onrecv(const Notice & msg);
    public:
        Notifier(const awolmsg::MsgActor & actor);
        virtual ~Notifier();
    private:
        NotifierImpl * impl{ nullptr };
    };

}
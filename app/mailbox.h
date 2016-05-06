#pragma  once
#include "dcpots/base/noncopyable.h"

namespace awolmsg {
    struct MsgActor;
};
namespace awolapp {
    struct MailBoxImpl;
    class Mail;
    struct MailBox : noncopyable {
        int list();
        int remove(uint64_t id);
        int insert(const Mail & mail);
        virtual void onremove(uint64_t id, bool fromc);
        virtual void onread(uint64_t id, const Mail & mail);
        virtual void onfetch(uint64_t id, const Mail & mail);
        virtual void onnotify(uint64_t id, const Mail & mail);
        virtual void onlist(bool fromc);
    public: //for client handle request
        int request(const string & req);
        virtual void response(const string & resp); //send to client actor
    public:
        MailBox(const awolmsg::MsgActor & actor);
        virtual ~MailBox();
    private:
        MailBoxImpl * impl{ nullptr };
    };

}
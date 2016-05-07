#pragma  once
#include "dcpots/base/noncopyable.h"

namespace awolmsg {
    struct MsgActor;
    class AwolMsg;
};
namespace awolapp {
    struct MailBoxImpl;
    class Mail;
    struct MailBox : noncopyable {
        int list();
        int remove(uint64_t id);
        int insert(const awolapp::Mail & mail);
        virtual void onread(uint64_t id, const awolapp::Mail & mail);
        virtual void onfetch(uint64_t id, const awolapp::Mail & mail);

    public:
        int request(const string & req);
        virtual void response(const string & resp); //send to client actor

    public:
        int   onop_do(uint64_t id, Mail & mail, int op);
        void  onop_did(uint64_t id, const Mail & mail, int op);
        void  set_msg(AwolMsg & msg, const Mail & mail);
        const Mail & get_msg(const AwolMsg & msg);
        void  onnotify(uint64_t id, const Mail & mail);
    public:
        MailBox(const awolmsg::MsgActor & actor);
        virtual ~MailBox();
    private:
        MailBoxImpl * impl{ nullptr };
    };

}
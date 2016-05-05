#pragma  once
#include "dcpots/base/dcutils.hpp"
namespace awolmsg {
    struct MsgActor;
};
namespace awolapp {
    struct MailBoxImpl;
    class Mail;
    struct MailBox : dcsutil::noncopyable {
        int list(bool fromc);
        int remove(uint64_t id);
        int read(uint64_t id);
        int fetch(uint64_t id);
        int insert(const Mail & mail);
        virtual void onremove(uint64_t id, bool fromc);
        virtual void onread(uint64_t id, const Mail & mail);
        virtual void onfetch(uint64_t id, const Mail & mail);
        virtual void onnotify(uint64_t id, const Mail & mail);
    public:
        MailBox(const awolmsg::MsgActor & actor);
        virtual ~MailBox();
    private:
        MailBoxImpl * impl{ nullptr };
    };

}
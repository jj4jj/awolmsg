#pragma  once
#include "dcpots/base/noncopyable.h"

namespace awolmsg {
    struct MsgActor;
};
namespace awolapp {
    class AwolMsg;
    struct MailBoxImpl;
    class Mail;
    struct MailBox : public noncopyable {
        int list();
        int remove(uint64_t id);
        int insert(const awolapp::Mail & mail);
        virtual void onread(uint64_t id, const awolapp::Mail & mail);
        virtual void onfetch(uint64_t id, const awolapp::Mail & mail);
        virtual int  checksend(const awolmsg::MsgActor & sendto, const awolapp::Mail & mail);
    public:
        int request(const string & req);
        virtual void response(const string & resp); //send to client actor

	public:
        int   checkop(uint64_t id, Mail & mail, int op);
        void  onmsgop(uint64_t id, const Mail & mail, int op);
        void  onnotify(uint64_t id, const Mail & mail);

	public:
		static int sendto(const awolmsg::MsgActor & actor, const awolapp::Mail & mail);
		virtual void  set_msg(AwolMsg & msg, const Mail & mail);
		virtual const Mail & get_msg(const AwolMsg & msg);
        const Mail * find_msg(uint64_t msgid);
        const Mail * find_refer_mail(uint64_t msgid);
        int          msg_list_size();
        const Mail * get_mail_by_idx(int idx);
        uint64_t     get_mail_id_by_idx(int idx);
        const awolmsg::MsgActor & actor();
        MailBox(const awolmsg::MsgActor & actor);
        virtual ~MailBox();
    private:
        MailBoxImpl * impl{ nullptr };
    };

}

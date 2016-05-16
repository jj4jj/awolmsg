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
        int   checkop(uint64_t id, Mail & mail, int op);
        void  onmsgop(uint64_t id, const Mail & mail, int op);
        void  onnotify(uint64_t id, const Mail & mail);

	public:
		static int sendto(const MsgActor & actor, const awolapp::Mail & mail);
		void  set_msg(AwolMsg & msg, const Mail & mail);
		const Mail & get_msg(const AwolMsg & msg);
	public:
        MailBox(const awolmsg::MsgActor & actor);
        virtual ~MailBox();
    private:
        MailBoxImpl * impl{ nullptr };
    };

}
#pragma once
#include "awol_msgbox.h"
namespace awolmsg {
    template<int MsgTypeV, class MsgT>
    struct MsgPortal {
        virtual int sendto(const MsgActor & actor, const MsgT & m, bool fromc = true);
		virtual int put(const MsgT & m);//server
		virtual int update(const MsgT & m);
        virtual int list(bool fromc = true);
        virtual int remove(uint64_t id, bool fromc = true);//client or server
        virtual int update(uint64_t id, int op);//client
    public:
        static int send(const MsgActor & actor, const MsgT & m);//server
        MsgPortal(const MsgActor & actor);
        virtual ~MsgPortal();
		//MsgBox<MsgT> & msgbox();
    private:
        //MsgBox<MsgT>    msgbox_;
    };
}

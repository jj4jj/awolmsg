#pragma once
#include "MsgBox.h"
namespace awolmsg {
    template<class MsgT>
    struct MsgPortal {
        virtual int send(const MsgActor & actor, const MsgT & m, bool fromc = true);
        virtual int update(const MsgT & m);
        virtual int list(bool fromc = true);
        virtual int remove(uint64_t id, bool fromc = true);
        virtual int update(uint64_t id, int op);
    public:
        static int send(const MsgActor & actor, const MsgT & m);
        MsgPortal(const MsgActor & actor);
        virtual ~MsgPortal();
        MsgBox<MsgT> & msgbox();
    private:
        MsgBox<MsgT>    msgbox_;
    };
}

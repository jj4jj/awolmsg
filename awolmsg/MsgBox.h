#pragma  once

namespace awolmsg {
    template<class ValueT>
    struct MsgFuture  {
        typedef std::function<void(ValueT &)> ResultNotify;
        typedef std::function<void(int)>    ErrorNotify;
        MsgFuture & result(ResultNotify notify);
        MsgFuture & error(ErrorNotify notify);
    };
    template<class MsgActorT, class ValueT>
    struct MsgFuture  {
        typedef std::function<void(MsgActorT & sender, ValueT &)> ResultNotify;
        typedef std::function<void(int)>    ErrorNotify;
        MsgFuture & result(ResultNotify notify);
        MsgFuture & error(ErrorNotify notify);
    };

    template<class MsgT>
    struct MsgBox {
        MsgBox(const MsgActor & act, int type);
    public:
        MsgFuture<std::vector<MsgT>> list();
        MsgFuture<int> send(MsgBox & mb, const MsgT & m); //send msg to
        MsgFuture<int> remove(const MsgT & m); //remove one msg
        MsgFuture<int> update(const MsgT & m);
        MsgFuture<int> clean();
        MsgFuture<MsgActor, MsgT> recv(); //notify a new message
    };






}


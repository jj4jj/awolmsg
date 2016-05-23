#pragma once
namespace awolmsg {
    class MsgActor;
}
namespace awolapp {
    class    CSAwolMsg;
    int dispatch(const awolmsg::MsgActor & actor, int type, const std::string & request);
}

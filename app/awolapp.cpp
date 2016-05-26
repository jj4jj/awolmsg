#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "../core/awol_msgsvr.h"
#include "../core/awol_msgportal.h"
#include "../proto/awolapp.pb.h"
#include "awolapp.h"

using namespace awolmsg;
namespace awolapp {
    int dispatch(const awolmsg::MsgActor & actor, int type, const std::string & request){
        MsgPortal * portal = MsgSvr::instance().find(actor, type);
        if (!portal){
            GLOG_ERR("not found actor:%s type:%d for request [%s]",
                actor.ShortDebugString().c_str(), type, request.c_str());
            return -1;
        }
        return portal->request(request);
    }
}

#pragma once

namespace awolmsg {
enum ErrorCode {
	AWOL_EC_OK = 0,
	AWOL_EC_NO_PERM = 1,
    AWOL_EC_ERROR_PACK = 2,
    AWOL_EC_ERROR_UNPACK = 3,
    AWOL_EC_MSG_NOT_FOUND = 4,
    AWOL_EC_MSG_EXIST_ALREADY = 5,
    AWOL_EC_MSG_UPDATE_STATE_ERROR = 6,
};


}
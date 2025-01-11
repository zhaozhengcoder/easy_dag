#pragma once

namespace easy_dag {

enum ErrorCode {
    SUCCESS = 0,
    FAIL = -1,
    TIMEOUT = -2,
    INVALID_ARG = -3,
    UNDEFINED = -4,
    UNKNOWN_MODULE = -5,
    INVALID_JSON = -6,
    INVALID_JSON_DATA = -7,
};
}

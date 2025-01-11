#pragma once
#include <iostream>
#include <cstdlib> 
#include <string>
#include "src/utils/log.h"

namespace easy_dag {

// 定义一个宏来控制终端输出为红色
#define RED_TERMINAL "\033[31m"
// 定义一个宏来恢复终端默认颜色
#define RESET_TERMINAL "\033[0m"

#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected)!= (actual)) { \
            std::string msg = "Expected: " + std::to_string(expected) + ", but actual: " + std::to_string(actual); \
            std::string colored_msg = RED_TERMINAL + msg + RESET_TERMINAL; \
            LOG(INFO) << colored_msg; \
        } \
    } while (0)

#define EXPECT_EQ_STR(expected, actual) \
    do { \
        if ((expected)!= (actual)) { \
            std::string msg = "Expected: \"" + (expected) + "\", but actual: \"" + (actual) + "\""; \
            std::string colored_msg = RED_TERMINAL + msg + RESET_TERMINAL; \
            LOG(INFO) << colored_msg; \
        } \
    } while (0)

}
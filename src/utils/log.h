#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include "src/utils/assert_test.h"

namespace easy_dag {
// 定义日志级别
enum LogLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// 日志类
class MyLog {
public:
    MyLog(LogLevel level, const std::string& file, int line)
        : level_(level), file_(file), line_(line) {}

    ~MyLog() {
        std::ostringstream stream;
        stream << "[" << getCurrentTime() << "] "
               << "[" << levelToString(level_) << "] "
               << "[" << file_ << ":" << line_ << "] "
               << message_.str();
        if (level_ == ERROR || level_ == FATAL) {
            std::string colored_msg = RED_TERMINAL + stream.str() + RESET_TERMINAL;
            std::cout << colored_msg << std::endl;
        } else {
            std::cout << stream.str() << std::endl;
        }
        if (level_ == FATAL) {
            abort();
        }
    }

    std::ostringstream& stream() {
        return message_;
    }

    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

private:
    std::string levelToString(LogLevel level) {
        switch (level) {
            case INFO: return "INFO";
            case WARNING: return "WARNING";
            case ERROR: return "ERROR";
            case FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    LogLevel level_;
    std::string file_;
    int line_;
    std::ostringstream message_;
};

// 定义日志宏
#define LOG(level) MyLog(level, __FILE__, __LINE__).stream()

}
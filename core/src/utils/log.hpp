#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace aardvark {

class Log {
  public:
    static std::shared_ptr<spdlog::logger> create_logger() {
        auto logger = spdlog::stdout_color_mt("a");
        logger->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
        return logger;
    }

    static spdlog::logger* get_logger() {
        static auto logger = create_logger();
        return logger.get();
    }

    template <class... ArgsTypes>
    static void debug(ArgsTypes... args) {
        get_logger()->debug(args...);
    }

    template <class... ArgsTypes>
    static void info(ArgsTypes... args) {
        get_logger()->info(args...);
    }

    template <class... ArgsTypes>
    static void error(ArgsTypes... args) {
        get_logger()->error(args...);
    }
};

} // namespace aardvark

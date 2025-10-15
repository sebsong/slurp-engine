#pragma once
#include <iostream>

namespace logging {
    template<typename T>
    static void log(const char* level, T message) {
        // TODO: replace with real logging to a file
        std::cout << level << ": " << message << std::endl;
    }

    template<typename T>
    void debug(const T& message) {
#if DEBUG
        log("DEBUG", message);
#endif
    }

    template<typename T>
    void info(const T& message) {
        log("INFO", message);
    }

    template<typename T>
    void warn(const T& message) {
        log("WARN", message);
    }

    template<typename T>
    void error(const T& message) {
        log("ERROR", message);
    }
}

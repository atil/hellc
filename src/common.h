#pragma once
//#define NDEBUG // TODO @TASK @DEV: Use build configurations

#include <string>

#ifdef _DEBUG
#include <iostream>
#include <cstdarg>
#include <vector>
    #ifdef _WIN32 // Nobody's gonna run this on linux, but you never know
    #define NOMINMAX // To undef min and max macros in windows header. Those collide with std::min and std::max
    #include <windows.h>
    #include <debugapi.h>
    #endif
#endif

// TODO @TASK @DEV: This should be enabled only when the HELL_TEST flag is enabled
// That's a thing we'll enable when we actually debug stuff, not used in the implementation
// The reason is because it makes the compilation a bit longer

struct Debug {
    // Taken from: https://stackoverflow.com/a/49812356/4894526
    void static log(const std::string s_format, ...) { // NOLINT(performance-unnecessary-value-param): Paramter used in va macro
#ifdef _DEBUG
        const char * const zc_format = s_format.c_str();

        va_list va_args;
        va_start(va_args, s_format);

        va_list va_copy;
        va_copy(va_copy, va_args);
        const size_t i_len = std::vsnprintf(nullptr, 0, zc_format, va_copy);
        va_end(va_copy);

        std::vector<char> zc(1 + i_len);
        std::vsnprintf(zc.data(), zc.size(), zc_format, va_args);
        va_end(va_args);
        std::cout << std::string(zc.data(), zc.size()) << std::endl;
#endif
    }

    void static break_on_fail(bool condition, const std::string& message) {
#if _DEBUG && _WIN32
        if (!condition) {
            DebugBreak();
        }
#endif
    }
};

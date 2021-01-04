#pragma once
#include <string>

#ifdef _DEBUG
#include <iostream>
#include <cstdarg>
#include <cassert>
#include <vector>
#endif

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

    void static hell_assert(bool condition) {
#ifdef _DEBUG
        if (!condition) {
            assert(condition);
        }
#endif
    }

};

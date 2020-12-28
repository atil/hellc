#pragma once
#include <cstdlib>

inline float random_float(float low, float high) {
    return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (high - low)));
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

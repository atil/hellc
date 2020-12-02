#pragma once

#include "world.h"

inline int sign(const float f) {
    return (f > 0.0f) - (f < 0.0f);
}

inline float min(const float f1, const float f2) {
    return f1 < f2 ? f1 : f2;
}

inline bool in_between(const float arg, const float f_small, const float f_big) {
    return arg >= f_small && arg <= f_big;
}

inline bool approx(const float arg1, const float arg2) {
    return abs(arg1 - arg2) < 0.001;
}

inline bool approx_vec(const glm::vec3& v1, const glm::vec3& v2) {
    return approx(v1.x, v2.x) && approx(v1.y, v2.y) && approx(v1.z, v2.z);
}

float get_line_segment_plane_distance(const glm::vec3& a, const glm::vec3& b, const Triangle& triangle, glm::vec3& closer_point);
glm::vec3 project_point_on_triangle_plane(const glm::vec3& point, const Triangle& triangle, float& distance);
bool is_point_in_triangle(const glm::vec3& point, const Triangle& triangle);
glm::vec3 get_closest_point_on_triangle(const glm::vec3& point, const Triangle& triangle);

void run_geom_tests_internal();

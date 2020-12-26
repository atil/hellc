#pragma once
#include "world.h"

inline bool in_between(const float arg, const float f_small, const float f_big) {
    return arg >= f_small && arg <= f_big;
}

inline bool approx(const float arg1, const float arg2) {
    return std::abs(arg1 - arg2) < 0.001f;
}

inline bool approx_vec(const Vector3& v1, const Vector3& v2) {
    return approx(v1.x, v2.x) && approx(v1.y, v2.y) && approx(v1.z, v2.z);
}

Vector3 project_vector_on_plane(const Vector3& v, const Vector3& n);
float get_line_segment_plane_distance(const Vector3& a, const Vector3& b, const Triangle& triangle, Vector3& closer_point);
Vector3 project_point_on_triangle_plane(const Vector3& point, const Triangle& triangle, float& distance);
bool is_point_in_triangle(const Vector3& point, const Triangle& triangle);
Vector3 get_closest_point_on_triangle(const Vector3& point, const Triangle& triangle);

void run_geom_tests_internal();

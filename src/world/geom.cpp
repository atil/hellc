#include "geom.h"
#include <iostream>
#include <cstdlib>

float get_line_segment_plane_distance(const Vector3& a, const Vector3& b, const Triangle& triangle,
                                      Vector3& closer_point) {

    const float dist_a = abs(Vector3::dot(a - triangle.p0, triangle.normal));
    const float dist_b = abs(Vector3::dot(b - triangle.p0, triangle.normal));

    const float side_a = Vector3::dot(a - triangle.p0, triangle.normal);
    const float side_b = Vector3::dot(b - triangle.p0, triangle.normal);

    if (side_a * side_b < 0) {
        // Directions face towards different sides
        // i.e. line segment is crossing the plane
        const float t = dist_a / (dist_a + dist_b);
        closer_point = a + Vector3::normalize(b - a) * t;
        return 0;
    }

    if (approx(dist_a, dist_b)) {
        closer_point = (a + b) * 0.5f;
        return dist_a;
    }

    closer_point = dist_a < dist_b ? a : b;
    return min(dist_a, dist_b);
}

Vector3 project_point_on_triangle_plane(const Vector3& point, const Triangle& triangle, float& distance) {
    const float dot = Vector3::dot(point - triangle.p0, triangle.normal);
    distance = abs(dot);
    const float side = dot > 0.0f ? -1.0f : 1.0f;
    return point + triangle.normal * distance * side;
}

bool is_point_in_triangle(const Vector3& point, const Triangle& triangle) {
    if (abs(Vector3::dot(point - triangle.p0, triangle.normal)) > 0.0001f) {
        std::cout << "Attempted to perform point-triangle check on non-coplanar point-triangle : " << std::endl <<
            "point: " << point.to_string() << std::endl <<
            "triangle p0: " << triangle.p0.to_string() << std::endl <<
            "triangle normal" << triangle.normal.to_string() << std::endl;
        return false;
    }

    // Barycentric coordinates
    const float a = Vector3::length(Vector3::cross(triangle.p1 - point, triangle.p2 - point)) / (2.0f * triangle.area);
    const float b = Vector3::length(Vector3::cross(triangle.p2 - point, triangle.p0 - point)) / (2.0f * triangle.area);
    const float c = Vector3::length(Vector3::cross(triangle.p0 - point, triangle.p1 - point)) / (2.0f * triangle.area);

    return in_between(a, 0.0f, 1.0f) && in_between(b, 0.0f, 1.0f) && in_between(c, 0.0f, 1.0f)
        && approx(a + b + c, 1.0f);
}

Vector3 get_closest_point_on_line_segment(const Vector3& point, const Vector3& a, const Vector3& b, float& distance) {
    const Vector3 line_segment_dir = Vector3::normalize(b - a);
    const float line_segment_length = Vector3::length(b - a);

    const float dot = Vector3::dot(point - a, line_segment_dir);
    if (in_between(dot, 0, line_segment_length)) {
        distance = dot;
        return a + line_segment_dir * dot;
    }

    const float dist1 = Vector3::distance(point, a);
    const float dist2 = Vector3::distance(point, b);

    if (dist1 < dist2) {
        distance = dist1;
        return a;
    } 

    distance = dist2;
    return b;
}

Vector3 get_closest_point_on_triangle(const Vector3& point, const Triangle& triangle) {
    float d0, d1, d2;
    const Vector3 p0 = get_closest_point_on_line_segment(point, triangle.p0, triangle.p1, d0);
    const Vector3 p1 = get_closest_point_on_line_segment(point, triangle.p1, triangle.p2, d1);
    const Vector3 p2 = get_closest_point_on_line_segment(point, triangle.p2, triangle.p0, d2);

    if (d0 < d1 && d0 < d2) {
        return p0;
    }
    if (d1 < d2) {
        return p1;
    }
    return p2;
}

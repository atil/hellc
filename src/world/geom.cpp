#include "geom.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cstdlib>

float get_line_segment_plane_distance(const glm::vec3& a, const glm::vec3& b, const Triangle& triangle,
                                      glm::vec3& closer_point) {

    const float dist_a = abs(glm::dot(a - triangle.p0, triangle.normal));
    const float dist_b = abs(glm::dot(b - triangle.p0, triangle.normal));

    const float side_a = glm::dot(a - triangle.p0, triangle.normal);
    const float side_b = glm::dot(b - triangle.p0, triangle.normal);

    if (side_a * side_b < 0) {
        // Directions face towards different sides
        // i.e. line segment is crossing the plane
        const float t = dist_a / (dist_a + dist_b);
        closer_point = a + glm::normalize(b - a) * t;
        return 0;
    }

    if (approx(dist_a, dist_b)) {
        closer_point = (a + b) * 0.5f;
        return dist_a;
    }

    closer_point = dist_a < dist_b ? a : b;
    return min(dist_a, dist_b);
}

glm::vec3 project_point_on_triangle_plane(const glm::vec3& point, const Triangle& triangle, float& distance) {
    const float dot = glm::dot(point - triangle.p0, triangle.normal);
    distance = abs(dot);
    const float side = dot > 0.0f ? -1.0f : 1.0f;
    return point + triangle.normal * distance * side;
}

bool is_point_in_triangle(const glm::vec3& point, const Triangle& triangle) {
    if (abs(glm::dot(point - triangle.p0, triangle.normal)) > 0.0001f) {
        std::cout << "Attempted to perform point-triangle check on non-coplanar point-triangle : " << std::endl <<
            "point: " << glm::to_string(point) << std::endl <<
            "triangle p0: " << glm::to_string(triangle.p0) << std::endl <<
            "triangle normal" << glm::to_string(triangle.normal) << std::endl;
        return false;
    }

    // Barycentric coordinates
    const float a = glm::length(glm::cross(triangle.p1 - point, triangle.p2 - point)) / (2.0f * triangle.area);
    const float b = glm::length(glm::cross(triangle.p2 - point, triangle.p0 - point)) / (2.0f * triangle.area);
    const float c = glm::length(glm::cross(triangle.p0 - point, triangle.p1 - point)) / (2.0f * triangle.area);

    return in_between(a, 0.0f, 1.0f) && in_between(b, 0.0f, 1.0f) && in_between(c, 0.0f, 1.0f)
        && approx(a + b + c, 1.0f);
}

glm::vec3 get_closest_point_on_line_segment(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, float& distance) {
    const glm::vec3 line_segment_dir = glm::normalize(b - a);
    const float line_segment_length = glm::length(b - a);

    const float dot = glm::dot(point - a, line_segment_dir);
    if (in_between(dot, 0, line_segment_length)) {
        distance = dot;
        return a + dot * line_segment_dir;
    }

    const float dist1 = glm::distance(point, a);
    const float dist2 = glm::distance(point, b);

    if (dist1 < dist2) {
        distance = dist1;
        return a;
    } 

    distance = dist2;
    return b;
}

glm::vec3 get_closest_point_on_triangle(const glm::vec3& point, const Triangle& triangle) {
    float d0, d1, d2;
    const glm::vec3 p0 = get_closest_point_on_line_segment(point, triangle.p0, triangle.p1, d0);
    const glm::vec3 p1 = get_closest_point_on_line_segment(point, triangle.p1, triangle.p2, d1);
    const glm::vec3 p2 = get_closest_point_on_line_segment(point, triangle.p2, triangle.p0, d2);

    if (d0 < d1 && d0 < d2) {
        return p0;
    }
    if (d1 < d2) {
        return p1;
    }
    return p2;
}

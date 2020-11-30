#include <iostream>
#include "world.h"
#include "geom.h"
#include <cassert>

void Physics::register_obj(const ObjModelData& obj_data) {
    this->static_colliders.emplace_back(obj_data);
}

bool Physics::resolve_penetration(const PlayerShape& player_shape, const Triangle& triangle, glm::vec3& penetration) {
    glm::vec3 closer_segment_point; // Could be on the segment itself
    const float dist_to_plane = get_line_segment_plane_distance(player_shape.segment_up, player_shape.segment_bottom, triangle, closer_segment_point);
    if (dist_to_plane > player_shape.radius) {
        return false; // Far from the segment
    }

    float closer_segment_point_distance, upper_tip_distance, lower_tip_distance;
    const glm::vec3 closer_point_projection = project_point_on_triangle_plane(closer_segment_point, triangle, closer_segment_point_distance);
    const glm::vec3 upper_tip_projection = project_point_on_triangle_plane(player_shape.tip_up, triangle, upper_tip_distance);
    const glm::vec3 lower_tip_projection = project_point_on_triangle_plane(player_shape.tip_bottom, triangle, lower_tip_distance);

    if (is_point_in_triangle(closer_point_projection, triangle)
        || is_point_in_triangle(upper_tip_projection, triangle)
        || is_point_in_triangle(lower_tip_projection, triangle)) {
        // The line segment (with its tips) can be projected to the triangle
        // Penetration can be directly calculated via (projection) to (closer_tip)
        glm::vec3 closer_tip_point;
        if (abs(upper_tip_distance - lower_tip_distance) < 0.00001f) {
            // Equidistance from the both tips indicate a vertical triangle case
            // The tip here is the point on the capsule surface
            closer_tip_point = player_shape.mid_point - triangle.normal * player_shape.radius;
        } else if (upper_tip_distance < lower_tip_distance) {
            closer_tip_point = player_shape.tip_up;
        } else if (upper_tip_distance > lower_tip_distance) {
            closer_tip_point = player_shape.tip_bottom;
        }
        
        penetration = glm::distance(closer_tip_point, closer_point_projection) * triangle.normal;
        return true;
    }

    // TODO @BUG: This is wrong
    // It's kind of hard to explain in words. Draw this case on paper. The solution should include some trigonometry
    const glm::vec3 closest_point_on_triangle = get_closest_point_on_triangle(closer_segment_point, triangle);
    const glm::vec3 penet_dir = glm::normalize(closer_point_projection - closest_point_on_triangle);
    const float penet_amount = player_shape.radius - glm::length(closer_point_projection - closest_point_on_triangle);
    penetration = penet_dir * penet_amount;

    return true;
}

// TODO @TASK: Use a fixed deltaTime here. Physics should be frame independent
void Physics::tick(glm::vec3& player_pos, float dt) const {
    constexpr float player_height = 1.0f;
    constexpr float player_radius = 0.5f;
    PlayerShape player_shape(player_pos, player_height, player_radius);

    for (const StaticCollider& static_collider : this->static_colliders) {
        for (const Triangle& triangle : static_collider.get_triangles()) {
            glm::vec3 penet;
            if (resolve_penetration(player_shape, triangle, penet)) {
                player_pos += penet;
                player_shape = PlayerShape(player_pos, player_height, player_radius);
            }
        }
    }
}

void Physics::run_geom_tests() {
    run_geom_tests_internal();
}


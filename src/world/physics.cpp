#include <iostream>
#include "world.h"
#include "geom.h"

constexpr float player_height = 1.0f;
constexpr float player_radius = 0.5f;

StaticCollider::StaticCollider(const ObjModelData& obj_data, const Vector3& position) {
    const std::vector<Vector3>& position_data = obj_data.position_data;

    for (const ObjSubmodelData& obj_submodel_data : obj_data.submodel_data) {
        for (const ObjFaceData& face_data : obj_submodel_data.faces) {
            const size_t* position_indices = face_data.position_indices;
            const Vector3& p0 = position_data[position_indices[0]] + position;
            const Vector3& p1 = position_data[position_indices[1]] + position;
            const Vector3& p2 = position_data[position_indices[2]] + position;

            this->triangles.emplace_back(p0, p1, p2);
        }
    }
}

void World::register_static_collider(const ObjModelData& obj_data, const Vector3& position) {
    this->static_colliders.emplace_back(obj_data, position);
}

bool Physics::resolve_penetration(const PlayerShape& player_shape, const Triangle& triangle, Vector3& penetration) {
    Vector3 closer_segment_point; // Could be on the segment itself
    const float dist_to_plane = get_line_segment_plane_distance(player_shape.segment_up, player_shape.segment_bottom, triangle, closer_segment_point);
    if (dist_to_plane > player_shape.radius) {
        return false; // Far from the plane
    }

    float closer_segment_point_distance, upper_tip_distance, lower_tip_distance;
    const Vector3 closer_point_projection = project_point_on_triangle_plane(closer_segment_point, triangle, closer_segment_point_distance);
    const Vector3 upper_tip_projection = project_point_on_triangle_plane(player_shape.tip_up, triangle, upper_tip_distance);
    const Vector3 lower_tip_projection = project_point_on_triangle_plane(player_shape.tip_bottom, triangle, lower_tip_distance);

    if (is_point_in_triangle(closer_point_projection, triangle)
        || is_point_in_triangle(upper_tip_projection, triangle)
        || is_point_in_triangle(lower_tip_projection, triangle)) {
        // The line segment (with its tips) can be projected to the triangle
        // Penetration can be directly calculated via (projection) to (closer_tip)
        Vector3 closer_tip_point;
        if (abs(upper_tip_distance - lower_tip_distance) < 0.00001f) {
            // Equidistance from the both tips indicate a vertical triangle case
            // The tip here is the point on the capsule surface
            closer_tip_point = player_shape.mid_point - triangle.normal * player_shape.radius;
        } else if (upper_tip_distance < lower_tip_distance) {
            closer_tip_point = player_shape.tip_up;
        } else if (upper_tip_distance > lower_tip_distance) {
            closer_tip_point = player_shape.tip_bottom;
        }
        
        penetration = triangle.normal * Vector3::distance(closer_tip_point, closer_point_projection);
        return true;
    }

    const Vector3 closest_point_on_triangle = get_closest_point_on_triangle(closer_segment_point, triangle);
    const float dist_to_projection = Vector3::length(closer_point_projection - closest_point_on_triangle);
    if (dist_to_projection > player_shape.radius) {
        return false;
    }
    // @CLEANUP: This penet_amount is not 100% precise for vertical triangles
    // The exact solution requires getting the (closest_point_on_segment -> closest_point_on_triangle) vector
    // and multiplying with sin of the angle which this vector makes with vector3.down
    // Draw it on something, then it'll be clearer to understand
    const float penet_amount = player_shape.radius - dist_to_plane;
    penetration = triangle.normal * penet_amount;

    return true;
}

Vector3 Physics::compute_penetrations(const Vector3& player_pos, const std::vector<StaticCollider>& static_colliders) {
    PlayerShape player_shape(player_pos, player_height, player_radius);
    Vector3 total_displacement;
    for (const StaticCollider& static_collider : static_colliders) {
        for (const Triangle& triangle : static_collider.triangles) {
            Vector3 penet;
            if (resolve_penetration(player_shape, triangle, penet)) {
                total_displacement += penet;
                player_shape.displace(penet);
            }
        }
    }

    return total_displacement;
}

bool Physics::is_grounded(const Vector3& player_pos, const Vector3& player_move_dir_horz, const std::vector<StaticCollider>& static_colliders, Vector3& ground_normal) {
    const PlayerShape player_shape(player_pos, player_height, player_radius);
    const Vector3 mid_pos = player_shape.segment_bottom;

    const Vector3 left = Vector3::cross(Vector3::up, player_move_dir_horz);
    std::vector<Ray> grounded_check_rays {
        Ray(mid_pos, Vector3::down),
        Ray(mid_pos + player_move_dir_horz * player_shape.radius, Vector3::down),
        Ray(mid_pos - player_move_dir_horz * player_shape.radius, Vector3::down),
        Ray(mid_pos + left * player_shape.radius, Vector3::down),
        Ray(mid_pos - left * player_shape.radius, Vector3::down),
    };

    const float ray_length = player_shape.radius + 0.1f;
    Ray hit(Vector3::zero, Vector3::zero);
    for (const Ray& ray : grounded_check_rays) {
        if (raycast(ray, ray_length, static_colliders, hit)) {

            ground_normal = hit.direction;
            return true;
        }
    }
    return false;
}

bool Physics::raycast(const Ray& ray, float max_dist, const std::vector<StaticCollider>& static_colliders, Ray& out) {
    for (const StaticCollider& collider : static_colliders) {
        for (const Triangle& triangle : collider.triangles) {

            // Ray-triangle check
            const float dir_dot_normal = Vector3::dot(ray.direction, triangle.normal);
            if (approx(dir_dot_normal, 0)) {
                continue; // Parallel
            }

            const float t = Vector3::dot(triangle.p0 - ray.origin, triangle.normal) / dir_dot_normal;
            if (t < 0.0f) {
                continue; // Behind the ray
            }

            if (t > max_dist) {
                continue; // Triangle-plane is too far away
            }
            
            Vector3 hit_on_plane = ray.at(t);
            if (is_point_in_triangle(hit_on_plane, triangle)) {
                out = Ray(hit_on_plane, triangle.normal);
                return true;
            }
        }
    }

    return false;
}


void Physics::run_geom_tests() {
    run_geom_tests_internal(); // TODO @CLEANUP: This looks weird. Maybe move this to world.h?
}


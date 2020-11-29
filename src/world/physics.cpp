#include <iostream>
#include "world.h"
#include <cassert>

constexpr glm::vec3 unit_y(0, 1, 0);

struct PlayerShape {
    glm::vec3 segment_up{};
    glm::vec3 segment_bottom{};
    glm::vec3 tip_up{};
    glm::vec3 tip_bottom{};
    float radius;

    explicit PlayerShape(const glm::vec3& player_pos, float height, float r) : radius(r) {
        this->segment_up = player_pos + (unit_y * (height / 2));
        this->segment_bottom = player_pos - (unit_y * (height / 2));
        this->tip_up = this->segment_up + unit_y * this->radius;
        this->tip_bottom = this->segment_bottom - unit_y * this->radius;
    }
};

void Physics::register_obj(const ObjModelData& obj_data) {
    this->static_colliders.emplace_back(obj_data);
}

glm::vec3 resolve_penetration(const PlayerShape& player_shape, const Triangle& triangle) {
    // TODO @TASK: Collision resolving here:
    // check shape-triangle distance. early return if too far (also consider vertical triangles)
    // project "closer_point" and two tips onto the triangle plane. if one of them is in the triangle, then there's penetration
    //   penet = (closer_tip_point - point_on_plane).magnitude() * triangle.normal
    // if none of them is in the triangle (which means the capsule "line" doesn't intersect with the tirangle),
    // then check capsule_segment-triangle distance
    //   penet = let v = (point_on_plane - closest_point_on_triangle).normalize()
    //        * (player_shape.radius - distance_to_triangle_on_plane);
    return unit_y;
}

// TODO @TASK: Use a fixed deltaTime here. Physics should be frame independent
void Physics::tick(glm::vec3& player_pos, float dt) const {
    constexpr float player_height = 1.0f;
    constexpr float player_radius = 0.5f;
    PlayerShape player_shape(player_pos, player_height, player_radius);

    for (const StaticCollider& static_collider : this->static_colliders) {
        for (const Triangle& triangle : static_collider.get_triangles()) {
            const glm::vec3 penet = resolve_penetration(player_shape, triangle);
            player_pos += penet;
            player_shape = PlayerShape(player_pos, player_height, player_radius);
        }
    }
}

void Physics::run_tests() {
    std::cout << "Running tests " << std::endl;

    PlayerShape ps(glm::vec3(0, 0, 0), 1, 0.5);
    Triangle t1(glm::vec3(-1, -3, 1), glm::vec3(1, -3, 1), glm::vec3(0, -3, -1));

    glm::vec3 penet1 = resolve_penetration(ps, t1);
    assert(penet1 == glm::zero<glm::vec3>());
}

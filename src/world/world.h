#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../platform.h"
#include "../assets.h"

constexpr glm::vec3 unit_y(0, 1, 0);

struct Triangle {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 normal {};
    float area;

    explicit Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
        : p0(p0), p1(p1), p2(p2) {
        glm::vec3 c = cross(p1 - p0, p2 - p0);
        normal = normalize(c);
        area = glm::length(c) * 0.5f;
    }
};

class Player {
    glm::vec3 forward{};
    bool fly_move_enabled = true;
    void fly_move(const Input& input, float dt);
    void mouse_look(const Input& input, float dt);

public:
    glm::vec3 position{};

    Player();
    glm::mat4 get_view_matrix() const;
    bool get_fly_move_enabled() const;
    void process_input(const Input& input, float dt);
};

struct PlayerShape {
    glm::vec3 segment_up{};
    glm::vec3 segment_bottom{};
    glm::vec3 tip_up{};
    glm::vec3 tip_bottom{};
    glm::vec3 mid_point{};
    float radius;

    explicit PlayerShape(const glm::vec3& player_pos, float height, float r) : radius(r) {
        this->segment_up = player_pos + (unit_y * (height * 0.5f));
        this->segment_bottom = player_pos - (unit_y * (height * 0.5f));
        this->tip_up = this->segment_up + unit_y * this->radius;
        this->tip_bottom = this->segment_bottom - unit_y * this->radius;
        this->mid_point = (this->segment_bottom + this->segment_up) * 0.5f;
    }
};

class StaticCollider {
    std::vector<Triangle> triangles;
public:
    explicit StaticCollider(const ObjModelData& obj_data);
    const std::vector<Triangle>& get_triangles() const;
};

class Physics {
    std::vector<StaticCollider> static_colliders;
    static bool resolve_penetration(const PlayerShape& player_shape, const Triangle& triangle, glm::vec3& penetration);
public:
    void register_obj(const ObjModelData& obj_data);
    void tick(glm::vec3& player_pos, float dt) const;
    static void run_geom_tests();
    static void run_collision_tests();
};

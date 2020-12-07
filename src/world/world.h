#pragma once

#include "../platform.h"
#include "../assets.h"
#include "../vector3.h"

struct Triangle {
    Vector3 p0;
    Vector3 p1;
    Vector3 p2;
    Vector3 normal {};
    float area;

    explicit Triangle(const Vector3& p0_, const Vector3& p1_, const Vector3& p2_)
        : p0(p0_), p1(p1_), p2(p2_) {
        const Vector3 c = Vector3::cross(p1 - p0, p2 - p0);
        normal = Vector3::normalize(c);
        area = Vector3::length(c) * 0.5f;
    }
};

class Player {
    Vector3 forward{};
    bool fly_move_enabled = true;
    void fly_move(const Input& input, float dt);
    void mouse_look(const Input& input, float dt);

public:
    Vector3 position{};

    Player();
    Matrix4 get_view_matrix() const;
    bool get_fly_move_enabled() const;
    void process_input(const Input& input, float dt);
};

struct PlayerShape {
    Vector3 segment_up{};
    Vector3 segment_bottom{};
    Vector3 tip_up{};
    Vector3 tip_bottom{};
    Vector3 mid_point{};
    float radius;

    explicit PlayerShape(const Vector3& player_pos, float height, float r) : radius(r) {
        this->mid_point = player_pos;
        this->segment_up = player_pos + (Vector3::up * (height * 0.5f));
        this->segment_bottom = player_pos - (Vector3::up * (height * 0.5f));
        this->tip_up = this->segment_up + Vector3::up * this->radius;
        this->tip_bottom = this->segment_bottom - Vector3::up * this->radius;
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
    static bool resolve_penetration(const PlayerShape& player_shape, const Triangle& triangle, Vector3& penetration);
public:
    void register_obj(const ObjModelData& obj_data);
    void resolve_collisions(Vector3& player_pos) const;
    static void run_geom_tests();
    static void run_collision_tests();
};

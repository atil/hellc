#pragma once

//#include "../common.h"
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

struct Ray {
    Vector3 origin;
    Vector3 direction;

    explicit Ray(Vector3 o, Vector3 d) : origin(o), direction(d) { }

    Vector3 at(float t) const {
        return origin + t * direction;
    }
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

    void displace(const Vector3& displacement) {
        this->mid_point += displacement;
        this->segment_up += displacement;
        this->segment_bottom += displacement;
        this->tip_up += displacement;
        this->tip_bottom += displacement;
    }
};

struct StaticCollider {
    std::vector<Triangle> triangles;
    explicit StaticCollider(const ObjModelData& obj_data, const Vector3& position, const Vector3& rotation);
};

struct Physics {
    static Vector3 compute_penetrations(const Vector3& player_pos, const std::vector<StaticCollider>& static_colliders);
    static bool is_grounded(const Vector3& player_pos, const Vector3& player_move_dir_horz, const std::vector<StaticCollider>& static_colliders, Vector3
                            & ground_normal);
    static bool raycast(const Ray& ray, float max_dist, const std::vector<StaticCollider>& static_colliders, Ray& out);

    static bool resolve_penetration(const PlayerShape& player_shape, const Triangle& triangle, Vector3& penetration);
    static void run_geom_tests();
    static void run_collision_tests();
};

class World {

    std::vector<StaticCollider> static_colliders;

    // Player
    Vector3 player_position{ -10, 3, 0 };
    Vector3 player_forward{ 0, 0, -1 };
    Vector3 player_velocity{ 0, 0, 0 };
    bool is_prev_grounded = false;
    bool fly_move_enabled = false;
    void fly_move(const Platform& platform, float dt);
    void mouse_look(const Platform& platform, float dt);

public:

    void register_static_collider(const ObjModelData& obj_data, const Vector3& position, const Vector3& rotation);
    void tick(const Platform& platform, float dt);
    Matrix4 get_view_matrix() const;

};

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../platform.h"
#include "../assets.h"

struct Triangle {
    const glm::vec3 p0;
    const glm::vec3 p1;
    const glm::vec3 p2;
    const glm::vec3 normal;

	explicit Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
        : p0(p0), p1(p1), p2(p2), normal(normalize(cross(p1 - p0, p2 - p0))) {
	}
};

class Player {
    glm::vec3 forward {};
    bool fly_move_enabled = true;
    void fly_move(const Input& input, float dt);
    void mouse_look(const Input& input, float dt);

public:
    glm::vec3 position {};

    Player();
	glm::mat4 get_view_matrix() const;
    bool get_fly_move_enabled() const;
    void process_input(const Input& input, float dt);
};

class StaticCollider {
    std::vector<Triangle> triangles;
public:
    explicit StaticCollider(const ObjModelData& obj_data);

    const std::vector<Triangle>& get_triangles() const;
};

class Physics {
    std::vector<StaticCollider> static_colliders;

public:
    void register_obj(const ObjModelData& obj_data);
    void tick(glm::vec3& player_pos, float dt) const;
    void run_tests();
};


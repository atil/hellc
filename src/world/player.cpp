#include <iostream>

#include "world.h" 
#include "geom.h"
#include <tuple>

constexpr float fly_speed = 3.5f;
constexpr float sensitivity = 1.0f;

Matrix4 World::get_view_matrix() const {
    return Matrix4::look_at(this->player_position, this->player_forward, Vector3::up);
}

void World::fly_move(const Platform& platform, float dt) {

    const float displacement = fly_speed * dt;

    if (platform.get_key(KeyCode::Forward)) {
        this->player_position += displacement * this->player_forward;
    }
    if (platform.get_key(KeyCode::Back)) {
        this->player_position -= displacement * this->player_forward;
    }

    if (platform.get_key(KeyCode::Left)) {
        this->player_position -= displacement * Vector3::cross(this->player_forward, Vector3::up);
    }
    if (platform.get_key(KeyCode::Right)) {
        this->player_position += displacement * Vector3::cross(this->player_forward, Vector3::up);
    }

    if (platform.get_key(KeyCode::Up)) {
        this->player_position += displacement * Vector3::up;
    }
    if (platform.get_key(KeyCode::Down)) {
        this->player_position -= displacement * Vector3::up;
    }
}

void World::mouse_look(const Platform& platform, float dt) {
    std::tuple<float, float> mouse_delta = platform.get_mouse_delta();
    const float dx = std::get<0>(mouse_delta);
    const float dy = std::get<1>(mouse_delta);

    std::cout << dx << " ==== " << dy << std::endl;

    this->player_forward = Vector3::rotate_around(this->player_forward, Vector3::up, -dx * sensitivity * dt);
    const Vector3 left = Vector3::cross(this->player_forward, Vector3::up);
    this->player_forward = Vector3::rotate_around(this->player_forward, left, -dy * sensitivity * dt);
}

void accelerate(Vector3& player_velocity, const Vector3& wish_dir, float accel_coeff, float dt) {
    const float proj_speed = Vector3::dot(player_velocity, wish_dir);
    constexpr float max_speed= 0.01f;
    const float add_speed = max_speed - proj_speed;
    if (add_speed < 0.0f) {
        return;
    }

    float accel_amount = accel_coeff * max_speed * dt;
    if (accel_amount > add_speed) {
        accel_amount = add_speed;
    }

    player_velocity += wish_dir * accel_amount;
}

void apply_friction(Vector3& player_velocity, float dt) {
    const float speed = Vector3::length(player_velocity);
    if (speed < 0.001f) {
        player_velocity = Vector3::zero;
        return;
    }

    const float down_limit = std::max(speed, 0.001f);
    const float drop_amount = speed - (down_limit * 0.02f * dt);
    if (drop_amount > 0.0f) {
        player_velocity *= drop_amount / speed;
    }
}

void World::tick(const Platform& platform, float dt) {
    if (platform.get_key_down(KeyCode::ToggleFly)) {
        this->fly_move_enabled = !this->fly_move_enabled;
    }
    
    mouse_look(platform, dt);

    if (this->fly_move_enabled) {
        fly_move(platform, dt);
        return;
    }

    Vector3 wish_dir;
    Vector3 player_forward_horz = this->player_forward;
    player_forward_horz.y = 0;
    if (platform.get_key(KeyCode::Forward)) {
        wish_dir += player_forward_horz;
    } else if (platform.get_key(KeyCode::Back)) {
        wish_dir -= player_forward_horz;
    }

    if (platform.get_key(KeyCode::Left)) {
        wish_dir -= Vector3::cross(player_forward_horz, Vector3::up);
    } else if (platform.get_key(KeyCode::Right)) {
        wish_dir += Vector3::cross(player_forward_horz, Vector3::up);
    }

    if (Vector3::length(wish_dir) > 0.01f) {
        wish_dir = Vector3::normalize(wish_dir);
    }

    Vector3 ground_normal;
    if (Physics::is_grounded(this->player_position, Vector3::zero, this->static_colliders, ground_normal)) {

        constexpr float ground_accel = 0.3f;
        apply_friction(this->player_velocity, dt);
        accelerate(this->player_velocity, wish_dir, ground_accel, dt);
        this->player_velocity = project_vector_on_plane(this->player_velocity, ground_normal);
    } else {
        constexpr float gravity = 0.00001f;
        this->player_velocity -= Vector3::up * gravity;
    }

    this->player_position += this->player_velocity * dt;

    const Vector3 displacement = Physics::compute_penetrations(this->player_position, this->static_colliders);
    this->player_position += displacement;

}

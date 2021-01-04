#include "world.h" 
#include "geom.h"
#include <tuple>

constexpr float fly_speed = 30.0f;
constexpr float sensitivity = 2.0f;
constexpr float max_speed = 8.0f;
constexpr float gravity = 16.0f;
constexpr float ground_accel = 8.0f;
constexpr float ground_friction = 8.0f;
constexpr float jump_force = 6.0f;
constexpr float air_accel = 1.0f;
constexpr float air_decel = 1.0f;
constexpr float air_control_coeff = 8.0f;
constexpr float air_control_cpm_coeff = 8.0f;

Matrix4 World::get_view_matrix() const {
    return Matrix4::look_at(this->player_position, this->player_position + this->player_forward, Vector3::up);
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
        this->player_position -= displacement * Vector3::normalize(Vector3::cross(this->player_forward, Vector3::up));
    }
    if (platform.get_key(KeyCode::Right)) {
        this->player_position += displacement * Vector3::normalize(Vector3::cross(this->player_forward, Vector3::up));
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

    // TODO @TASK: Up-down angle limits
    // Rename forward to "look_forward" and introduce "body_forward"
    // then check the angle between look forward and body forward

    this->player_forward = Vector3::rotate_around(this->player_forward, Vector3::up, -dx * sensitivity * dt);
    const Vector3 left = Vector3::normalize(Vector3::cross(Vector3::up, this->player_forward));
    this->player_forward = Vector3::rotate_around(this->player_forward, left, dy * sensitivity * dt);

}

void accelerate(Vector3& player_velocity, const Vector3& wish_dir, float accel_coeff, float dt) {
    const float proj_speed = Vector3::dot(player_velocity, wish_dir);
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
    const float drop_amount = speed - (down_limit * ground_friction * dt);
    if (drop_amount > 0.0f) {
        player_velocity *= drop_amount / speed;
    }
}

void apply_air_control(Vector3& player_velocity, const Vector3& wish_dir, const Vector3& move_input, float dt) {
    Vector3 player_dir_horz = Vector3::normalize(player_velocity.horizontal());
    const float player_speed_horz = Vector3::length(player_velocity.horizontal());

    const float dot = Vector3::dot(player_dir_horz, wish_dir);
    if (dot > 0) {
        float k = air_control_coeff * dot * dot * dt;

        if (approx(move_input.x, 0)) {
            k *= air_control_cpm_coeff;
        }

        player_dir_horz = player_dir_horz * player_speed_horz + wish_dir * k;
        player_dir_horz = Vector3::normalize(player_dir_horz);
        player_velocity = (player_dir_horz * player_speed_horz).horizontal() + Vector3::up * player_velocity.y;
    }
}

void World::player_tick(const Platform& platform, float dt) {
    if (platform.get_key_down(KeyCode::ToggleFly)) {
        this->fly_move_enabled = !this->fly_move_enabled;
    }

    mouse_look(platform, dt);
    //Debug::log("pos %f %f %f", player_position.x, player_position.y, player_position.z);

    if (this->fly_move_enabled) {
        fly_move(platform, dt);
        return;
    }

    Vector3 wish_dir;
    Vector3 move_input;
    Vector3 player_forward_horz = this->player_forward;
    player_forward_horz.y = 0;
    if (platform.get_key(KeyCode::Forward)) {
        wish_dir += player_forward_horz;
        move_input += Vector3::forward;
    } else if (platform.get_key(KeyCode::Back)) {
        wish_dir -= player_forward_horz;
        move_input -= Vector3::forward;
    }

    if (platform.get_key(KeyCode::Left)) {
        wish_dir -= Vector3::cross(player_forward_horz, Vector3::up);
        move_input += Vector3::left;
    } else if (platform.get_key(KeyCode::Right)) {
        wish_dir += Vector3::cross(player_forward_horz, Vector3::up);
        move_input -= Vector3::left;
    }

    if (Vector3::length(wish_dir) > 0.01f) {
        wish_dir = Vector3::normalize(wish_dir);
    }

    const Vector3 horz_vel_dir = Vector3::normalize(this->player_velocity.horizontal());
    Vector3 ground_normal;
    const bool is_grounded = Physics::is_grounded(this->player_position, horz_vel_dir, this->static_colliders, ground_normal);
    if (is_grounded) {
        const bool is_gonna_jump = platform.get_key(KeyCode::Jump);
        if (this->is_prev_grounded && !is_gonna_jump) {
            apply_friction(this->player_velocity, dt);
        }

        accelerate(this->player_velocity, wish_dir, ground_accel, dt);
        this->player_velocity = project_vector_on_plane(this->player_velocity, ground_normal);

        if (is_gonna_jump) {
            this->player_velocity += Vector3::up * jump_force;
        }
    } else {
        const float air_coeff = Vector3::dot(wish_dir, this->player_velocity) > 0.0f ? air_accel : air_decel;
        accelerate(this->player_velocity, wish_dir, air_coeff, dt);
        if (move_input.z > 0.0001f) {
        }
        apply_air_control(this->player_velocity, wish_dir, move_input, dt);
        this->player_velocity += Vector3::down * gravity * dt;
    }

    this->player_position += this->player_velocity * dt;

    const Vector3 displacement = Physics::compute_penetrations(this->player_position, this->static_colliders);
    this->player_position += displacement;
    this->is_prev_grounded = is_grounded;

}

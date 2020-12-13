#include "world.h" 
#include <tuple>

#define MOVE_SPEED 3.5f
#define SENSITIVITY 1.f

Matrix4 Player::get_view_matrix() const {
    return Matrix4::look_at(this->position, this->forward, Vector3::up);
}

bool Player::get_fly_move_enabled() const {
    return this->fly_move_enabled;
}

void Player::fly_move(const Platform& platform, float dt) {

    const float fly_move_speed = MOVE_SPEED * dt;

    if (platform.get_key(KeyCode::Forward)) {
        this->position += fly_move_speed * this->forward;
    }
    if (platform.get_key(KeyCode::Back)) {
        this->position -= fly_move_speed * this->forward;
    }

    if (platform.get_key(KeyCode::Left)) {
        this->position -= fly_move_speed * Vector3::cross(this->forward, Vector3::up);
    }
    if (platform.get_key(KeyCode::Right)) {
        this->position += fly_move_speed * Vector3::cross(this->forward, Vector3::up);
    }

    if (platform.get_key(KeyCode::Up)) {
        this->position += fly_move_speed * Vector3::up;
    }
    if (platform.get_key(KeyCode::Down)) {
        this->position -= fly_move_speed * Vector3::up;
    }
}

void Player::mouse_look(const Platform& platform, float dt) {

    std::tuple<float, float> mouse_delta = platform.get_mouse_delta();
    const float dx = std::get<0>(mouse_delta);
    const float dy = std::get<1>(mouse_delta);

    this->forward = Vector3::rotate_around(this->forward, Vector3::up, -dx * SENSITIVITY * dt);
    const Vector3 left = Vector3::cross(this->forward, Vector3::up);
    this->forward = Vector3::rotate_around(this->forward, left, -dy * SENSITIVITY * dt);
}

Player::Player() {
    this->position = Vector3(0, 3, 1);
    this->forward = Vector3(0, 0, -1);
}

void Player::process_input(const Platform& platform, const float dt) {
    if (get_fly_move_enabled()) {
        fly_move(platform, dt);
        mouse_look(platform, dt);
        return;
    }

    // TODO @TASK: Gravity
}


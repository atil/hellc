#include "world.h" 

#define MOVE_SPEED 3.5f
#define SENSITIVITY 1.f

Matrix4 Player::get_view_matrix() const {
    return Matrix4::look_at(this->position, this->forward, Vector3::up);
}

bool Player::get_fly_move_enabled() const {
    return this->fly_move_enabled;
}

void Player::fly_move(const Input& input, float dt) {

    const float fly_move_speed = MOVE_SPEED * dt;

    if (input.forward) {
        this->position += fly_move_speed * this->forward;
    }
    if (input.back) {
        this->position -= fly_move_speed * this->forward;
    }

    if (input.left) {
        this->position -= fly_move_speed * Vector3::cross(this->forward, Vector3::up);
    }
    if (input.right) {
        this->position += fly_move_speed * Vector3::cross(this->forward, Vector3::up);
    }

    if (input.up) {
        this->position += fly_move_speed * Vector3::up;
    }
    if (input.down) {
        this->position -= fly_move_speed * Vector3::up;
    }
}

void Player::mouse_look(const Input& input, float dt) {
    const float dx = input.mouse_x - input.prev_mouse_x;
    const float dy = input.mouse_y - input.prev_mouse_y;

    this->forward = Vector3::rotate_around(this->forward, Vector3::up, -dx * SENSITIVITY * dt);
    const Vector3 left = Vector3::cross(this->forward, Vector3::up);
    this->forward = Vector3::rotate_around(this->forward, left, -dy * SENSITIVITY * dt);
}

Player::Player() {
    this->position = Vector3(0, 3, 1);
    this->forward = Vector3(0, 0, -1);
}

void Player::process_input(const Input& input, const float dt) {
    if (get_fly_move_enabled()) {
        fly_move(input, dt);
        mouse_look(input, dt);
        return;
    }

    // TODO @TASK: Gravity
}


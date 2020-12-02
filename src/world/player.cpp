#include "world.h" 

#define MOVE_SPEED 3.5f
#define SENSITIVITY 1.f

static glm::vec3 up(0.0f, 1.0f, 0.0f);

void print_vec3(const glm::vec3& vec) {
    printf("%f %f %f\n", vec.x, vec.y, vec.z);
}

glm::mat4 Player::get_view_matrix() const {
    return lookAt(this->position, this->position + this->forward, up);
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
        this->position -= fly_move_speed * glm::cross(this->forward, up);
    }
    if (input.right) {
        this->position += fly_move_speed * glm::cross(this->forward, up);
    }

    if (input.up) {
        this->position += fly_move_speed * up;
    }
    if (input.down) {
        this->position -= fly_move_speed * up;
    }
}

void Player::mouse_look(const Input& input, float dt) {
    const float dx = input.mouse_x - input.prev_mouse_x;
    const float dy = input.mouse_y - input.prev_mouse_y;

    const glm::quat horz_rot = glm::angleAxis(glm::radians(-dx * SENSITIVITY * dt), up);
    this->forward = horz_rot * this->forward;

    const glm::vec3 left = glm::cross(this->forward, up);
    const glm::quat vert_rot = glm::angleAxis(glm::radians(-dy * SENSITIVITY * dt), left);
    this->forward = vert_rot * this->forward;
}

Player::Player() {
    this->position = glm::vec3(0, 3, 1);
    this->forward = glm::vec3(0, 0, -1);
}

void Player::process_input(const Input& input, const float dt) {
    if (get_fly_move_enabled()) {
        fly_move(input, dt);
        mouse_look(input, dt);
        return;
    }
}


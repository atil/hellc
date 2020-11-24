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

Player::Player() {
    this->position = glm::vec3(0, 0, 1);
    this->forward = glm::vec3(0, 0, -1);
}

void Player::process_input(const Input& input, const float dt) {
	const float move_speed = MOVE_SPEED * dt;

    if (input.forward) {
        this->position += move_speed * this->forward;
    }
    if (input.back) {
        this->position -= move_speed * this->forward;
    }

    if (input.left) {
        this->position -= move_speed * glm::cross(this->forward, up);
    }
    if (input.right) {
        this->position += move_speed * glm::cross(this->forward, up);
    }

    if (input.up) {
        this->position += move_speed * up;
    }
    if (input.down) {
        this->position -= move_speed * up;
    }

	const float dx = input.mouse_x - input.prev_mouse_x;
	const float dy = input.mouse_y - input.prev_mouse_y;

    const glm::quat horz_rot = glm::angleAxis(glm::radians(-dx * SENSITIVITY * dt), up);
    this->forward = horz_rot * this->forward;

	const glm::vec3 left = glm::cross(this->forward, up);
	const glm::quat vert_rot = glm::angleAxis(glm::radians(-dy * SENSITIVITY * dt), left);
    this->forward = vert_rot * this->forward;
}

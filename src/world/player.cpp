#include "world.h" 

#define MOVE_SPEED 3.5f
#define SENSITIVITY 1.f

static glm::vec3 up(0.0f, 1.0f, 0.0f);

void print_vec3(const glm::vec3& vec) {
    printf("%f %f %f\n", vec.x, vec.y, vec.z);
}

glm::mat4 Player::get_view_matrix() const {
    // TODO @CLEANUP: Construct a lookAt matrix in whatever form the gl thing is expecting (probably row major?)
    // Don't need to write an entire matrix library. We just need to construct one matrix
    const glm::vec3 pos(this->position.x, this->position.y, this->position.z);
    const glm::vec3 forw(this->forward.x, this->forward.y, this->forward.z);

    return lookAt(pos, pos + forw, up);
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

    // TODO @CLEANUP: Rotating a vector around an axis
    glm::vec3 forw(this->forward.x, this->forward.y, this->forward.z);

    const glm::quat horz_rot = glm::angleAxis(glm::radians(-dx * SENSITIVITY * dt), up);
    forw = horz_rot * forw;

    const glm::vec3 left = glm::cross(forw, up);
    const glm::quat vert_rot = glm::angleAxis(glm::radians(-dy * SENSITIVITY * dt), left);
    forw = vert_rot * forw;
    this->forward = Vector3(forw.x, forw.y, forw.z);
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
}


#include "player.h" 

#define MOVE_SPEED 1.5f
#define SENSITIVITY 1.f

static glm::vec3 up(0.0f, 1.0f, 0.0f);

glm::mat4 get_view_matrix(Player* player) {
    return glm::lookAt(player->position, player->position + player->forward, up);
}

void init_player(Player* player) {
    player->position = glm::vec3(0, 0, 1);
    player->forward = glm::vec3(0, 0, -1);
}

void process_player_input(Player* player, Input* input, float dt) {
    float move_speed = MOVE_SPEED * dt;

    if (input->forward) {
        player->position += move_speed * player->forward;
    }
    if (input->back) {
        player->position -= move_speed * player->forward;
    }

    if (input->left) {
        player->position -= move_speed * glm::cross(player->forward, up);
    }
    if (input->right) {
        player->position += move_speed * glm::cross(player->forward, up);
    }

    if (input->up) {
        player->position += move_speed * up;
    }
    if (input->down) {
        player->position -= move_speed * up;
    }

    float dx = input->mouse_x - input->prev_mouse_x;
    float dy = input->mouse_y - input->prev_mouse_y;

    glm::quat horz_rot = glm::angleAxis(glm::radians(-dx * SENSITIVITY * dt), up);
    player->forward = horz_rot * player->forward;

    glm::vec3 left = glm::cross(player->forward, up);
    glm::quat vert_rot = glm::angleAxis(glm::radians(-dy * SENSITIVITY * dt), left);
    player->forward = vert_rot * player->forward;
}

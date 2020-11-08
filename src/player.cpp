#include "player.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#define MOVE_SPEED 0.05f
#define SENSITIVITY 0.1f

static glm::vec3 up(0.0f, 1.0f, 0.0f);

glm::mat4 get_view_matrix(Player* player) {
    return glm::lookAt(player->position, player->position + player->forward, up);
}

void process_player_input(Player* player, GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player->position += MOVE_SPEED * player->forward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player->position -= MOVE_SPEED * player->forward;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player->position -= MOVE_SPEED * glm::cross(player->forward, up);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player->position += MOVE_SPEED * glm::cross(player->forward, up);
    }
    
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    float dx = (float)(mouse_x - player->prev_mouse_x);
    float dy = (float)(mouse_y - player->prev_mouse_y);

    glm::quat horz_rot = glm::angleAxis(glm::radians(-dx * SENSITIVITY), up);
    player->forward = horz_rot * player->forward;

    glm::vec3 left = glm::cross(player->forward, up);
    glm::quat vert_rot = glm::angleAxis(glm::radians(-dy * SENSITIVITY), left);
    player->forward = vert_rot * player->forward;

    printf("%f %f %f\n", player->forward.x, player->forward.y,player->forward.z); 

    player->prev_mouse_x = mouse_x;
    player->prev_mouse_y = mouse_y;
}

void init_player(Player* player, GLFWwindow* window) {
    player->position = glm::vec3(0, 0, 1);
    player->forward = glm::vec3(0, 0, -1);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    player->prev_mouse_x = (float) x;
    player->prev_mouse_y = (float) y;
}

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct Player {
    glm::vec3 position;
    glm::vec3 forward;

    float prev_mouse_x;
    float prev_mouse_y;
};

void init_player(Player* player, GLFWwindow* window);
glm::mat4 get_view_matrix(Player* player);
void process_player_input(Player* player, GLFWwindow* window);

#endif

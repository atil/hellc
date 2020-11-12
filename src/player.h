#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "input.h"

struct Player {
    glm::vec3 position;
    glm::vec3 forward;
};

void init_player(Player* player);
glm::mat4 get_view_matrix(Player* player);
void process_player_input(Player* player, Input* input, float dt);
#endif

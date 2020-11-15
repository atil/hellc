#ifndef _WORLD_H_
#define _WORLD_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "input.h"

struct Triangle {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
};

Triangle* create_triangle(float* vertex_data, int vertex_count, int* index_data, int index_data_length);

struct Player {
    glm::vec3 position;
    glm::vec3 forward;
};

void init_player(Player* player);
glm::mat4 get_view_matrix(Player* player);
void process_player_input(Player* player, Input* input, float dt);
#endif

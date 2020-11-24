#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "platform.h"

struct Triangle {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
};

Triangle* create_triangle(float* vertex_data, int vertex_count, int* index_data, int index_data_length);

class Player {
    glm::vec3 position;
    glm::vec3 forward;
public:
    Player();
	glm::mat4 get_view_matrix() const;
    void process_input(const Input& input, float dt);

};

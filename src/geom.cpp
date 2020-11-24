#include "world.h"

// TODO: C++ify this

Triangle* create_triangle(float* vertex_data, int vertex_count, int* index_data, int index_data_length) {

    int triangle_count = index_data_length / 3;
    Triangle* triangles = (Triangle*) malloc(sizeof(Triangle) * triangle_count);

    for (int i = 0, triangle_index = 0; i < index_data_length; i += 3) {
        int vertex_index_0 = index_data[i];
        int vertex_index_1 = index_data[i + 1];
        int vertex_index_2 = index_data[i + 2];
        
        float x0 = vertex_data[vertex_index_0 * 3];
        float y0 = vertex_data[vertex_index_0 * 3 + 1];
        float z0 = vertex_data[vertex_index_0 * 3 + 2];

        float x1 = vertex_data[vertex_index_1 * 3];
        float y1 = vertex_data[vertex_index_1 * 3 + 1];
        float z1 = vertex_data[vertex_index_1 * 3 + 2];
        
        float x2 = vertex_data[vertex_index_2 * 3];
        float y2 = vertex_data[vertex_index_2 * 3 + 1];
        float z2 = vertex_data[vertex_index_2 * 3 + 2];

        Triangle tri;
        tri.p0 = glm::vec3(x0, y0, z0);
        tri.p1 = glm::vec3(x1, y1, z1);
        tri.p2 = glm::vec3(x2, y2, z2);
        triangles[triangle_index++] = tri;
    }

    return triangles;
}

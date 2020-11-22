#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "render.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

constexpr size_t floats_per_vertex = 8;
constexpr size_t bytes_per_vertex = floats_per_vertex * sizeof(float);
constexpr size_t bytes_per_face = 3 * bytes_per_vertex;

std::array<float, floats_per_vertex> get_single_vertex_data(int face_index, int vertex_index, const std::vector<int>& face_data, const ObjModelData& obj_data) {

    int offset_in_face = vertex_index * 3;
    int pos_index = face_data[face_index + offset_in_face];
    int uv_index = face_data[face_index + offset_in_face + 1];
    int norm_index = face_data[face_index + offset_in_face + 2];

    std::array<float, floats_per_vertex> single_vertex_data;
    single_vertex_data[0] = obj_data.position_data[pos_index * 3];
    single_vertex_data[1] = obj_data.position_data[pos_index * 3 + 1];
    single_vertex_data[2] = obj_data.position_data[pos_index * 3 + 2];
    single_vertex_data[3] = obj_data.uv_data[uv_index * 2];
    single_vertex_data[4] = obj_data.uv_data[uv_index * 2 + 1];
    single_vertex_data[5] = obj_data.normal_data[norm_index * 3];
    single_vertex_data[6] = obj_data.normal_data[norm_index * 3 + 1];
    single_vertex_data[7] = obj_data.normal_data[norm_index * 3 + 2];

    return single_vertex_data;
}

RenderUnit::RenderUnit(Material material, const ObjFaceData& obj_face_data, const ObjModelData& obj_data) {

    // TODO @PERF: We have duplicate vertex data in this case
    // We just copy whatever vertex info (pos/uv/norm) that the face data tells us
    // If we want to save buffer space, we must get the individual vertices
    // only once and arrange the index buffer accordingly

    const std::vector<int>& face_indices = obj_face_data.indices;

    int face_count = face_indices.size() / 9;
    float* vertex_data = (float*) malloc(face_count * bytes_per_face);
    int vertex_data_length = face_count * floats_per_vertex * 3;

    for (int face_index = 0; face_index < face_count; face_index++) {

        int face_data_index = face_index * 9;
        int vertex_index = 0;

        int buffer_offset = face_index * floats_per_vertex * 3 + vertex_index * floats_per_vertex;
        std::array<float, floats_per_vertex> first_vertex_data = get_single_vertex_data(face_data_index, vertex_index, face_indices, obj_data);
        memcpy(vertex_data + buffer_offset, first_vertex_data.data(), bytes_per_vertex);
        vertex_index++;

        buffer_offset += floats_per_vertex; // Advance one vertex
        std::array<float, floats_per_vertex> second_vertex_data = get_single_vertex_data(face_data_index, vertex_index, face_indices, obj_data);
        memcpy(vertex_data + buffer_offset, second_vertex_data.data(), bytes_per_vertex);
        vertex_index++;

        buffer_offset += floats_per_vertex; // Advance one vertex
        std::array<float, floats_per_vertex> third_vertex_data = get_single_vertex_data(face_data_index, vertex_index, face_indices, obj_data);
        memcpy(vertex_data + buffer_offset, third_vertex_data.data(), bytes_per_vertex);
        vertex_index++;
    }

    // Yes, vert count. We just enumerate the vertices
    int vertex_count = face_indices.size() / 3; 
    int* index_data = (int*) malloc(vertex_count * sizeof(int));
    for (int i = 0; i < vertex_count; i++) {
        index_data[i] = i;
    }
    this->index_data_length = vertex_count;

    // 
    // Fill GL buffers
    //

    glGenBuffers(1, &(this->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_data_length * sizeof(float), vertex_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &(this->vao));
    glBindVertexArray(this->vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glGenBuffers(1, &(this->ibo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->index_data_length * sizeof(int), index_data, GL_STATIC_DRAW);

    std::string image_path = "../assets/" + material.diffuse_texture_name;
    Image image(image_path);
    glGenTextures(1, &(this->tex_handle));
    glBindTexture(GL_TEXTURE_2D, this->tex_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image_data);

    this->shader_program_handle = load_shader_program("../src/render/world.glsl");
    glm::mat4 model = glm::mat4(1);
    // TODO: This is the same for everything. Coud be somewhere global maybe?
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 100.0f);

    glUseProgram(this->shader_program_handle);
    set_int(this->shader_program_handle, "u_texture", 0);
    set_mat4(this->shader_program_handle, "u_perspective", perspective);
    set_mat4(this->shader_program_handle, "u_model", model);

    free(vertex_data);
    free(index_data);
}

void RenderUnit::render(glm::mat4 player_view_matrix) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->tex_handle);
    set_mat4(this->shader_program_handle, "u_view", player_view_matrix);

    glDrawElements(GL_TRIANGLES, this->index_data_length, GL_UNSIGNED_INT, nullptr);
}

RenderUnit::~RenderUnit() {
    glDeleteVertexArrays(1, &(this->vao));
    glDeleteBuffers(1, &(this->vbo));
    glDeleteBuffers(1, &(this->ibo));
    glDeleteProgram(this->shader_program_handle);
}

Image::Image(const std::string& file_path) {
    stbi_set_flip_vertically_on_load(true);
    int n;
    this->image_data = stbi_load(file_path.c_str(), &(this->width), &(this->height), &n, 0);
    if (image_data == nullptr) {
        std::cout << "Couldn't load the image at: " << file_path << std::endl;
        return;
    }
}

Image::~Image() {
    stbi_image_free(this->image_data);
}



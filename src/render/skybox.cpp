#define GLEW_STATIC
#include <GL/glew.h>
#include "render.h"

constexpr float skybox_vertex_data[108] = {
    -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0,
    -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0,
    -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
    1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0,
    -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
    -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0,
    -1.0, 1.0, 1.0, -1.0, 1.0,
};

void load_skybox_face(const std::string& face_path, GLuint skybox_side) {
    const Image image(face_path, false);
    glTexImage2D(skybox_side, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image_data);
}

Skybox::Skybox(const std::string& skybox_path, const Matrix4& projection) {
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), skybox_vertex_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    this->cubemap_handle = 0;
    glGenTextures(1, &this->cubemap_handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemap_handle);
    load_skybox_face(skybox_path + "_front.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    load_skybox_face(skybox_path + "_back.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    load_skybox_face(skybox_path + "_left.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    load_skybox_face(skybox_path + "_right.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    load_skybox_face(skybox_path + "_top.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    load_skybox_face(skybox_path + "_bottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    this->shader = std::make_unique<Shader>("src/render/shader/skybox.glsl");
    this->shader->use();
    this->shader->set_mat4("u_projection", projection);
    this->shader->set_int("u_skybox", 0);
}

Skybox::~Skybox() {
    glDeleteTextures(1, &this->cubemap_handle);
    glDeleteBuffers(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
}


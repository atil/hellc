#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>
#include "render.h"
#include "render_debug.h"

//
// Directional light
//

DirectionalLight::DirectionalLight(const Vector3& dir) {
    constexpr float s = 100.0f; // Ortho volume size
    const Matrix4 proj = Matrix4::ortho(-s, s, -s, s, shadow_near_plane, shadow_far_plane);
    const Matrix4 view = Matrix4::look_at(dir, Vector3::zero, Vector3::up);
    this->view_proj = proj * view;

    this->shader = Shader("src/render/shader/shadowmap_depth_directional.glsl");
    this->shader.use();
    this->shader.set_mat4("u_light_vp", this->view_proj);
    this->shader.set_mat4("u_model", Matrix4::identity());

    glGenTextures(1, &this->depth_tex_handle);
    glBindTexture(GL_TEXTURE_2D, this->depth_tex_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float border_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowmap_size, shadowmap_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    this->fbo = 0;
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_tex_handle, 0);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DirectionalLight& DirectionalLight::operator=(DirectionalLight&& other) noexcept {
    this->shader = std::move(other.shader);
    this->fbo = other.fbo;
    other.fbo = 0;
    this->depth_tex_handle = other.depth_tex_handle;
    other.depth_tex_handle = 0;
    this->view_proj = other.view_proj;
    return *this;
}

DirectionalLight::~DirectionalLight() {
    glDeleteFramebuffers(1, &this->fbo);
    glDeleteTextures(1, &this->depth_tex_handle);
}

//
// Point light
// 

PointLight::PointLight(const PointLightProperties& params, int light_index) {
    this->properties = params;

    const Matrix4 proj = Matrix4::perspective(90.0f, 1.0f, shadow_near_plane, shadow_far_plane);

    const Vector3 pos = params.position;
    const Matrix4 v0 = Matrix4::look_at(pos, pos + Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
    const Matrix4 v1 = Matrix4::look_at(pos, pos + Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
    const Matrix4 v2 = Matrix4::look_at(pos, pos + Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
    const Matrix4 v3 = Matrix4::look_at(pos, pos + Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
    const Matrix4 v4 = Matrix4::look_at(pos, pos + Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f));
    const Matrix4 v5 = Matrix4::look_at(pos, pos + Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f));

    this->shader = Shader("src/render/shader/shadowmap_depth_point.glsl");
    this->shader.use();
    this->shader.set_mat4("u_shadow_matrices[0]", proj * v0);
    this->shader.set_mat4("u_shadow_matrices[1]", proj * v1);
    this->shader.set_mat4("u_shadow_matrices[2]", proj * v2);
    this->shader.set_mat4("u_shadow_matrices[3]", proj * v3);
    this->shader.set_mat4("u_shadow_matrices[4]", proj * v4);
    this->shader.set_mat4("u_shadow_matrices[5]", proj * v5);
    this->shader.set_float("u_far_plane", shadow_far_plane);
    this->shader.set_vec3("u_light_pos", pos);
    this->shader.set_int("u_light_index", light_index);
    this->shader.set_mat4("u_model", Matrix4::identity());

    check_gl_error("point_light_ctor");
}

PointLight& PointLight::operator=(PointLight&& other) noexcept {
    this->shader = std::move(other.shader);
    this->properties = other.properties;
    return *this;
}

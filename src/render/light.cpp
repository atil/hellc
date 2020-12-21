#define GLEW_STATIC
#include <GL/glew.h>
#include "render.h"

DirectionalLight::DirectionalLight(const Vector3& dir) {
    constexpr float s = 100.0f; // Ortho volume size
    const Matrix4 proj = Matrix4::ortho(-s, s, -s, s, directional_shadow_near_plane, directional_shadow_far_plane);
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

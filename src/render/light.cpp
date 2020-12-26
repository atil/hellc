#define GLEW_STATIC
#include <GL/glew.h>
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

    this->shader = std::make_unique<Shader>("src/render/shader/shadowmap_depth_directional.glsl");
    this->shader->use();
    this->shader->set_mat4("u_light_vp", this->view_proj);
    this->shader->set_mat4("u_model", Matrix4::identity());

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

    check_gl_error("directional_light");
}

DirectionalLight::~DirectionalLight() {
    glDeleteFramebuffers(1, &this->fbo);
    glDeleteTextures(1, &this->depth_tex_handle);
}

//
// Point light
// 

PointLight::PointLight(const PointLightInfo& point_light_info, int light_index) : info(point_light_info) {

    this->shader = std::make_unique<Shader>("src/render/shader/shadowmap_depth_point.glsl");
    this->shader->use();

    const Matrix4 proj = Matrix4::perspective(90.0f, 1.0f, shadow_near_plane, shadow_far_plane);

    const Vector3 dirs[6] = { Vector3::right, Vector3::left, Vector3::up, Vector3::down, Vector3::forward, Vector3::back };
    const Vector3 ups[6] = { Vector3::down, Vector3::down, Vector3::forward, Vector3::back, Vector3::down, Vector3::down };
    for (int i = 0; i < 6; i++) {
        Matrix4 view = Matrix4::look_at(this->info.position, this->info.position + dirs[i], ups[i]);
        const std::string mat_prop_name = "u_shadow_matrices[" + std::to_string(i) + "]";
        this->shader->set_mat4(mat_prop_name, proj * view);
    }

    this->shader->set_float("u_far_plane", shadow_far_plane);
    this->shader->set_vec3("u_light_pos", this->info.position);
    this->shader->set_int("u_light_index", light_index);
    this->shader->set_mat4("u_model", Matrix4::identity());

    check_gl_error("point_light_ctor");
}


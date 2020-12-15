#define GLEW_STATIC
#include <GL/glew.h>
#include "render.h"

constexpr size_t shadowmap_size = 2048;

tex_handle_t create_shadowmap_texture() {
    tex_handle_t depth_texture_handle;

    glGenTextures(1, &depth_texture_handle);
    glBindTexture(GL_TEXTURE_2D, depth_texture_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float border_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowmap_size, shadowmap_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    return depth_texture_handle;
}

DirectionalLight::DirectionalLight() : shader("shadowmap_depth_directional.glsl") {
    this->direction = Vector3(100.0f, -100.0f, -20.0f);
    this->view = Matrix4::look_at(-direction, direction, Vector3::up);

    const float s = 100.0f;
    this->projection = Matrix4::ortho(-s, s, -s, s, 0.01f, 1000.0f);

    const tex_handle_t depth_tex_handle = create_shadowmap_texture();

    glGenFramebuffers(1, &(this->fbo));
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex_handle, 0);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    this->shader.use();
    this->shader.set_mat4("u_light_v", this->view);
    this->shader.set_mat4("u_light_p", this->projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_tex_handle);
}

void DirectionalLight::fill_depth_texture(const std::vector<RenderUnit>& render_units) const {
    this->shader.use();
    glViewport(0, 0, shadowmap_size, shadowmap_size);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const RenderUnit& ru : render_units) {
        ru.render();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

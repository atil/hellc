#define GLEW_STATIC
#include <GL/glew.h>
#include "render.h"
#include "render_debug.h"
#include "../config.h"

constexpr float aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
const Matrix4 perspective = Matrix4::perspective(45.0f, aspect_ratio, near_plane, far_plane);

void create_point_light_cubemap_and_fbo(tex_handle_t& cubemap_handle, buffer_handle_t& fbo, int point_light_count) {
    
    glGenTextures(1, &cubemap_handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubemap_handle);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, shadowmap_size, shadowmap_size,
        6 * point_light_count, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

    check_gl_error("point_light_cubemap");

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap_handle, 0);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    check_gl_framebuffer_complete("point_light_fbo");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::Renderer() {
    glewInit(); // Needs to be after the glfw context creation
    // TODO @CLEANUP: This looks stupid. We're doing this stuff on a variable declaration in main()
    // Would look better in an Init function or something.
    glViewport(0, 0, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    const Vector3 directional_light_dir(55.0f, 55.0f, -50.0f);
    this->directional_light = std::make_unique<DirectionalLight>(directional_light_dir);
// 2.73503, 3.07614, 49.1217
    std::unique_ptr<PointLight> point_light_0 = std::make_unique<PointLight>(Vector3(24.0f, 2.0f, -3.0f), 8.0f, 0.25f, 0);
    this->point_lights.push_back(std::move(point_light_0));
    std::unique_ptr<PointLight> point_light_1 = std::make_unique<PointLight>(Vector3(3.0f, 3.0f, 50.0f), 8.0f, 0.25f, 1);
    this->point_lights.push_back(std::move(point_light_1));

    const int point_light_count = static_cast<int>(this->point_lights.size());

    create_point_light_cubemap_and_fbo(this->point_light_cubemap_handle, this->point_light_fbo, point_light_count);

    this->world_shader = std::make_unique<Shader>("src/render/shader/world.glsl");
    this->world_shader->use();
    this->world_shader->set_int("u_texture", 0);
    this->world_shader->set_mat4("u_projection", perspective);
    this->world_shader->set_mat4("u_model", Matrix4::identity()); // @CLEANUP: A system to handle different positions of objects
                      
    this->world_shader->set_vec3("u_directional_light_dir", directional_light_dir);
    this->world_shader->set_mat4("u_directional_light_vp", this->directional_light->view_proj);
    this->world_shader->set_int("u_shadowmap_directional", 1);
                      
    for (size_t i = 0; i < point_light_count; i++) {
        const std::string pos_prop_name = "u_point_lights[" + std::to_string(i) + "].position";
        this->world_shader->set_vec3(pos_prop_name, this->point_lights[i]->position);

        const std::string intensity_prop_name = "u_point_lights[" + std::to_string(i) + "].intensity";
        this->world_shader->set_float(intensity_prop_name, this->point_lights[i]->intensity);

        const std::string att_prop_name = "u_point_lights[" + std::to_string(i) + "].attenuation";
        this->world_shader->set_float(att_prop_name, this->point_lights[i]->attenuation);
    }
    this->world_shader->set_int("u_shadowmaps_point", 2);
    this->world_shader->set_int("u_point_light_count", point_light_count);
    this->world_shader->set_float("u_far_plane", shadow_far_plane);

    this->skybox = std::make_unique<Skybox>("assets/skybox/gehenna", perspective);
}

void Renderer::register_obj(const ObjModelData& obj_data) {
    for (const ObjSubmodelData& obj_face_data : obj_data.submodel_data) {
        for (const Material& m : obj_data.materials) {
            if (m.name == obj_face_data.material_name) {
                std::unique_ptr<RenderUnit> ru = std::make_unique<RenderUnit>(m, obj_face_data, obj_data);
                this->render_units.push_back(std::move(ru));
            }
        }
    }
}

void Renderer::render(const Matrix4& player_view_matrix) {
    
    // Directional shadow
    glViewport(0, 0, shadowmap_size, shadowmap_size);
    glBindFramebuffer(GL_FRAMEBUFFER, this->directional_light->fbo);
    glDisable(GL_CULL_FACE); // Write to depth buffer with all faces. Otherwise the backfaces won't cause shadows
    glClear(GL_DEPTH_BUFFER_BIT);
    this->directional_light->shader->use();
    for (auto& ru : this->render_units) {
        ru->render();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Point shadow
    glBindFramebuffer(GL_FRAMEBUFFER, this->point_light_fbo);
    glDisable(GL_CULL_FACE); // Can we do this before the framebuffer bind? Also for the directional light
    glClear(GL_DEPTH_BUFFER_BIT);
    for (auto& point_light : this->point_lights) {
        point_light->shader->use();
        for (auto& ru : render_units) {
            ru->render();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);

    // Draw to screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);
    this->world_shader->use();
    this->world_shader->set_mat4("u_view", player_view_matrix);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->directional_light->depth_tex_handle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, this->point_light_cubemap_handle);
    for (auto& ru : this->render_units) {
        ru->render();
    }
    
    // Skybox: fill fragments with depth == 1
    this->skybox->shader->use();
    glDepthFunc(GL_LEQUAL);
    Matrix4 skybox_view = player_view_matrix;
    skybox_view.data[3 * 4 + 0] = 0.0f; // Clear translation row
    skybox_view.data[3 * 4 + 1] = 0.0f;
    skybox_view.data[3 * 4 + 2] = 0.0f;
    this->skybox->shader->set_mat4("u_view", skybox_view);
    glBindVertexArray(this->skybox->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skybox->cubemap_handle);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    // TODO @TASK: Low-res effect
    // TODO @TASK: Text rendering
}

Renderer::~Renderer() {
    glDeleteTextures(1, &this->point_light_cubemap_handle);
    glDeleteBuffers(1, &this->point_light_fbo);
}

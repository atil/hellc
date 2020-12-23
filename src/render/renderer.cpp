#define GLEW_STATIC
#include <GL/glew.h>
#include "render.h"
#include "render_debug.h"
#include "../config.h"

constexpr float aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
const Matrix4 perspective = Matrix4::perspective(45.0f, aspect_ratio, 0.01f, 100.0f);

void create_point_light_cubemap_and_fbo(tex_handle_t& cubemap_handle, buffer_handle_t& fbo) {

    constexpr int point_light_count = 1; // @CLEANUP
    
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

    std::unique_ptr<Shader> a = std::make_unique<Shader>("asdf");

    const Vector3 directional_light_dir(55.0f, 55.0f, -50.0f);
    this->directional_light = DirectionalLight(directional_light_dir);

    PointLightProperties props;
    props.position = Vector3(24.0f, 2.0f, -3.0f);
    props.intensity = 16.0f;
    props.attenuation = 0.25f;
    this->point_light = PointLight(props, 0);
    create_point_light_cubemap_and_fbo(this->point_light_cubemap_handle, this->point_light_fbo);

    this->world_shader = std::make_unique<Shader>("src/render/shader/world.glsl");
    this->world_shader->use();
    this->world_shader->set_int("u_texture", 0);
    this->world_shader->set_mat4("u_projection", perspective);
    this->world_shader->set_mat4("u_model", Matrix4::identity()); // @CLEANUP
                      
    this->world_shader->set_vec3("u_directional_light_dir", directional_light_dir);
    this->world_shader->set_mat4("u_directional_light_vp", this->directional_light.view_proj);
    this->world_shader->set_int("u_shadowmap_directional", 1);
                      
    this->world_shader->set_int("u_shadowmaps_point", 2);
    this->world_shader->set_int("u_point_light_count", 1); // @CLEANUP
    this->world_shader->set_vec3("u_point_lights[0].position", this->point_light.properties.position); // @CLEANUP
    this->world_shader->set_float("u_point_lights[0].intensity", this->point_light.properties.intensity); // @CLEANUP
    this->world_shader->set_float("u_point_lights[0].attenuation", this->point_light.properties.attenuation); // @CLEANUP
    this->world_shader->set_float("u_far_plane", shadow_far_plane);

    this->skybox = Skybox("assets/skybox/gehenna", perspective);

}

void Renderer::register_obj(const ObjModelData& obj_data) {

    // NOTE @CPP: This is important. The vector must not reallocate memory during the loop
    // Otherwise it calls the dtors (and I think ctors afterwards) of the RenderUnits
    // which we don't want because we do GL stuff in there
    this->render_units.reserve(obj_data.submodel_data.size());

    for (const ObjSubmodelData& obj_face_data : obj_data.submodel_data) {
        for (const Material& m : obj_data.materials) {
            if (m.name == obj_face_data.material_name) {
                this->render_units.emplace_back(m, obj_face_data, obj_data);
            }
        }
    }
}

void Renderer::render(const Matrix4& player_view_matrix) {
    
    // Directional shadow
    glViewport(0, 0, shadowmap_size, shadowmap_size);
    glBindFramebuffer(GL_FRAMEBUFFER, this->directional_light.fbo);
    glDisable(GL_CULL_FACE); // Write to depth buffer with all faces. Otherwise the backfaces won't cause shadows
    glClear(GL_DEPTH_BUFFER_BIT);
    this->directional_light.shader.use();
    for (const RenderUnit& ru : render_units) {
        ru.render();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Point shadow
    glBindFramebuffer(GL_FRAMEBUFFER, this->point_light_fbo);
    glDisable(GL_CULL_FACE); // Can we do this before the framebuffer bind? Also for the directional light
    glClear(GL_DEPTH_BUFFER_BIT);
    this->point_light.shader.use();
    for (const RenderUnit& ru : render_units) {
        ru.render();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);

    // Draw to screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);
    this->world_shader->use();
    this->world_shader->set_mat4("u_view", player_view_matrix);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->directional_light.depth_tex_handle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, this->point_light_cubemap_handle);
    for (const RenderUnit& ru : this->render_units) {
        ru.render();
    }
    
    // Skybox: fill fragments with depth == 1
    this->skybox.render(player_view_matrix);

    // TODO @TASK: Point shadows
    // TODO @TASK: Low-res effect
    // TODO @TASK: Text rendering
}

Renderer::~Renderer() {
    glDeleteTextures(1, &this->point_light_cubemap_handle);
    glDeleteBuffers(1, &this->point_light_fbo);
}

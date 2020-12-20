#define GLEW_STATIC
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "render.h"
#include "../config.h"

constexpr float aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
const Matrix4 perspective = Matrix4::perspective(45.0f, aspect_ratio, 0.01f, 100.0f);

Renderer::Renderer() {
    glewInit(); // Needs to be after the glfw context creation
    // TODO @CLEANUP: This looks stupid
    glViewport(0, 0, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    const Vector3 directional_light_dir(15.0f, 55.0f, -5.0f);
    this->directional_light = DirectionalLight(directional_light_dir);

    this->world_shader = Shader("src/render/shader/world.glsl");
    this->world_shader.use();
    this->world_shader.set_vec3("u_directional_light_dir", directional_light_dir);
    this->world_shader.set_mat4("u_directional_light_vp", this->directional_light.view_proj);
    this->world_shader.set_int("u_texture", 0);
    this->world_shader.set_int("u_shadowmap_directional", 1);
    this->world_shader.set_mat4("u_projection", perspective);
    this->world_shader.set_mat4("u_model", Matrix4::identity());
}

std::vector<Material> load_mtl_file(const std::string& file_path) {
    std::vector<Material> materials;

    std::ifstream stream(file_path);

    if (!stream.is_open()) {
        std::cout << "Error opening mtl file" << file_path << std::endl;
        return materials;
    }

    std::string line;
    std::string command;
    while (std::getline(stream, line)) {
        if (line.find("newmtl") != 0) {
            continue; // Start parsing from here
        }

        Material m;
        std::istringstream line_stream;

        line_stream.str(line);
        line_stream >> command >> m.name; // newmtl

        while (!line.empty()) {
            std::getline(stream, line);
            line_stream.clear();
            line_stream.str(line);

            if (line.find("map_Kd") == 0) {
                line_stream >> command >> m.diffuse_texture_name; // map_Kd
            }
            else if (line.find("Kd") == 0) {
                float d0, d1, d2;
                line_stream >> command >> d0 >> d1 >> d2; // Kd
                m.diffuse[0] = d0;
                m.diffuse[1] = d1;
                m.diffuse[2] = d2;
            }
            else if (line.find('d') == 0) {
                line_stream >> command >> m.transparency; // d
            }
        }
        materials.push_back(m);
    }

    return materials;
}

void Renderer::register_obj(const ObjModelData& obj_data) {
    std::vector<Material> materials = load_mtl_file(obj_data.mtllib_path);

    // NOTE @CPP: This is important. The vector must not reallocate memory during the loop
    // Otherwise it calls the dtors (and I think ctors afterwards) of the RenderUnits
    // which we don't want because we do GL stuff in there
    this->render_units.reserve(obj_data.submodel_data.size());

    for (const ObjSubmodelData& obj_face_data : obj_data.submodel_data) {
        for (const Material& m : materials) {
            if (m.name == obj_face_data.material_name) {
                this->render_units.emplace_back(m, obj_face_data, obj_data);
            }
        }
    }
}

void Renderer::render(const Matrix4& player_view_matrix) {

    this->directional_light.shader.use();
    glViewport(0, 0, shadowmap_size, shadowmap_size);
    glBindFramebuffer(GL_FRAMEBUFFER, this->directional_light.fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    for (const RenderUnit& ru : render_units) {
        ru.render();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);

    this->world_shader.use();
    this->world_shader.set_mat4("u_view", player_view_matrix);

    for (const RenderUnit& ru : this->render_units) {
        ru.render();
    }

    // TODO @TASK: Skybox
    // TODO @TASK: Point shadows
    // TODO @TASK: Low-res effect
    // TODO @TASK: Text rendering
}

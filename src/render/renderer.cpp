#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include "render.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

Renderer::Renderer() {
    // TODO: This looks stupid
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
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
        line_stream.clear();

        std::getline(stream, line);
        line_stream.str(line);
        line_stream >> command >> m.diffuse_texture_name; // map_Kd
        line_stream.clear();

        std::getline(stream, line);
        line_stream.str(line);
        line_stream >> command >> m.diffuse[0] >> m.diffuse[1] >> m.diffuse[2]; // Kd
        line_stream.clear();

        std::getline(stream, line);
        line_stream.str(line);
        line_stream >> command >> m.transparency; // d
        line_stream.clear();

        materials.push_back(m);
    }

    return materials;
}

void Renderer::register_obj(const ObjModelData& obj_data) {
    std::vector<Material> materials = load_mtl_file(obj_data.mtllib_path);

    for (const ObjFaceData& obj_face_data : obj_data.face_data) {
        for (Material& m : materials) {
            if (m.name == obj_face_data.material_name) {
                RenderUnit* ru = new RenderUnit(m, obj_face_data, obj_data);
                this->render_units.push_back(ru);
            }
        }
    }
}

void Renderer::render(const glm::mat4& player_view_matrix) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (RenderUnit* ru : this->render_units) {
        ru->render(player_view_matrix);
    }
}

Renderer::~Renderer() {
    for (RenderUnit* ru : this->render_units) {
        delete ru;
    }
}

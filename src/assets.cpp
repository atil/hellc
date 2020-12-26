#define _CRT_SECURE_NO_WARNINGS // To suppress "sscanf is bad" warning
#include "assets.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>

std::vector<Material> load_mtl_file(const std::string& file_path) {
    std::vector<Material> materials;

    std::ifstream stream(file_path);

    if (!stream.is_open()) {
        std::cout << "Error opening mtl file: " << file_path << std::endl;
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
                m.diffuse = Vector3(d0, d1, d2);
            }
            else if (line.find('d') == 0) {
                line_stream >> command >> m.transparency; // d
            }
        }
        materials.push_back(m);
    }

    return materials;
}

ObjModelData::ObjModelData(const std::string& file_path) {
    std::ifstream stream(file_path);
    if (!stream.is_open()) {
        std::cout << "Failed to open obj model file" << file_path << std::endl;
        return;
    }

    std::string line;
    std::string command;

    // Ignore everything above mtllib
    while (line.find("mtllib") != 0) {
        std::getline(stream, line);
    }

    // Prepare mtl file directory
    std::istringstream line_stream(line);
    std::string mtllib_name;
    line_stream >> command >> mtllib_name;
    mtllib_name = mtllib_name.substr(2); // Remove the ./ in the beginning
    size_t slash_index = file_path.find_last_of('/');
    std::string mtllib_path = file_path.substr(0, slash_index) + "/" + mtllib_name;

    this->materials = load_mtl_file(mtllib_path);

    ObjSubmodelData current_submodel;
    while (std::getline(stream, line)) {
        line_stream.clear();
        line_stream.str(line);

        if (line.find('#') == 0) {
            continue;
        }

        if (line.find("vt") == 0) {
            float u, v;
            line_stream >> command >> u >> v;
            this->uv_data.emplace_back(u, v);
        }
        else if (line.find("vn") == 0) {
            float nx, ny, nz;
            line_stream >> command >> nx >> ny >> nz;
            this->normal_data.emplace_back(nx, ny, nz);
        }
        else if (line.find('v') == 0) {
            float x, y, z;
            line_stream >> command >> x >> y >> z;
            this->position_data.emplace_back(x, y, z);
        }
        else if (line.find("usemtl") == 0) {
            if (!current_submodel.material_name.empty()) {
                const ObjSubmodelData& prev_face_data(current_submodel);
                this->submodel_data.push_back(prev_face_data);
            }
            line_stream >> command >> current_submodel.material_name;
            current_submodel.faces.clear();
        }
        else if (line.find('f') == 0) {
            char dummy;
            ObjFaceData face_data;
            sscanf(line.c_str(), "%c %zu/%zu/%zu %zu/%zu/%zu %zu/%zu/%zu", &dummy,
                &face_data.position_indices[0], &face_data.uv_indices[0], &face_data.normal_indices[0],
                &face_data.position_indices[1], &face_data.uv_indices[1], &face_data.normal_indices[1],
                &face_data.position_indices[2], &face_data.uv_indices[2], &face_data.normal_indices[2]);

            for (size_t i = 0; i < 3; i++) {
                face_data.position_indices[i]--;
                face_data.uv_indices[i]--;
                face_data.normal_indices[i]--;
            }

            current_submodel.faces.push_back(face_data);
        }
    }

    this->submodel_data.push_back(current_submodel);
}


Vector3 read_vec3(const std::string& line, std::istringstream& line_stream) {
    line_stream.clear();
    line_stream.str(line);
    float x, y, z;
    line_stream >> x >> y >> z;

    return Vector3(x, y, z);
}

std::string read_string(const std::string& line, std::istringstream& line_stream) {
    line_stream.clear();
    line_stream.str(line);
    std::string str;
    line_stream >> str;
    return str;
}


float read_float(const std::string& line, std::istringstream& line_stream) {
    line_stream.clear();
    line_stream.str(line);
    float f;
    line_stream >> f;
    return f;
}

Scene read_scene(const std::string& file_path) {
    std::ifstream stream(file_path);
    if (!stream.is_open()) {
        std::cout << "Failed to open scene file" << file_path << std::endl;
        return { };
    }

    Scene scene;

    std::string line;
    std::istringstream line_stream;
    while (std::getline(stream, line)) {
        if (line.find("#worldspawn") == 0) {

            std::getline(stream, line);
            std::string obj_name = read_string(line, line_stream);

            std::getline(stream, line);
            Vector3 pos = read_vec3(line, line_stream);

            std::getline(stream, line);
            Vector3 rot = read_vec3(line, line_stream);

            scene.worldspawn.push_back({ obj_name, pos, rot });
        }
        else if (line.find("#point_light") == 0) {
            std::getline(stream, line);
            Vector3 pos = read_vec3(line, line_stream);

            std::getline(stream, line);
            Vector3 color = read_vec3(line, line_stream);

            std::getline(stream, line);
            float attenuation = read_float(line, line_stream);

            std::getline(stream, line);
            float intensity = read_float(line, line_stream);

            scene.point_light_info.push_back({ pos, color, attenuation, intensity });
        }
        else if (line.find("#directional_light") == 0) {
            std::getline(stream, line);
            Vector3 pos = read_vec3(line, line_stream);

            std::getline(stream, line);
            Vector3 color = read_vec3(line, line_stream);

            scene.directional_light_info = { pos, color };
        }
    }

    return scene;
}


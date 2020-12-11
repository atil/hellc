#define _CRT_SECURE_NO_WARNINGS // TODO @CLEANUP What was this for?
#include "assets.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>

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
    this->mtllib_path = file_path.substr(0, slash_index) + "/" + mtllib_name;

    ObjSubmodelData current_submodel;
    while (std::getline(stream, line)) {
        line_stream.clear();
        line_stream.str(line);

        if (line.find('#') == 0) {
            continue;
        } else if (line.find("vt") == 0) {
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


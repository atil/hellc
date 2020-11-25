#define _CRT_SECURE_NO_WARNINGS
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

    ObjFaceData current_face_data;
    while (std::getline(stream, line)) {
        line_stream.clear();
        line_stream.str(line);

        if (line.find('#') == 0) {
            continue;
        } else if (line.find("vt") == 0) {
            float u, v;
            line_stream >> command >> u >> v;
            this->uv_data.push_back(u);
            this->uv_data.push_back(v);
        } else if (line.find("vn") == 0) {
            float nx, ny, nz;
            line_stream >> command >> nx >> ny >> nz;
            this->normal_data.push_back(nx);
            this->normal_data.push_back(ny);
            this->normal_data.push_back(nz);
        } else if (line.find('v') == 0) {
            float x, y, z;
            line_stream >> command >> x >> y >> z;
            this->position_data.push_back(x);
            this->position_data.push_back(y);
            this->position_data.push_back(z);
        } else if (line.find("usemtl") == 0) {
            if (!current_face_data.material_name.empty()) {
	            const ObjFaceData& prev_face_data(current_face_data);
                this->face_data.push_back(prev_face_data);
            }
            line_stream >> command >> current_face_data.material_name;
            current_face_data.indices.clear();
        } else if (line.find('f') == 0) {
            char dummy;
            int face_indices[9];
            sscanf(line.c_str(), "%c %d/%d/%d %d/%d/%d %d/%d/%d", &dummy,
                    &face_indices[0], &face_indices[1], &face_indices[2],
                    &face_indices[3], &face_indices[4], &face_indices[5],
                    &face_indices[6], &face_indices[7], &face_indices[8]);

            for (int i = 0; i < 9; i++) {
                face_indices[i]--;
            }
            current_face_data.indices.insert(current_face_data.indices.end(), face_indices, face_indices + 9);
        }
    }

    this->face_data.push_back(current_face_data);
}


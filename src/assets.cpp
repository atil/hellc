#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "assets.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

char* read_file(const char* file_path) {
    FILE* f = fopen(file_path, "rb");
    if (!f) {
        printf("failed to open file %s", file_path);
        return nullptr;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = (char*) malloc(length + 1);
    fread(buffer, sizeof(char), length, f);
    fclose(f);
    buffer[length] = 0;

    return buffer;
}

ObjFileData::ObjFileData(const std::string& file_path) {
    std::ifstream stream(file_path);

    std::vector<float> positions;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<int> face_data;
    std::string mtl_file_path;

    //
    // Read the vertex/uv/normal/face data into memory
    //

    std::string line;
    std::string command;
    while (std::getline(stream, line)) {
        std::istringstream line_stream(line);

        if (line.find("#") == 0) {
            continue; // Comments
        } else if (line.find("mtllib") == 0) {
            line_stream >> command >> mtl_file_path;
        } else if (line.find("o") == 0) {
            line_stream >> command >> this->name;
        } else if (line.find("vt") == 0) {
            float u, v;
            line_stream >> command >> u >> v;
            uvs.push_back(u);
            uvs.push_back(v);
        } else if (line.find("vn") == 0) {
            float nx, ny, nz;
            line_stream >> command >> nx >> ny >> nz;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        } else if (line.find("v") == 0) {
            float x, y, z;
            line_stream >> command >> x >> y >> z;
            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        } else if (line.find("f") == 0) {
            char dummy[1];
            int pos_x_index, pos_y_index, pos_z_index;
            int uv_x_index, uv_y_index, uv_z_index;
            int norm_x_index, norm_y_index, norm_z_index;
            sscanf(line.c_str(), "%s %d/%d/%d %d/%d/%d %d/%d/%d", dummy,
                    &pos_x_index, &uv_x_index, &norm_x_index,
                    &pos_y_index, &uv_y_index, &norm_y_index,
                    &pos_z_index, &uv_z_index, &norm_z_index);

            int single_face_data[9];
            single_face_data[0] = pos_x_index - 1;
            single_face_data[1] = uv_x_index - 1;
            single_face_data[2] = norm_x_index - 1;

            single_face_data[3] = pos_y_index - 1;
            single_face_data[4] = uv_y_index - 1;
            single_face_data[5] = norm_y_index - 1;

            single_face_data[6] = pos_z_index - 1;
            single_face_data[7] = uv_z_index - 1;
            single_face_data[8] = norm_z_index - 1;

            face_data.insert(face_data.end(), single_face_data, single_face_data + 9);
        }
    }

    // 
    // Set batched data to be given directly to opengl
    //

    int face_count = face_data.size() / 9;
    const int vertex_length = 3 + 2 + 3; // pos - uv - normal
    this->batched_data_length = face_count * (3 * vertex_length); // 3 vertices per face
    this->batched_data = (float*) malloc(this->batched_data_length * sizeof(float));

    for (int i = 0, batched_data_index = 0; i < face_count; i++) {
        int face_start_index = i * 9;

        int pos_x_index = face_data[face_start_index];
        int uv_x_index = face_data[face_start_index + 1];
        int norm_x_index = face_data[face_start_index + 2];

        int pos_y_index = face_data[face_start_index + 3];
        int uv_y_index = face_data[face_start_index + 4];
        int norm_y_index = face_data[face_start_index + 5];

        int pos_z_index = face_data[face_start_index + 6];
        int uv_z_index = face_data[face_start_index + 7];
        int norm_z_index = face_data[face_start_index + 8];

        // TODO @PERF: Group the array usages together for cache-friendliness
        // That'd require each float's index to be precalculated in the batched array

        // First vertex's data
        this->batched_data[batched_data_index++] = positions[3 * pos_x_index];
        this->batched_data[batched_data_index++] = positions[3 * pos_x_index + 1];
        this->batched_data[batched_data_index++] = positions[3 * pos_x_index + 2];
        this->batched_data[batched_data_index++] = uvs[2 * uv_x_index];
        this->batched_data[batched_data_index++] = uvs[2 * uv_x_index + 1];
        this->batched_data[batched_data_index++] = normals[3 * norm_x_index];
        this->batched_data[batched_data_index++] = normals[3 * norm_x_index + 1];
        this->batched_data[batched_data_index++] = normals[3 * norm_x_index + 2];

        // Second vertex's data
        this->batched_data[batched_data_index++] = positions[3 * pos_y_index];
        this->batched_data[batched_data_index++] = positions[3 * pos_y_index + 1];
        this->batched_data[batched_data_index++] = positions[3 * pos_y_index + 2];
        this->batched_data[batched_data_index++] = uvs[2 * uv_y_index];
        this->batched_data[batched_data_index++] = uvs[2 * uv_y_index + 1];
        this->batched_data[batched_data_index++] = normals[3 * norm_y_index];
        this->batched_data[batched_data_index++] = normals[3 * norm_y_index + 1];
        this->batched_data[batched_data_index++] = normals[3 * norm_y_index + 2];

        // Third vertex's data
        this->batched_data[batched_data_index++] = positions[3 * pos_z_index];
        this->batched_data[batched_data_index++] = positions[3 * pos_z_index + 1];
        this->batched_data[batched_data_index++] = positions[3 * pos_z_index + 2];
        this->batched_data[batched_data_index++] = uvs[2 * uv_z_index];
        this->batched_data[batched_data_index++] = uvs[2 * uv_z_index + 1];
        this->batched_data[batched_data_index++] = normals[3 * norm_z_index];
        this->batched_data[batched_data_index++] = normals[3 * norm_z_index + 1];
        this->batched_data[batched_data_index++] = normals[3 * norm_z_index + 2];
    }

    this->batched_index_length = face_count * 3; // 3 vertices per face
    this->batched_index_data = (int*) malloc(this->batched_index_length * sizeof(int));

    for (int i = 0, index_data_index = 0; i < face_count; i++) {
        this->batched_index_data[index_data_index++] = i * 3;
        this->batched_index_data[index_data_index++] = i * 3 + 1;
        this->batched_index_data[index_data_index++] = i * 3 + 2;
    }

    mtl_file_path = mtl_file_path.substr(2); // Remove the ./ in the beginning
    size_t slash_index = file_path.find_last_of("/");
    std::string file_directory = file_path.substr(0, slash_index);
    std::vector<Material> m = load_mtl_file(file_directory + "/" + mtl_file_path);
    // after loading, create vertex buffers with faces that are to be rendered with the corresponding materials

}

ObjFileData::~ObjFileData() {
    free(this->batched_data);
    free(this->batched_index_data);
}

Image::Image(const std::string& file_path) {
    stbi_set_flip_vertically_on_load(true);
    int n;
    this->image_data = stbi_load(file_path.c_str(), &(this->width), &(this->height), &n, 0);
    if (image_data == nullptr) {
        std::cout << "Couldn't load the image at: " << file_path << std::endl;
        return;
    }
}

Image::~Image() {
    stbi_image_free(this->image_data);
}

std::vector<Material> ObjFileData::load_mtl_file(const std::string& file_path) {
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


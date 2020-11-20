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

std::vector<RenderUnit*> parse_faces(std::ifstream& stream,
        std::string first_line,
        const std::vector<Material>& materials,
        const std::vector<float>& positions,
        const std::vector<float>& uvs,
        const std::vector<float>& normals) 
{
    std::istringstream line_stream;
    std::string line = first_line;
    std::string command;

    line_stream.clear();
    line_stream.str(line);
    std::string mtl_name;
    line_stream >> command >> mtl_name;

    Material current_material = materials[0];
    for (Material m : materials) {
        if (m.name == mtl_name) {
            current_material = m;
        }
    }

    std::vector<int> current_face_data;
    std::vector<RenderUnit*> render_units;

    while (!stream.eof()) {
        std::getline(stream, line);
        if (line.find("f") == 0) {
            char dummy[1];
            int face_indices[9];
            sscanf(line.c_str(), "%s %d/%d/%d %d/%d/%d %d/%d/%d", dummy,
                    &face_indices[0], &face_indices[1], &face_indices[2],
                    &face_indices[3], &face_indices[4], &face_indices[5],
                    &face_indices[6], &face_indices[7], &face_indices[8]);

            for (int i = 0; i < 9; i++) {
                face_indices[i]--;
            }
            
            current_face_data.insert(current_face_data.end(), face_indices, face_indices + 9);
        } else {
            RenderUnit* ru = new RenderUnit(current_material, current_face_data, positions, uvs, normals);
            render_units.push_back(ru);

            while (std::getline(stream, line) && line.find("usemtl") != 0);
            line_stream.clear();
            line_stream.str(line);
            line_stream >> command >> mtl_name;

            for (Material m : materials) {
                if (m.name == mtl_name) {
                    current_material = m;
                }
            }
        }
    }

    return render_units;
}

std::vector<RenderUnit*> load_obj_file(const std::string& file_path) {
    std::ifstream stream(file_path);

    std::string line;
    std::string command;

    // Ignore everything above mtllib
    while (line.find("mtllib") != 0) {
        std::getline(stream, line);
    }

    std::vector<float> positions;
    std::vector<float> uvs;
    std::vector<float> normals;
    std::vector<RenderUnit*> render_units;

    // Read the materials from the .mtl file
    std::istringstream line_stream(line);
    std::string mtl_file_path;
    line_stream >> command >> mtl_file_path;
    mtl_file_path = mtl_file_path.substr(2); // Remove the ./ in the beginning
    size_t slash_index = file_path.find_last_of("/");
    std::string file_directory = file_path.substr(0, slash_index);
    std::vector<Material> materials = load_mtl_file(file_directory + "/" + mtl_file_path);

    while (std::getline(stream, line)) {
        line_stream.clear();
        line_stream.str(line);

        if (line.find("#") == 0) {
            continue; // Comments
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
        } else if (line.find("usemtl") == 0) {
            // Pos/uv/norm information is parsed until this point
            // From now on we only have the face information
            // The rest of the file is parsed in that function 
            // TODO: This is not good code. Feels weird
            render_units = parse_faces(stream, line, materials, positions, uvs, normals);
            break; 
        }
    }

    return render_units;
}

void get_vertex_info(const std::vector<int>& face_data, const std::vector<float>& position_data, const std::vector<float>& uv_data, const std::vector<float>& normal_data) {

}

RenderUnit::RenderUnit(Material material, const std::vector<int>& face_data, const std::vector<float>& position_data, const std::vector<float>& uv_data, const std::vector<float>& normal_data) {

    // TODO @PERF: We have duplicate vertex data in this case
    // We just copy whatever vertex info (pos/uv/norm) that the face data tells us
    // If we want to save buffer space, we must get the individual vertices
    // only once and arrange the index buffer accordingly

    this->material = material;

    const int floats_per_vertex = 8;
    const size_t bytes_per_vertex = floats_per_vertex * sizeof(float);
    int face_count = face_data.size() / 9;
    this->vertex_data = (float*) malloc(face_count * 3 * bytes_per_vertex);
    this->vertex_data_length = face_count * floats_per_vertex * 3;

    for (int i = 0, vertex_index = 0; i < face_data.size(); i += 9) {
        float first_vertex_data[floats_per_vertex];
        first_vertex_data[0] = position_data[face_data[i] * 3];
        first_vertex_data[1] = position_data[face_data[i] * 3 + 1];
        first_vertex_data[2] = position_data[face_data[i] * 3 + 2];
        first_vertex_data[3] = uv_data[face_data[i + 1] * 2];
        first_vertex_data[4] = uv_data[face_data[i + 1] * 2 + 1];
        first_vertex_data[5] = normal_data[face_data[i + 2] * 3];
        first_vertex_data[6] = normal_data[face_data[i + 2] * 3 + 1];
        first_vertex_data[7] = normal_data[face_data[i + 2] * 3 + 2];
        memcpy(this->vertex_data + vertex_index * floats_per_vertex, first_vertex_data, bytes_per_vertex);
        vertex_index++;

        float second_vertex_data[floats_per_vertex];
        second_vertex_data[0] = position_data[face_data[i + 3] * 3];
        second_vertex_data[1] = position_data[face_data[i + 3] * 3 + 1];
        second_vertex_data[2] = position_data[face_data[i + 3] * 3 + 2];
        second_vertex_data[3] = uv_data[face_data[i + 4] * 2];
        second_vertex_data[4] = uv_data[face_data[i + 4] * 2 + 1];
        second_vertex_data[5] = normal_data[face_data[i + 5] * 3];
        second_vertex_data[6] = normal_data[face_data[i + 5] * 3 + 1];
        second_vertex_data[7] = normal_data[face_data[i + 5] * 3 + 2];
        memcpy(this->vertex_data + vertex_index * floats_per_vertex, second_vertex_data, bytes_per_vertex);
        vertex_index++;

        float third_vertex_data[floats_per_vertex];
        third_vertex_data[0] = position_data[face_data[i + 6] * 3];
        third_vertex_data[1] = position_data[face_data[i + 6] * 3 + 1];
        third_vertex_data[2] = position_data[face_data[i + 6] * 3 + 2];
        third_vertex_data[3] = uv_data[face_data[i + 7] * 2];
        third_vertex_data[4] = uv_data[face_data[i + 7] * 2 + 1];
        third_vertex_data[5] = normal_data[face_data[i + 8] * 3];
        third_vertex_data[6] = normal_data[face_data[i + 8] * 3 + 1];
        third_vertex_data[7] = normal_data[face_data[i + 8] * 3 + 2];
        memcpy(this->vertex_data + vertex_index * floats_per_vertex, third_vertex_data, bytes_per_vertex);
        vertex_index++;
    }

    // Yes, vert count. We just enumerate the vertices
    int vertex_count = face_data.size() / 3; 
    this->index_data = (int*) malloc(vertex_count * sizeof(int));
    for (int i = 0; i < vertex_count; i++) {
        this->index_data[i] = i;
    }
    this->index_data_length = vertex_count;
}

RenderUnit::~RenderUnit() {
    free(this->vertex_data);
    free(this->index_data);
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



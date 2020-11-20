#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "assets.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>

constexpr size_t floats_per_vertex = 8;
constexpr size_t bytes_per_vertex = floats_per_vertex * sizeof(float);
constexpr size_t bytes_per_face = 3 * bytes_per_vertex;


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
        const ObjModelData& obj_data)
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
            RenderUnit* ru = new RenderUnit(current_material, current_face_data, obj_data);
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

    ObjModelData obj_data;
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
            obj_data.uv_data.push_back(u);
            obj_data.uv_data.push_back(v);
        } else if (line.find("vn") == 0) {
            float nx, ny, nz;
            line_stream >> command >> nx >> ny >> nz;
            obj_data.normal_data.push_back(nx);
            obj_data.normal_data.push_back(ny);
            obj_data.normal_data.push_back(nz);
        } else if (line.find("v") == 0) {
            float x, y, z;
            line_stream >> command >> x >> y >> z;
            obj_data.position_data.push_back(x);
            obj_data.position_data.push_back(y);
            obj_data.position_data.push_back(z);
        } else if (line.find("usemtl") == 0) {
            // Pos/uv/norm information is parsed until this point
            // From now on we only have the face information
            // The rest of the file is parsed in that function 
            // TODO: This is not good code. Feels weird
            // Probably gonna want to open the file the second time
            // and seek until this line
            render_units = parse_faces(stream, line, materials, obj_data);
            break; 
        }
    }

    return render_units;
}

std::array<float, floats_per_vertex> get_single_vertex_data(int face_index, int vertex_index, const std::vector<int>& face_data, const ObjModelData& obj_data) {

    int offset_in_face = vertex_index * 3;
    int pos_index = face_data[face_index + offset_in_face];
    int uv_index = face_data[face_index + offset_in_face + 1];
    int norm_index = face_data[face_index + offset_in_face + 2];

    std::array<float, floats_per_vertex> single_vertex_data;
    single_vertex_data[0] = obj_data.position_data[pos_index * 3];
    single_vertex_data[1] = obj_data.position_data[pos_index * 3 + 1];
    single_vertex_data[2] = obj_data.position_data[pos_index * 3 + 2];
    single_vertex_data[3] = obj_data.uv_data[uv_index * 2];
    single_vertex_data[4] = obj_data.uv_data[uv_index * 2 + 1];
    single_vertex_data[5] = obj_data.normal_data[norm_index * 3];
    single_vertex_data[6] = obj_data.normal_data[norm_index * 3 + 1];
    single_vertex_data[7] = obj_data.normal_data[norm_index * 3 + 2];

    return single_vertex_data;
}


RenderUnit::RenderUnit(Material material, const std::vector<int>& face_data, const ObjModelData& obj_data) {

    // TODO @PERF: We have duplicate vertex data in this case
    // We just copy whatever vertex info (pos/uv/norm) that the face data tells us
    // If we want to save buffer space, we must get the individual vertices
    // only once and arrange the index buffer accordingly

    this->material = material;

    int face_count = face_data.size() / 9;
    this->vertex_data = (float*) malloc(face_count * bytes_per_face);
    this->vertex_data_length = face_count * floats_per_vertex * 3;

    for (int face_index = 0; face_index < face_count; face_index++) {

        int face_data_index = face_index * 9;
        int vertex_index = 0;

        int buffer_offset = face_index * floats_per_vertex * 3 + vertex_index * floats_per_vertex;
        std::array<float, floats_per_vertex> first_vertex_data = get_single_vertex_data(face_data_index, vertex_index, face_data, obj_data);
        memcpy(this->vertex_data + buffer_offset, first_vertex_data.data(), bytes_per_vertex);
        vertex_index++;

        buffer_offset += floats_per_vertex; // Advance one vertex
        std::array<float, floats_per_vertex> second_vertex_data = get_single_vertex_data(face_data_index, vertex_index, face_data, obj_data);
        memcpy(this->vertex_data + buffer_offset, second_vertex_data.data(), bytes_per_vertex);
        vertex_index++;

        buffer_offset += floats_per_vertex; // Advance one vertex
        std::array<float, floats_per_vertex> third_vertex_data = get_single_vertex_data(face_data_index, vertex_index, face_data, obj_data);
        memcpy(this->vertex_data + buffer_offset, third_vertex_data.data(), bytes_per_vertex);
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



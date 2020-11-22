#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../assets.h"

int load_shader_program(const char* shader_path);
void set_int(int shader_program_id, const char* property_name, int i);
void set_vec3(int shader_program_id, const char* property_name, glm::vec3 v);
void set_mat4(int shader_program_id, const char* property_name, glm::mat4 m);
void set_float(int shader_program_id, const char* property_name, float f);
void delete_shader_program(int shader_program_id);

struct Material {
    std::string name;
    std::string diffuse_texture_name;
    float diffuse[3];
    float transparency;
};

class Image {
public:
    int width;
    int height;
    unsigned char* image_data;

    Image(const std::string& file_path);
    ~Image();
};

class RenderUnit {
private: 
    unsigned int shader_program_handle;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ibo;
    unsigned int tex_handle;
    int index_data_length;

public:
    RenderUnit(Material material, const ObjFaceData& obj_face_data, const ObjModelData& obj_data);
    ~RenderUnit();

    void render(glm::mat4 player_view_matrix);
};

class Renderer {
private:
    std::vector<RenderUnit> render_units;

public:
    Renderer();
    void register_obj(const ObjModelData& obj_data);
    void render(glm::mat4 player_view_matrix);
};


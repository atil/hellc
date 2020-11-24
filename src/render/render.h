#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../assets.h"

typedef unsigned int buffer_handle_t;
typedef unsigned int tex_handle_t;
typedef int uniform_loc_t;
typedef unsigned int shader_handle_t;

class Shader {
    shader_handle_t shader_program_handle;
    uniform_loc_t get_location(const std::string& property_name) const;
public:
	void set_int(const std::string& property_name, int i) const;
	void set_vec3(const std::string& property_name, const glm::vec3& v) const;
	void set_mat4(const std::string& property_name, const glm::mat4& m) const;
	void set_float(const std::string& property_name, float f) const;
    void use() const;

    explicit Shader(const std::string& file_path);
    Shader() = default;
    ~Shader();
};

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

    explicit Image(const std::string& file_path);
    ~Image();
};

class RenderUnit {

    Shader shader {};
    buffer_handle_t vao {};
    buffer_handle_t vbo {};
    buffer_handle_t ibo {};
    tex_handle_t tex_handle {};
    int index_data_length;

public:
    RenderUnit(const Material& material, const ObjFaceData& obj_face_data, const ObjModelData& obj_data);
    ~RenderUnit();

    void render(const glm::mat4& player_view_matrix) const;
};

class Renderer {
    std::vector<RenderUnit*> render_units;

public:
    Renderer();
    ~Renderer();
	
    void register_obj(const ObjModelData& obj_data);
    void render(const glm::mat4& player_view_matrix);
};

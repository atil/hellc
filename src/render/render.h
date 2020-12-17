#pragma once
#include <vector>
#include <string>
#include "../assets.h"
#include "../vector3.h"

constexpr size_t shadowmap_size = 2048;

typedef int uniform_loc_t;
typedef unsigned int shader_handle_t;
typedef unsigned int buffer_handle_t;
typedef unsigned int tex_handle_t;

class Shader {
    shader_handle_t shader_program_handle {0};
    uniform_loc_t get_location(const std::string& property_name) const;
public:
    void set_int(const std::string& property_name, int i) const;
    void set_vec3(const std::string& property_name, const Vector3& v) const;
    void set_mat4(const std::string& property_name, const Matrix4& m) const;
    void set_float(const std::string& property_name, float f) const;
    void use() const;

    explicit Shader(const std::string& file_path);
    Shader& operator=(Shader&& other) noexcept;
    explicit Shader() = default;
    ~Shader();
};

struct Material {
    std::string name;
    std::string diffuse_texture_name;
    float diffuse[3];
    float transparency;
};

struct Image {
    int width;
    int height;
    unsigned char* image_data;

    explicit Image(const std::string& file_path);
    ~Image();
};

class RenderUnit {
    buffer_handle_t vao{};
    buffer_handle_t vbo{};
    buffer_handle_t ibo{};
    tex_handle_t tex_handle{};
    size_t index_data_length;

    static const Matrix4 perspective;

public:
    RenderUnit(const Material& material, const ObjSubmodelData& obj_submodel_data, const ObjModelData& obj_data);
    ~RenderUnit();

    void render() const;
};

class DirectionalLight {
    Vector3 direction;
    Shader shader;
    buffer_handle_t fbo;
    
public:
    Matrix4 view;
    Matrix4 projection;

    DirectionalLight() = default;
    explicit DirectionalLight(const Vector3& dir);
    DirectionalLight& operator=(DirectionalLight&& other) noexcept;
    ~DirectionalLight();

    void fill_depth_texture(const std::vector<RenderUnit>& render_units) const;
};

class Renderer {
    std::vector<RenderUnit> render_units;
    Shader world_shader;
    DirectionalLight directional_light;

public:
    Renderer();

    void register_obj(const ObjModelData& obj_data);
    void render(const Matrix4& player_view_matrix);
};


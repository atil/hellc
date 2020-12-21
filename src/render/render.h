#pragma once
#include <vector>
#include <string>
#include "../assets.h"
#include "../vector3.h"

constexpr size_t shadowmap_size = 2048;
constexpr float near_plane = 0.01f;
constexpr float far_plane = 100.0f;
constexpr float directional_shadow_near_plane = 0.001f;
constexpr float directional_shadow_far_plane = 1000.0f;

typedef int uniform_loc_t;
typedef unsigned int shader_handle_t;
typedef unsigned int buffer_handle_t;
typedef unsigned int tex_handle_t;

class Shader {
    shader_handle_t shader_program_handle {0};
    uniform_loc_t get_location(const std::string& property_name) const;
public:
    void set_int(const std::string& property_name, int i) const;
    void set_vec3(const std::string& roperty_name, const Vector3& v) const;
    void set_mat4(const std::string& property_name, const Matrix4& m) const;
    void set_float(const std::string& property_name, float f) const;
    void use() const;

    explicit Shader() = default;
    explicit Shader(const std::string& file_path);
    Shader(Shader& other) = delete;
    Shader(Shader&& other) = delete;
    Shader& operator=(Shader& other) = delete;
    Shader& operator=(Shader&& other) noexcept;

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

    explicit Image(const std::string& file_path, bool flip_vertical);
    Image(const Image& other) = delete;
    Image(const Image&& other) = delete;
    Image& operator=(Image& other) = delete;
    Image& operator=(Image&& other) = delete;
    ~Image();
};

class RenderUnit {  // NOLINT(cppcoreguidelines-special-member-functions): This is used as a template type for a vector
    buffer_handle_t vao{};
    buffer_handle_t vbo{};
    buffer_handle_t ibo{};
    tex_handle_t tex_handle{};
    size_t index_data_length{};

    static const Matrix4 perspective;

public:
    RenderUnit(const Material& material, const ObjSubmodelData& obj_submodel_data, const ObjModelData& obj_data);
    ~RenderUnit();

    void render() const;
};

struct DirectionalLight {
    Shader shader;
    buffer_handle_t fbo;
    Matrix4 view_proj;
    tex_handle_t depth_tex_handle;

    DirectionalLight() = default;
    explicit DirectionalLight(const Vector3& dir);
    DirectionalLight(const DirectionalLight& other) = delete;
    DirectionalLight(const DirectionalLight&& other) = delete;
    DirectionalLight& operator=(DirectionalLight& other) = delete;
    DirectionalLight& operator=(DirectionalLight&& other) noexcept;
    ~DirectionalLight();
};

struct Skybox {
    Shader shader;
    tex_handle_t cubemap_handle;
    buffer_handle_t vao;
    buffer_handle_t vbo;

    Skybox() = default;
    explicit Skybox(const std::string& skybox_path, const Matrix4& projection);
    Skybox& operator=(Skybox&& other) noexcept;
    ~Skybox();
};

class Renderer {
    std::vector<RenderUnit> render_units;
    Shader world_shader;
    DirectionalLight directional_light;
    Skybox skybox;

public:
    Renderer();

    void register_obj(const ObjModelData& obj_data);
    void render(const Matrix4& player_view_matrix);
};


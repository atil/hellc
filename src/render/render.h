#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../assets.h"
#include "../vector3.h"


constexpr size_t shadowmap_size = 2048;
constexpr float near_plane = 0.01f;
constexpr float far_plane = 1000.0f;
constexpr float shadow_near_plane = 0.001f;
constexpr float shadow_far_plane = 1000.0f;
constexpr Vector2i draw_framebuffer_size(640, 360);

typedef int uniform_loc_t;
typedef unsigned int shader_handle_t;
typedef unsigned int buffer_handle_t;
typedef unsigned int tex_handle_t;

constexpr shader_handle_t default_shader_handle = 0;
constexpr buffer_handle_t default_buffer_handle = 0;
constexpr tex_handle_t default_tex_handle = 0;

class Shader {
    shader_handle_t shader_program_handle{ default_shader_handle };
    uniform_loc_t get_location(const std::string& property_name) const;
public:
    void set_int(const std::string& property_name, int i) const;
    void set_vec3(const std::string& roperty_name, const Vector3& v) const;
    void set_mat4(const std::string& property_name, const Matrix4& m) const;
    void set_float(const std::string& property_name, float f) const;
    void use() const;

    explicit Shader() = default;
    explicit Shader(const std::string& file_path);

    ~Shader();
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

class RenderUnit {
    buffer_handle_t vao{ default_buffer_handle };
    buffer_handle_t vbo{ default_buffer_handle };
    buffer_handle_t ibo{ default_buffer_handle };
    tex_handle_t tex_handle{ default_tex_handle };
    size_t index_data_length{ 0 };

    static const Matrix4 perspective;

public:
    RenderUnit(const Material& material, const ObjSubmodelData& obj_submodel_data, const ObjModelData& obj_data);
    ~RenderUnit();

    void render() const;
};

struct DirectionalLight {
    std::unique_ptr<Shader> shader;
    buffer_handle_t fbo{ default_buffer_handle };
    Matrix4 view_proj{ 0 };
    tex_handle_t depth_tex_handle{ default_tex_handle };

    DirectionalLight() = default;
    explicit DirectionalLight(const Vector3& dir);
    ~DirectionalLight();
};


struct PointLight {
    Vector3 position;
    Vector3 color;
    float intensity;
    float attenuation;
    std::unique_ptr<Shader> shader;

    PointLight(Vector3 position_, Vector3 color_, float intensity_, float attenuation_, int light_index);
};

struct Skybox {
    std::unique_ptr<Shader> shader;
    tex_handle_t cubemap_handle{ default_tex_handle };
    buffer_handle_t vao{ default_buffer_handle };
    buffer_handle_t vbo{ default_buffer_handle };

    Skybox() = default;
    explicit Skybox(const std::string& skybox_path, const Matrix4& projection);
    ~Skybox();
};

class Renderer {
    std::vector<std::unique_ptr<RenderUnit>> render_units;
    std::unique_ptr<Shader> world_shader;
    std::unique_ptr<DirectionalLight> directional_light;
    std::unique_ptr<Skybox> skybox;

    std::vector<std::unique_ptr<PointLight>> point_lights;
    tex_handle_t point_light_cubemap_handle { default_tex_handle };
    buffer_handle_t point_light_fbo { default_buffer_handle};

    buffer_handle_t draw_fbo;
    tex_handle_t draw_tex_handle;
    buffer_handle_t draw_rbo;

public:
    Renderer();
    ~Renderer();

    void register_obj(const ObjModelData& obj_data);
    void render(const Matrix4& player_view_matrix);

};


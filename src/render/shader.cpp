#define _CRT_SECURE_NO_WARNINGS // TODO @CLEANUP: Is it possible to remove these?
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include "render.h"

void check_gl_error_shader(const std::string& tag) {
    const int error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Shader GL error [" << tag << "] error code: [" << error << "]" << std::endl;
    }
}

shader_handle_t load_shader(const std::string& header, const std::string& program_string, int shader_type) {
    const std::string shader_string = header + program_string;
    const char* cstr = shader_string.c_str();

    const shader_handle_t shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &cstr, nullptr);
    glCompileShader(shader_id);

    // Check for compile errors
    GLint is_success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &is_success);
    if (is_success == GL_FALSE) {
        std::cout << "Failed to compile shader" << std::endl;
        int log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

        char* log = static_cast<char*>(malloc(log_length));
        glGetShaderInfoLog(shader_id, log_length, &log_length, log);

        std::cout << log << std::endl;
        free(log);
        glDeleteShader(shader_id);
        return -1;
    }

    return shader_id;
}

Shader::Shader(const std::string& file_path) {
    const std::ifstream shader_stream(file_path);
    if (!shader_stream.is_open()) {
        std::cout << "Failed to open shader file" << file_path << std::endl;
        this->shader_program_handle = -1;
        return;
    }

    std::stringstream buffer;
    buffer << shader_stream.rdbuf();
    const std::string program_string = buffer.str();

    const shader_handle_t vert_shader = load_shader("#version 450\n#define VERTEX\n", program_string, GL_VERTEX_SHADER);
    const shader_handle_t frag_shader = load_shader("#version 450\n#define FRAGMENT\n", program_string, GL_FRAGMENT_SHADER);

    const shader_handle_t shader_program = glCreateProgram();
    glAttachShader(shader_program, frag_shader);
    glAttachShader(shader_program, vert_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    // Check for link errors
    int is_success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
    if (is_success == GL_FALSE) {

        int log_length;
        glGetShaderiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);

        char* log = static_cast<char*>(malloc(log_length));
        glGetShaderInfoLog(shader_program, log_length, &log_length, log);

        std::cout << "Shader link error: " << log << std::endl;

        free(log);
        glDeleteShader(shader_program);
        this->shader_program_handle = -1;
        return;
    }

    this->shader_program_handle = shader_program;
    check_gl_error_shader("shader_init");
}

Shader& Shader::operator=(Shader&& other) noexcept {
    this->shader_program_handle = other.shader_program_handle;
    other.shader_program_handle = 0; // No valid shader gets this value
    return *this;
}

void Shader::use() const {
    glUseProgram(this->shader_program_handle);
}

uniform_loc_t Shader::get_location(const std::string& property_name) const {
    const uniform_loc_t loc = glGetUniformLocation(this->shader_program_handle, property_name.c_str());
    if (loc == -1) {
        std::cout << "Error when getting shader property location: " << property_name << std::endl;
        return -1;
    }
    return loc;
}

void check_uniform_error(const std::string& property_name) {
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error when setting uniform [" << property_name << "] error code: " << error << std::endl;
    }
}

void Shader::set_int(const std::string& property_name, int i) const {
    glUniform1i(this->get_location(property_name), i);
    check_uniform_error(property_name);
}

void Shader::set_vec3(const std::string& property_name, const Vector3& v) const {
    glUniform3f(get_location(property_name), v.x, v.y, v.z);
    check_uniform_error(property_name);
}

void Shader::set_mat4(const std::string& property_name, const Matrix4& m) const {
    glUniformMatrix4fv(get_location(property_name), 1, GL_FALSE, m.data);
    check_uniform_error(property_name);
}

void Shader::set_float(const std::string& property_name, float f) const {
    glUniform1f(get_location(property_name), f);
    check_uniform_error(property_name);
}

Shader::~Shader() {
    glDeleteProgram(this->shader_program_handle);
}


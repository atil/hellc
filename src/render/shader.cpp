#define _CRT_SECURE_NO_WARNINGS // TODO: Is it possible to remove these?
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "render.h"

int load_shader(const std::string& header, const std::string& program_string, int shader_type) {
    // Concat header with the whole program's string
    std::string shader_string = header + program_string;

    const char* cstr = shader_string.c_str();

    const int shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &cstr, nullptr);
    glCompileShader(shader_id);

    // Check for compile errors
    int is_success;
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

int load_shader_program(const std::string& shader_path) {
	const std::ifstream shader_stream(shader_path);
    if (!shader_stream.is_open()) {
        std::cout << "Failed to open shader file" << shader_path << std::endl;
        return -1;
    }
	std::stringstream buffer;
	buffer << shader_stream.rdbuf();
	const std::string program_string = buffer.str();

    int vert_shader = load_shader("#version 450\n#define VERTEX\n", program_string, GL_VERTEX_SHADER);
    int frag_shader = load_shader("#version 450\n#define FRAGMENT\n", program_string, GL_FRAGMENT_SHADER);

    int shader_program = glCreateProgram();
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
        return -1;
    }

    return shader_program;
}

int get_location(int shader_program_id, const char* property_name) {
    int loc = glGetUniformLocation(shader_program_id, property_name);
    if (loc == -1)
    {
        std::cout << "Couldn't find shader property: " << property_name << std::endl;
        return -1;
    }
    return loc;
}

void check_uniform_error(const char* property_name) {
    int error = GL_NO_ERROR;
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error when setting uniform " << property_name << "error code: " << error << std::endl;
    }
}

void set_int(int shader_program_id, const char* property_name, int i) {
    glUniform1i(get_location(shader_program_id, property_name), i);
    check_uniform_error(property_name);
}

void set_vec3(int shader_program_id, const char* property_name, glm::vec3 v) {
    glUniform3fv(get_location(shader_program_id, property_name), 1, (const float*) &v[0]);
    check_uniform_error(property_name);
}

void set_mat4(int shader_program_id, const char* property_name, glm::mat4 m) {
    glUniformMatrix4fv(get_location(shader_program_id, property_name), 1, GL_FALSE, &m[0][0]);
    check_uniform_error(property_name);
}

void set_float(int shader_program_id, const char* property_name, float f) {
    glUniform1f(get_location(shader_program_id, property_name), f);
    check_uniform_error(property_name);
}

void delete_shader_program(int shader_program_id) {
    glDeleteShader(shader_program_id);
}


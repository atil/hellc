#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "render.h"

char* read_file(const char* file_path) {
    FILE* f = fopen(file_path, "rb");
    if (!f) {
        std::cout << "Failed to open file " << file_path << std::endl;
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


int load_shader(const char* header, const char* program_string, int shader_type) {
    // Concat header with the whole program's string
    int shader_string_len = strlen(header) + strlen(program_string) + 1;
    char* shader_string = (char*) calloc(shader_string_len, sizeof(char));
    strcat(shader_string, header);
    strcat(shader_string, program_string);
    shader_string[shader_string_len] = '\0';

    int shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_string, NULL);
    glCompileShader(shader_id);
    free(shader_string);

    // Check for compile errors
    int is_success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &is_success);
    if (is_success == GL_FALSE) {
        std::cout << "Failed to compile shader" << std::endl;
        int log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

        char* log = (char*) malloc(log_length);
        glGetShaderInfoLog(shader_id, log_length, &log_length, log);

        printf("%s\n", log);
        free(log);
        glDeleteShader(shader_id);
        return -1;
    }

    return shader_id;
}

int load_shader_program(const char* shader_path) {
    char* program_string = read_file(shader_path);
    int vert_shader = load_shader("#version 450\n#define VERTEX\n", program_string, GL_VERTEX_SHADER);
    int frag_shader = load_shader("#version 450\n#define FRAGMENT\n", program_string, GL_FRAGMENT_SHADER);

    int shader_program = glCreateProgram();
    glAttachShader(shader_program, frag_shader);
    glAttachShader(shader_program, vert_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    free(program_string);

    // Check for link errors
    int is_success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &is_success);
    if (is_success == GL_FALSE) {

        int log_length;
        glGetShaderiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);

        char* log = (char*) malloc(log_length);
        glGetShaderInfoLog(shader_program, log_length, &log_length, log);

        free(log);
        glDeleteShader(shader_program);
        return -1;
    }

    return shader_program;
}

int get_location(int shader_program_id, const char* property_name) {
    /* printf("about to get the location for:\n"); */
    /* printf("%s %d\n", property_name, shader_program_id); */
    int loc = glGetUniformLocation(shader_program_id, property_name);
    /* printf("got location\n"); */
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


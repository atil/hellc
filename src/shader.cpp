#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "assets.h"
#include "render.h"

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
        printf("Failed to compile shader\n");
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

        printf("Failed to link shader program\n");
        int log_length;
        glGetShaderiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);

        char* log = (char*) malloc(log_length);
        glGetShaderInfoLog(shader_program, log_length, &log_length, log);

        printf("%s\n", log);
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
        printf("Couldn't find shader property: %s\n", property_name);
        return -1;
    }
    return loc;
}

void check_uniform_error(const char* property_name) {
    int error = GL_NO_ERROR;
    error = glGetError();
    if (error != GL_NO_ERROR) {
        printf("Error when setting uniform %s, error code: %d", property_name, error);
    }
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


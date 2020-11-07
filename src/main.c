#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including dev environment header
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Take-away: malloc works without this on windows, but it returns an int and not the pointer
// Turns out that, the linker won't complain if it can't stuff like malloc/free declarations
// probably beacuse these exist in the very-deep system libraries
// It'll assume the return value is sizeof(int) big and will happily move on
// On 32bit system it's not a problem since that's also the size of a pointer
// But on 64bit, it casts that integer to a pointer, which turns into an invalid one
// More info: https://stackoverflow.com/a/28172426/4894526
#include <stdlib.h>

#include "file.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


int compile_shader(const char* shader_path) {
    char* shader_string = read_file(shader_path);
    const char* vert_shader_header = "#version 410\n#define VERTEX\n";
    char* vert_string = (char*) calloc((strlen(vert_shader_header) + strlen(shader_string)), sizeof(char));
    strcat(vert_string, vert_shader_header);
    strcat(vert_string, shader_string);

    const char* frag_shader_header = "#version 410\n#define FRAGMENT\n";
    char* frag_string = (char*) calloc((strlen(frag_shader_header) + strlen(shader_string)), sizeof(char));
    strcat(frag_string, frag_shader_header);
    strcat(frag_string, shader_string);

    int vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_string, NULL);
    glCompileShader(vert_shader);

    int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_string, NULL);
    glCompileShader(frag_shader);

    int shader_program = glCreateProgram();
    glAttachShader(shader_program, frag_shader);
    glAttachShader(shader_program, vert_shader);
    glLinkProgram(shader_program);

    free(shader_string);
    return shader_program;
}

void load_obj(const char* file_path, char** object_name, float** vertices, int* vertex_count, int** faces, int* face_count) {
    char* obj_str = read_file(file_path);

    char* obj_str_copy = malloc(strlen(obj_str));
    strcpy(obj_str_copy, obj_str);

    const char* newline = "\r\n";

    *vertex_count = 0;
    *face_count = 0;

    // Determine lengths
    for (char* line = strtok(obj_str_copy, newline);
            line != NULL;
            line = strtok(NULL, newline)) {
        switch (*(line)) {
            case 'v':
                (*vertex_count)++;
                break;
            case 'f':
                (*face_count)++;
                break;
        }
    }

    //
    // Parse information
    //

    *vertices = (float*) malloc((*vertex_count) * 3 * sizeof(float));
    *faces = (int*) malloc((*face_count) * 3 * sizeof(int));

    int i_vertex = 0;
    int i_face = 0;

    for (char* line = strtok(obj_str, newline);
            line != NULL;
            line = strtok(NULL, newline)) {
        char tmp;
        switch (*(line)) {
            case 'o':
                int len = strlen(line);
                *object_name = (char*) malloc(len);
                sscanf(line, "%c %s", &tmp, *object_name);
                break;
            case 'v':
                float x, y, z;
                sscanf(line,"%c %f %f %f", &tmp, &x, &y, &z);
                (*vertices)[i_vertex++] = x;
                (*vertices)[i_vertex++] = y;
                (*vertices)[i_vertex++] = z;
                break;
            case 'f':
                int i1, i2, i3;
                sscanf(line,"%c %d %d %d", &tmp, &i1, &i2, &i3);
                (*faces)[i_face++] = i1;
                (*faces)[i_face++] = i2;
                (*faces)[i_face++] = i3;
                break;
        }
        /* printf("line: %s\n", line); */
    }

    free(obj_str);
}


int main() {
    char* object_name = NULL;
    float* vertices = NULL;
    int vertex_count = 0;
    int* faces = NULL;
    int face_count = 0;
    load_obj("../assets/triangle.obj", &object_name, &vertices, &vertex_count, &faces, &face_count);

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(640, 480, "here we go again", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit(); // Needs to be after the context creation

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);

    int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int shader_id = compile_shader("../src/world.glsl");

    glfwSetKeyCallback(window, key_callback);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_id);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, faces);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    free(object_name);
    free(vertices);
    free(faces);
    return 0;
}




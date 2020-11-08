#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including glfw dev environment header
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

#include "assets.h"
#include "render.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    char* object_name = NULL;
    float* vertices = NULL;
    int vertex_count = 0;
    int* faces = NULL;
    int face_count = 0;
    load_obj("../assets/triangle.obj", &object_name, &vertices, &vertex_count, &faces, &face_count);

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "here we go again", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit(); // Needs to be after the context creation
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // TODO: what does this do?

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glm::mat4 model = glm::mat4(1);

    glm::vec3 position = glm::vec3(0, 0, 1);
    glm::vec3 forward = glm::vec3(0, 0, -1);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::mat4 view = glm::lookAt(position, position + forward, up);

    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 100.0f);

    glm::mat4 mvp = perspective * view * model;


    int shader_program_id = load_shader_program("../src/world.glsl");
    glUseProgram(shader_program_id); // TODO: Move to glProgramUniform* functions to get rid of this
    set_mat4(shader_program_id, "u_mvp", mvp);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, faces);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete_shader_program(shader_program_id);

    free(object_name);
    free(vertices);
    free(faces);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    return 0;
}

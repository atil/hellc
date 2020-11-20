// TODO:
// debug with triangle, create a mtl for it

#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including glfw dev environment header
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <vector>

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
#include "input.h"
#include "world.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

void process_input(Input* input, GLFWwindow* window) {
    float prev_mouse_x = input->mouse_x;
    float prev_mouse_y = input->mouse_y;

    memset(input, 0, sizeof(Input));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        input->forward = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        input->back = true;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        input->left = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        input->right = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        input->up = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        input->down = true;
    }

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    input->mouse_x = (float) mouse_x;
    input->mouse_y = (float) mouse_y;
    input->prev_mouse_x = (float) prev_mouse_x;
    input->prev_mouse_y = (float) prev_mouse_y;
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "i don't know what i'm doing", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit(); // Needs to be after the context creation
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

    std::vector<RenderUnit*> render_units = load_obj_file("../assets/quad.obj");
    RenderUnit* ru = render_units[0];

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    Input* input = (Input*) malloc(sizeof(Input));
    input->mouse_x = (float) mouse_x;
    input->mouse_y = (float) mouse_y;

    Player* player = (Player*) malloc(sizeof(Player));
    init_player(player);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, ru->vertex_data_length * sizeof(float), ru->vertex_data, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ru->index_data_length * sizeof(int), ru->index_data, GL_STATIC_DRAW);

    /* Image* image = new Image("../assets/GridBox_Default.png"); */
    std::string image_path = "../assets/" + ru->material.diffuse_texture_name;
    Image* image = new Image(image_path);
    unsigned int tex_handle;
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->image_data);

    int shader_program_id = load_shader_program("../src/world.glsl");
    glm::mat4 model = glm::mat4(1);
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.01f, 100.0f);

    glUseProgram(shader_program_id);
    set_int(shader_program_id, "u_texture", 0);
    set_mat4(shader_program_id, "u_perspective", perspective);
    set_mat4(shader_program_id, "u_model", model);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float prev_time = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return 0;
        }
        float now_time = glfwGetTime();
        float delta_time = now_time - prev_time;
        prev_time = now_time;

        process_input(input, window);
        process_player_input(player, input, delta_time);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        set_mat4(shader_program_id, "u_view", get_view_matrix(player));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, ru->index_data_length, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    delete_shader_program(shader_program_id);

    free(input);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    return 0;
}

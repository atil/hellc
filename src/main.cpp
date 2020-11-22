// TODO
// - make it compile
// - probably you're gonna have to bind some stuff in the render
// - debug

#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including glfw dev environment header
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <stdlib.h>
#include "render/render.h"
#include "input.h"
#include "assets.h"
#include "world.h"
#include <GLFW/glfw3.h>
#define GLEW_STATIC
#include <GL/glew.h>

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

    Renderer renderer;

    ObjModelData obj_data("../assets/test_lighting.obj");
    renderer.register_obj(obj_data);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    Input* input = (Input*) malloc(sizeof(Input));
    input->mouse_x = (float) mouse_x;
    input->mouse_y = (float) mouse_y;

    Player* player = (Player*) malloc(sizeof(Player));
    init_player(player);

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

        renderer.render(get_view_matrix(player));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    free(input);
    return 0;
}

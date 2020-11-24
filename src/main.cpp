// TODO
// - problem with reading test_lighting.obj
// - take a look if there's any c-like thing is remaining

#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including glfw dev environment header
#include <GLFW/glfw3.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "render/render.h"
#include "input.h"
#include "assets.h"
#include "world.h"
#include "config.h"

void read_input(Input* input, GLFWwindow* window) {
	const float prev_mouse_x = input->mouse_x;
	const float prev_mouse_y = input->mouse_y;

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
    input->mouse_x = static_cast<float>(mouse_x);
    input->mouse_y = static_cast<float>(mouse_y);
    input->prev_mouse_x = static_cast<float>(prev_mouse_x);
    input->prev_mouse_y = static_cast<float>(prev_mouse_y);
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "i don't know what i'm doing", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit(); // Needs to be after the context creation
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

    Renderer renderer;

    const ObjModelData obj_data("assets/test_lighting.obj");
    renderer.register_obj(obj_data);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    Input input = { 0 };
    input.mouse_x = static_cast<float>(mouse_x);
    input.mouse_y = static_cast<float>(mouse_y);

    Player player;

    float prev_time = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return 0;
        }
        const float now_time = static_cast<float>(glfwGetTime());
        const float delta_time = now_time - prev_time;
        prev_time = now_time;

        read_input(&input, window);
        player.process_input(input, delta_time);

        renderer.render(player.get_view_matrix());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

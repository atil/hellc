#include "platform.h"
#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including glfw dev environment header
#include <GLFW/glfw3.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "config.h"

Platform::Platform() {

    // TODO @CLEANUP: Feels weird to only define a Platform variable doing this sort of thing
    glfwInit();
    this->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "started working on this early in the mornings", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit(); // Needs to be after the context creation
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    this->input = { 0 };
    input.mouse_x = static_cast<float>(mouse_x);
    input.mouse_y = static_cast<float>(mouse_y);

}

Platform::~Platform() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

const Input& Platform::get_input() const {
    return this->input;
}

void Platform::read_input() {

    // TODO @TASK: One shot key press.
    // Create KeyCode enum
    // Keep prev_keys vector3 to figure out which keys are released that frame

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    const float prev_mouse_x = this->input.mouse_x;
    const float prev_mouse_y = this->input.mouse_y;

    this->input.forward = glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS;
    this->input.back = glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS;
    this->input.left = glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS;
    this->input.right = glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS;
    this->input.up = glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS;
    this->input.down = glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

    double mouse_x, mouse_y;
    glfwGetCursorPos(this->window, &mouse_x, &mouse_y);
    this->input.mouse_x = static_cast<float>(mouse_x);
    this->input.mouse_y = static_cast<float>(mouse_y);
    this->input.prev_mouse_x = static_cast<float>(prev_mouse_x);
    this->input.prev_mouse_y = static_cast<float>(prev_mouse_y);
}

float Platform::get_time() {
    return static_cast<float>(glfwGetTime());
}

bool Platform::should_window_close() const {
    return glfwWindowShouldClose(this->window);
}

void Platform::end_frame() const {
    glfwSwapBuffers(this->window);
    glfwPollEvents();
}

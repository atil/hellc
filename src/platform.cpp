#include "platform.h"
#define GLFW_DLL // Dynamically linking glfw
#define GLFW_INCLUDE_NONE // Disable including glfw dev environment header
#include <tuple>
#include <GLFW/glfw3.h>
#include "config.h"

Platform::Platform() {

    // TODO @CLEANUP: Feels weird to only define a Platform variable doing this sort of thing
    glfwInit();
    this->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "started working on this early in the mornings", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    this->prev_mouse_x = static_cast<float>(mouse_x);
    this->prev_mouse_y = static_cast<float>(mouse_y);
}

Platform::~Platform() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

bool contains_key(const std::vector<KeyCode>& key_vec, KeyCode key_code) {
    for (auto it = key_vec.begin(); it != key_vec.end(); ++it) {
        if (*it == key_code) {
            return true;
        }
    }
    return false;
}

bool Platform::get_key(KeyCode key_code) const {
    return contains_key(this->current_keys, key_code);
}

bool Platform::get_key_down(KeyCode key_code) const {
    return contains_key(this->current_keys, key_code)
        && !contains_key(this->prev_keys, key_code);
}

bool Platform::get_key_up(KeyCode key_code) const {
    return !contains_key(this->current_keys, key_code)
        && contains_key(this->prev_keys, key_code);
}

std::tuple<float, float> Platform::get_mouse_delta() const {
    return std::make_tuple(this->mouse_dx, this->mouse_dy);
}

void Platform::read_input() {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    this->prev_keys = this->current_keys;
    this->current_keys.clear();

    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
        this->current_keys.push_back(KeyCode::Forward);
    }
    
    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
        this->current_keys.push_back(KeyCode::Left);
    }
    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
        this->current_keys.push_back(KeyCode::Back);
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) {
        this->current_keys.push_back(KeyCode::Right);
    }
    if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        this->current_keys.push_back(KeyCode::Up);
    }
    if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        this->current_keys.push_back(KeyCode::Down);
    }

    double mouse_x, mouse_y;
    glfwGetCursorPos(this->window, &mouse_x, &mouse_y);
    this->mouse_dx = static_cast<float>(mouse_x) - this->prev_mouse_x;
    this->mouse_dy = static_cast<float>(mouse_y) - this->prev_mouse_y;
    this->prev_mouse_x = static_cast<float>(mouse_x);
    this->prev_mouse_y = static_cast<float>(mouse_y);
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

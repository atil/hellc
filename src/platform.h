#pragma once
#include <vector>
struct GLFWwindow;

enum class KeyCode {
    Forward,
    Back,
    Left,
    Right,
    Up,
    Down,
    ToggleFly,
    
};

class Platform {
    GLFWwindow* window;

    std::vector<KeyCode> prev_keys;
    std::vector<KeyCode> current_keys;
    float prev_mouse_x;
    float prev_mouse_y;
    float mouse_dx;
    float mouse_dy;

public:
    Platform();
    ~Platform();

    void read_input();
    bool get_key(KeyCode key_code) const;
    bool get_key_down(KeyCode key_code) const;
    bool get_key_up(KeyCode key_code) const;
    std::tuple<float, float> get_mouse_delta() const;

    static float get_time();
    bool should_window_close() const;
    void end_frame() const;
};


#pragma once
#include <vector>
struct GLFWwindow;

enum KeyCode : char {
    Forward,
    Back,
    Left,
    Right,
    Up,
    Down,
    ToggleFly,
    

};

struct Input {
    bool quit_game;
    bool forward;
    bool back;
    bool left;
    bool right;
    bool up;
    bool down;
    bool toggle_fly;

    // Mouse
    float mouse_x;
    float mouse_y;
    float prev_mouse_x;
    float prev_mouse_y;
};

class Platform {
    GLFWwindow* window;
    Input input{};

    std::vector<KeyCode> prev_keys;
    std::vector<KeyCode> current_keys;

public:

    Platform();
    ~Platform();

    bool get_key(KeyCode key_code) const;
    bool get_key_down(KeyCode key_code) const;
    bool get_key_up(KeyCode key_code) const;
    void get_mouse_delta(float& dx, float& dy) const;

    const Input& get_input() const;
    void read_input();
    static float get_time();
    bool should_window_close() const;
    void end_frame() const;
};


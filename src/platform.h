#pragma once
struct GLFWwindow;

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
    Input input;
public:

    Platform();
    ~Platform();

    const Input& get_input() const;
    void read_input();
    static float get_time();
    bool should_window_close() const;
    void end_frame() const;
};


#ifndef _INPUT_H_
#define _INPUT_H_

struct Input {
    bool forward;
    bool back;
    bool left;
    bool right;
    bool up;
    bool down;
    float mouse_x;
    float mouse_y;
    float prev_mouse_x;
    float prev_mouse_y;
};

#endif

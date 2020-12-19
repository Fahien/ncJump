#pragma once

#include "types.h"


namespace jmp
{
struct Button {
    bool down = false;
    bool just_down = false;
};

struct Mouse {
    Button left = {};
    Vec2i pos = {0, 0};
};

struct Joystick {
    Vec2f move = {0.0, 0.0};
    Button a = {};
};

struct Input {
    Mouse mouse = {};
    Joystick joystick = {};

    void reset();
};

} // namespace jmp

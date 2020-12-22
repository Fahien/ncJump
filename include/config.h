#pragma once

#include "types.h"

namespace jmp
{
struct Scale {
    f32 global = 4.0f;
    f32 scene = 2.0f;
};

struct Size {
    u32 tile = 16;
};

/// @brief Game configuration
struct Config {
    Scale scale = {};
    Size size = {};
};

} // namespace jmp

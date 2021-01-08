#pragma once

#include "types.h"

namespace jmp
{
struct Scale {
    f32 window = 4.0f;
    f32 global = 4.0f;
    f32 scene = 2.0f;
};

struct Extent {
    u32 width = 480;
    u32 height = 320;
};

struct Size {
    u32 tile = 16;
    Extent window = {};
};

/// @brief Game configuration
class Config {
public:
    /// @return A new config from a json file, or a default instance if such a file does not exist
    static Config from_json(const char* path);

    Scale scale = {};
    Size size = {};
};

} // namespace jmp

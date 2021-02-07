#pragma once

#include "types.h"

namespace jmp
{
struct Scale {
    f32 window = 4.0f;
    f32 global = 4.0f;
    f32 gui = 2.0f;
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

struct Toggle {
    bool gui_demo = false;
    bool editor = false;
};

/// @brief Game configuration
struct Config {
    /// @return A new config from a json file, or a default instance if such a file does not exist
    static Config from_json(const char* path);

    /// @return Conversion of scene coordinates to screen coordinates
    inline Vec2i scene_to_screen(const Vec2f& p) const;

    Scale scale = {};
    Size size = {};
    Toggle toggle = {};
};

inline Vec2i Config::scene_to_screen(const Vec2f& p) const
{
    return Vec2i(p.x * scale.global, p.y * scale.global);
}

} // namespace jmp

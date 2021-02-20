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

inline Extent operator*(const Extent& e, const f32 f)
{
    return Extent {u32(e.width * f), u32(e.height * f)};
}

inline Extent operator/(const Extent& e, const f32 f)
{
    return Extent {u32(e.width / f), u32(e.height / f)};
}

struct Size {
    u32 tile = 16;
    Extent window = {};
};

struct Toggle {
    bool gui_demo = false;
    bool editor = false;
};

/// @brief Game configuration
class Config {
public:
    /// @return A new config from a json file, or a default instance if such a file does not exist
    static Config from_json(const char* path);

    inline Extent get_real_window_size() const;

    inline Extent get_scene_window_size() const;

    /// @return Conversion of screen coordinates to scene coordinates
    inline Vec2f screen_to_scene(const Vec2i& p) const;

    /// @return Conversion of scene coordinates to screen coordinates
    inline Vec2i scene_to_screen(const Vec2f& p) const;

    /// @return Like `scene_to_screen` but it returns an ImGui vector
    inline ImVec2 scene_to_gui(const Vec2f& p) const;

    Scale scale = {};
    Size size = {};
    Toggle toggle = {};
};

inline Extent Config::get_real_window_size() const
{
    return size.window * scale.window;
}

inline Extent Config::get_scene_window_size() const
{
    return get_real_window_size() / (scale.global * scale.scene);
}

inline Vec2f Config::screen_to_scene(const Vec2i& p) const
{
    return Vec2f(p.x / (scale.global * scale.scene), p.y / (scale.global * scale.scene));
}

inline Vec2i Config::scene_to_screen(const Vec2f& p) const
{
    return Vec2i(i32(p.x * scale.global * scale.scene), i32(p.y * scale.global * scale.scene));
}

inline ImVec2 Config::scene_to_gui(const Vec2f& p) const
{
    return ImVec2(p.x * scale.global * scale.scene,
        size.window.height * scale.window - p.y * scale.global * scale.scene);
}

} // namespace jmp

#pragma once

#include <cassert>
#include <cstdint>
#include <optional>
#include <string>

#include <ncine/FileSystem.h>
#include <ncine/Vector2.h>
#include <ncine/common_macros.h>
#include <nctl/Array.h>
#include <nctl/StaticArray.h>

namespace nc = ncine;

using i8 = int8_t;
using u8 = uint8_t;
using i32 = int32_t;
using u32 = uint32_t;
using usize = size_t;
using f32 = float;

using Vec2i = nc::Vector2<i32>;
using Vec2u = nc::Vector2<u32>;
using Vec2f = nc::Vector2<f32>;
constexpr auto None = std::nullopt;

#define ARRAY nctl::StaticArray
#define VECTOR nctl::Array
#define OPTION std::optional
#define UNIQUE nctl::UniquePtr
#define MK nctl::makeUnique
#define MV nctl::move

#define PATH(path) nc::fs::joinPath(nc::fs::dataPath(), path).data()

struct Direction {
    enum Value {
        NONE = -1,
        UP = 0,
        DOWN = 1,
        LEFT = 2,
        RIGHT = 3,
    };
};

enum class DirectionFlags {
    NONE = 0,
    UP = 1 << Direction::UP,
    DOWN = 1 << Direction::DOWN,
    LEFT = 1 << Direction::LEFT,
    RIGHT = 1 << Direction::RIGHT,
};

constexpr inline DirectionFlags operator~(DirectionFlags a)
{
    return (DirectionFlags) ~(i32)a;
}

constexpr inline DirectionFlags operator|(DirectionFlags a, DirectionFlags b)
{
    return (DirectionFlags)((i32)a | (i32)b);
}

constexpr inline DirectionFlags operator&(DirectionFlags a, DirectionFlags b)
{
    return (DirectionFlags)((i32)a & (i32)b);
}
constexpr inline DirectionFlags operator^(DirectionFlags a, DirectionFlags b)
{
    return (DirectionFlags)((i32)a ^ (i32)b);
}

constexpr inline DirectionFlags& operator|=(DirectionFlags& a, DirectionFlags b)
{
    return (DirectionFlags&)((i32&)a |= (i32)b);
}

constexpr inline DirectionFlags& operator&=(DirectionFlags& a, DirectionFlags b)
{
    return (DirectionFlags&)((i32&)a &= (i32)b);
}

constexpr inline DirectionFlags& operator^=(DirectionFlags& a, DirectionFlags b)
{
    return (DirectionFlags&)((i32&)a ^= (i32)b);
}

constexpr inline bool any(DirectionFlags a)
{
    return a != DirectionFlags::NONE;
}

static std::string to_str(DirectionFlags a)
{
    if (a == DirectionFlags::NONE) {
        return "NONE";
    }

    std::string ret;

    if (any(a & DirectionFlags::UP)) {
        ret += "UP, ";
    }

    if (any(a & DirectionFlags::DOWN)) {
        ret += "DOWN, ";
    }

    if (any(a & DirectionFlags::LEFT)) {
        ret += "LEFT, ";
    }

    if (any(a & DirectionFlags::RIGHT)) {
        ret += "RIGHT, ";
    }

    // Remove last ", "
    assert(ret.length() > 2);
    ret.resize(ret.length() - 2);

    return ret;
}

inline bool closef(const f32 a, const f32 b)
{
    const f32 eps = 1.0f;
    return fabsf(a - b) < eps;
}

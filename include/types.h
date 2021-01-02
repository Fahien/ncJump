#pragma once

#include <cstdint>
#include <optional>

#include <ncine/Vector2.h>
#include <nctl/StaticArray.h>
#include <ncine/common_macros.h>

namespace nc = ncine;

using i32 = int32_t;
using u32 = uint32_t;
using usize = size_t;
using f32 = float;

using Vec2i = nc::Vector2<i32>;
using Vec2f = nc::Vector2<f32>;
constexpr auto None = std::nullopt;

#define ARRAY nctl::StaticArray
#define OPTION std::optional
#define UNIQUE nctl::UniquePtr
#define MK nctl::makeUnique
#define MV nctl::move

#define PATH(path) nc::fs::joinPath(nc::fs::dataPath(), path).data()

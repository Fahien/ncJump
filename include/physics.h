#pragma once

#include <Box2D/Box2D.h>

#include "types.h"

namespace jmp
{
/// @brief Physics system
class Physics
{
public:
    Physics();

    void update(const f32 delta);

    b2Vec2 gravity;
    b2World world;

    i32 velocity_iterations = 6;
    i32 position_iterations = 2;
};

} // namespace jmp

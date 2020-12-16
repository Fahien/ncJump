#pragma once

#include <Box2D/Box2D.h>

#include "types.h"

namespace jmp
{
/// @brief Physics system
class Physics
{
public:
    /// @todo remove x and y parameters
    Physics(f32 x, f32 y);
    Collisions(f32 x, f32 y);

    void update(const float delta);

    b2Vec2 gravity;
    b2World world;

    b2Body* hero_body = nullptr;

    i32 velocity_iterations = 6;
    i32 position_iterations = 2;
};

} // namespace jmp

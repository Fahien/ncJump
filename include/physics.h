#pragma once

#include <box2d/box2d.h>

#include "system/physics/destruction.h"

namespace jmp
{
class Game;
class Tilemap;

/// @brief Physics system
class PhysicsSystem
{
public:
    PhysicsSystem(Game& game);

    /// @param tilemap Tiles can be destroyed on collisions with a high impact
    void update(const f32 delta, Tilemap& tilemap);

    b2Vec2 gravity;
    b2World world;

    i32 velocity_iterations = 6;
    i32 position_iterations = 2;

    DestructionListener destruction;
};

} // namespace jmp

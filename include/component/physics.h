#pragma once

#include <Box2D/Box2D.h>

#include "types.h"

namespace jmp
{
struct PhysicsComponent {
    PhysicsComponent() = default;

    void update();

    b2Body* body = nullptr;

    f32 air_factor = 1.0f / 16.0f;
    f32 velocity_factor = 64.0f;
    f32 jump_y_factor = 360.0f;
    f32 jump_x_factor = 3.0f;
    f32 max_x_speed = 8.0f;
};

} // namespace jmp

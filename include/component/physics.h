#pragma once

#include <Box2D/Box2D.h>

#include "types.h"

namespace jmp
{
class Physics;

struct PhysicsComponent {
    static PhysicsComponent tile(Physics& physics, const Vec2f& pos, bool dynamic);
    static PhysicsComponent character(Physics& physics, const Vec2f& pos);

    PhysicsComponent(Physics& physics);

    PhysicsComponent(PhysicsComponent&& o);
    PhysicsComponent& operator=(PhysicsComponent&& o) noexcept;

    ~PhysicsComponent();

    void update();

    Physics& physics;

    b2Body* body = nullptr;

    DirectionFlags obstacle;

    f32 air_factor = 1.0f / 16.0f;
    f32 velocity_factor = 32.0f;
    f32 jump_y_factor = 160.0f;
    f32 jump_x_factor = 3.0f;
    f32 max_x_speed = 6.0f;

    /// A destructible object must have a physics component to
    /// be able to detect collisions that can destroy it
    bool destructible = false;
};

} // namespace jmp

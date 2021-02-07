#pragma once

#include <vector>
#include <box2d/box2d.h>

#include "types.h"

namespace jmp
{
class PhysicsComponent {
public:
    static PhysicsComponent tile(b2World& world, const Vec2f& pos, bool dynamic);
    static PhysicsComponent character(b2World& world, const Vec2f& pos);

    PhysicsComponent() = default;

    PhysicsComponent(PhysicsComponent&& o);
    PhysicsComponent& operator=(PhysicsComponent&& o) noexcept;

    ~PhysicsComponent();

    void update();

    b2Body* body = nullptr;

    DirectionFlags obstacle;

    /// A list of obstacles for each Direction
    std::vector<b2Body*> obstacles_dir[4];

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

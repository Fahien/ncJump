#pragma once

#include <box2d/box2d.h>
#include <vector>

#include "model/defs.h"

namespace jmp
{
class PhysicsSystem;

class PhysicsComponent
{
public:
    PhysicsComponent() = default;
    PhysicsComponent(const PhysicsDef& def, PhysicsSystem& physics);

    PhysicsComponent(const PhysicsComponent& o);
    PhysicsComponent& operator=(const PhysicsComponent& o);

    PhysicsComponent(PhysicsComponent&& o);
    PhysicsComponent& operator=(PhysicsComponent&& o) noexcept;

    ~PhysicsComponent();

    Vec2f get_position() const;
    void set_position(const Vec2f& pos);

    void set_enabled(bool e);

    /// @brief Updates current obstacle flags and list of obstacle bodies
    void update(b2ContactEdge& contact);

    void update();

    /// @brief Resets some variables of the physics component
    /// Needs to be called at the end of every game update
    void reset();

    /// @return The normal of this contact relative to itself,
    /// which means that this vector will point towards this body.
    b2Vec2 get_normal(const b2Contact& contact) const;

    PhysicsDef def = {};

    b2Body* body = nullptr;

    bool enabled = true;

    DirectionFlags obstacle = DirectionFlags::NONE;

    /// A list of obstacles for each Direction
    std::vector<b2Body*> obstacles_dir[4];

    f32 air_factor = 1.0f / 16.0f;
    f32 speed = 32.0f;
    f32 jump_y_factor = 160.0f;
    f32 jump_x_factor = 3.0f;
    f32 max_x_speed = 6.0f;

    /// A destructible object must have a physics component to
    /// be able to detect collisions that can destroy it
    bool destructible = false;
};

} // namespace jmp

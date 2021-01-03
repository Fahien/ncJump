#include "component/physics.h"

#include "physics.h"

namespace jmp
{
PhysicsComponent PhysicsComponent::solid_tile(Physics& physics, const Vec2f& pos)
{
    auto def = b2BodyDef();
    def.position.Set(pos.x, pos.y);

    auto body = physics.world.CreateBody(&def);

    auto box = b2PolygonShape();
    box.SetAsBox(0.5f, 0.5f);

    auto fixture_def = b2FixtureDef();
    fixture_def.shape = &box;
    fixture_def.density = 0.0f;
    fixture_def.friction = 3.0f;
    body->CreateFixture(&fixture_def);

    auto ret = PhysicsComponent(physics);
    ret.body = body;

    return ret;
}

PhysicsComponent::PhysicsComponent(Physics& physics)
    : physics {physics}
{
}

PhysicsComponent::PhysicsComponent(PhysicsComponent&& o)
    : physics {o.physics}
    , body {o.body}
    , air_factor {o.air_factor}
    , velocity_factor {64.0f}
    , jump_y_factor {360.0f}
    , jump_x_factor {3.0f}
    , max_x_speed {8.0f}
{
    o.body = nullptr;
}

PhysicsComponent& PhysicsComponent::operator=(PhysicsComponent&& o) noexcept
{
    std::swap(body, o.body);
    std::swap(air_factor, o.air_factor);
    std::swap(velocity_factor, o.velocity_factor);
    std::swap(jump_y_factor, o.jump_y_factor);
    std::swap(jump_x_factor, o.jump_x_factor);
    std::swap(max_x_speed, o.max_x_speed);

    return *this;
}

PhysicsComponent::~PhysicsComponent()
{
    if (body) {
        physics.world.DestroyBody(body);
    }
}

void PhysicsComponent::update()
{
    assert(body && "Physics component has no body");

    // Apply air resistance
    auto vel = -body->GetLinearVelocity();
    auto vel_len = vel.LengthSquared();
    vel.x *= air_factor * vel_len;
    vel.y *= air_factor * vel_len;
    body->ApplyForceToCenter(vel, false);
}

} // namespace jmp

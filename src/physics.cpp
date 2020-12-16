#include "physics.h"

namespace jmp
{
Physics::Physics(f32 x, f32 y)
    : gravity {0.0, -20.0f}
    , world {gravity}
{
    // Ground body
    auto ground_def = b2BodyDef();
    ground_def.position.Set(0.0f, 0.0f);

    auto ground_body = world.CreateBody(&ground_def);

    auto ground_box = b2PolygonShape();
    ground_box.SetAsBox(32.0f, 0.5f);

    ground_body->CreateFixture(&ground_box, 0.0f);

    // Dynamic body
    auto hero_def = b2BodyDef();
    hero_def.type = b2_dynamicBody;
    hero_def.position.Set(x, y);

    hero_body = world.CreateBody(&hero_def);

    auto hero_box = b2PolygonShape();
    hero_box.SetAsBox(0.5f, 0.5f);

    auto hero_fixture_def = b2FixtureDef();
    hero_fixture_def.shape = &hero_box;
    hero_fixture_def.density = 16.0f;
    hero_fixture_def.friction = 1.4f;

    hero_body->CreateFixture(&hero_fixture_def);
}

void Physics::update(const f32 delta)
{
    world.Step(delta, velocity_iterations, position_iterations);
}

} // namespace spot::gfx

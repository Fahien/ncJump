#include "physics.h"

namespace jmp
{
Physics::Physics()
    : gravity {0.0, -30.0f}
    , world {gravity}
{
    // Ground body
    auto ground_def = b2BodyDef();
    ground_def.position.Set(0.0f, 0.0f);

    auto ground_body = world.CreateBody(&ground_def);

    auto ground_box = b2PolygonShape();
    ground_box.SetAsBox(32.0f, 0.5f);

    auto ground_fixture_def = b2FixtureDef();
    ground_fixture_def.shape = &ground_box;
    ground_fixture_def.density = 0.0f;
    ground_fixture_def.friction = 3.0f;
    ground_body->CreateFixture(&ground_fixture_def);
}

void Physics::update(const f32 delta)
{
    world.Step(delta, velocity_iterations, position_iterations);
}

} // namespace spot::gfx

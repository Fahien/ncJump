#include "physics.h"

namespace jmp
{
Physics::Physics()
    : gravity {0.0, -30.0f}
    , world {gravity}
{
}

void Physics::update(const f32 delta)
{
    world.Step(delta, velocity_iterations, position_iterations);
}

} // namespace spot::gfx

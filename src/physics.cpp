#include "physics.h"

#include "entity.h"
#include "game.h"
#include "tilemap.h"

namespace jmp
{
PhysicsSystem::PhysicsSystem(Game& game)
    : gravity {0.0, -30.0f}
    , world {gravity}
    , destruction {game.config, game.scene, game.resource}
{
    world.SetContactListener(&destruction);
}

void PhysicsSystem::update(const f32 delta, Tilemap& tilemap)
{
    world.Step(delta, velocity_iterations, position_iterations);

    // @todo Process destruction after all updates
    destruction.update(tilemap);
}

} // namespace spot::gfx

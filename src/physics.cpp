#include "physics.h"

#include "entity.h"
#include "game.h"
#include "tilemap.h"

namespace jmp
{

PhysicsSystem::PhysicsSystem(Game& game)
    : gravity {0.0, -30.0f}
    , world {b2_nullWorldId}
    , destruction {game.config, game.scene, game.resource}
{
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity;
    world = b2CreateWorld(&worldDef);

    destruction.setWorld(world);
}

PhysicsSystem::~PhysicsSystem()
{
    b2DestroyWorld(world);
    world = b2_nullWorldId;
}

void PhysicsSystem::update(const f32 delta, Tilemap& tilemap)
{
    b2World_Step(world, delta, 4);

    // @todo Process destruction after all updates
    destruction.update(tilemap);
}

} // namespace jmp

#pragma once

#include <Box2D/Box2D.h>
#include <ncine/ParticleInitializer.h>
#include <ncine/ParticleSystem.h>

#include "types.h"

namespace jmp
{
class Config;
class Tilemap;
class Entity;

class DestructionListener : public b2ContactListener
{
public:
    DestructionListener(Config& config, nc::SceneNode& scene, nc::Texture& texture);

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

    void emit_particles(Entity& entity);

    void check_destruction(b2Fixture& fixture);

    /// @brief To be called after physics update
    /// @param tilemap This is responsible for destroying tiles
    void update(Tilemap& tilemap);

    Config& config;

    VECTOR<Entity*> to_destroy;

    nc::ParticleSystem particles;
};

} // namespace jmp

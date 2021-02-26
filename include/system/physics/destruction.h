#pragma once

#include <box2d/box2d.h>
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

    /// @brief Checks whether one has been killed by the other
    void check_kill(const b2Contact& contact, b2Fixture& a, b2Fixture& b);
    void check_destruction(Entity& entity);

    /// @brief To be called after physics update
    /// @param tilemap This is responsible for destroying tiles
    void update(Tilemap& tilemap);

    Config& config;

    /// List of tiles to destroy
    VECTOR<Entity*> to_destroy;

    /// List of entities to kill
    VECTOR<Entity*> to_kill;

    nc::ParticleSystem particles;
};

} // namespace jmp

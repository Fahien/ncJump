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

class DestructionListener
{
public:
    DestructionListener(Config& config, nc::SceneNode& scene, nc::Texture& texture);

    void setWorld(b2WorldId worldId);

	void PostSolve();

    void emit_particles(Entity& entity);

    /// @brief Checks whether one has been killed by the other
    void check_kill(const b2ContactData& contact, b2ShapeId a, b2ShapeId b);
    void check_destruction(Entity& entity);

    /// @brief To be called after physics update
    /// @param tilemap This is responsible for destroying tiles
    void update(Tilemap& tilemap);

private:
    b2WorldId world = b2_nullWorldId;

    Config& config;

    /// List of tiles to destroy
    VECTOR<Entity*> to_destroy;

    /// List of entities to kill
    VECTOR<Entity*> to_kill;

    nc::ParticleSystem particles;
};

} // namespace jmp

#pragma once

#include "component/graphics_component.h"
#include "model/defs.h"
#include "types.h"

namespace jmp
{
class Game;
class Entity;
class Config;
class GraphicsFactory;
class PhysicsSystem;

class EntityFactory
{
public:
    static EntityDef create_player_def(const Config& config, GraphicsFactory& graphics_factory);

    EntityFactory(Game& game);

    /// @return A unique Entity. Heap allocated because a pointer to it is stored in the physics
    /// component.
    UNIQUE<Entity> create(const EntityDef& def, GraphicsFactory& gf, PhysicsSystem& ph);

    void add(EntityDef def, GraphicsFactory& gf);

    /// Entities definitions used to create entities when placed into a tilemap
    VECTOR<EntityDef> entities;

    /// Sub-graphics associated to the entities definitions used in the editor
    VECTOR<UNIQUE<SubGraphics>> graphics;
};

} // namespace jmp

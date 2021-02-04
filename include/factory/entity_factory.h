#pragma once

#include "types.h"

namespace jmp
{
class Physics;
class Entity;

class EntityFactory
{
public:
    EntityFactory(Physics& physics);

    /// It needs the physics system when creating physics bodies for new entities
    Physics& physics;

    /// Entity prototypes which will be cloned when placed into a tilemap
    VECTOR<UNIQUE<Entity>> entities;
};

} // namespace jmp

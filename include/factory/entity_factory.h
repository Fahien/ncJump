#pragma once

#include "types.h"

namespace jmp
{
class Game;
class Entity;

class EntityFactory
{
public:
    EntityFactory(Game& game);

    /// Entity prototypes which will be cloned when placed into a tilemap
    VECTOR<UNIQUE<Entity>> entities;
};

} // namespace jmp

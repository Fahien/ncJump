#pragma once

#include "entity.h"
#include "tileset.h"
#include "types.h"

namespace jmp
{
class Tilemap
{
public:
    Tilemap(nc::SceneNode& root, const Tileset& tileset);

    void set(u32 x, u32 y, Entity&& tile);

    u32 width = 24;
    u32 height = 10;

    nc::SceneNode node;

    nctl::Array<Entity> tiles;
};

} // namespace jmp

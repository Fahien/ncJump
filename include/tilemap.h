#pragma once

#include "tileset.h"
#include "types.h"

namespace jmp
{
class Tilemap
{
public:
    Tilemap(nc::SceneNode& root, const Tileset& tileset);

    void set(u32 x, u32 y, UNIQUE<nc::Sprite>&& tile);

    u32 width = 24;
    u32 height = 10;

    nc::SceneNode node;

    nctl::Array<UNIQUE<nc::Sprite>> tiles;
};

} // namespace jmp

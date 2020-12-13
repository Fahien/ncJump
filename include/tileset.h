#pragma once

#include <ncine/Texture.h>
#include <ncine/Sprite.h>
#include <nctl/Array.h>

#include "types.h"

namespace jmp
{
/// A tile set is a collection of Sprite nodes, one for each tile
/// Which are used/cloned to make a tile map, a game scene made of tiles
class Tileset
{
public:
    Tileset(nc::Texture& texture, u32 tile_size = 16);

    u32 tile_size = 16;

    u32 width = 0;
    u32 height = 0;

    nctl::Array<UNIQUE<nc::Sprite>> tiles;
};

} // namespace jmp

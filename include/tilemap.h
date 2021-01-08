#pragma once

#include "entity.h"
#include "tileset.h"
#include "types.h"

namespace jmp
{
class Game;

class Tilemap
{
public:
    Tilemap(Game& game, nc::SceneNode& root, const Tileset& tileset);

    void set(u32 x, u32 y, Entity&& tile);

    Game& game;

    u32 width = 64;
    u32 height = 32;

    nc::SceneNode node;

    nctl::Array<Entity> tiles;
};

} // namespace jmp

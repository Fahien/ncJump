#include "tilemap.h"

namespace jmp
{
Tilemap::Tilemap(nc::SceneNode& root, const Tileset& tileset)
    : node {&root}
{
    node.x += tileset.tile_size / 2;
    node.y += tileset.tile_size / 2;

    for (u32 i = 0; i < width; ++i) {
        for (u32 j = 0; j < height; ++j) {
            auto tile = tileset.create_tile(13);
            tile->x = i * tileset.tile_size;
            tile->y = j * tileset.tile_size;
            tile->setParent(&node);
            tiles.emplaceBack(MV(tile));
        }
    }
}

void Tilemap::set(u32 x, u32 y, UNIQUE<nc::Sprite>&& tile)
{
    if (x < width && y < height) {
        tile->setParent(&node);
        tile->x = x * tile->texRect().w;
        tile->y = y * tile->texRect().h;
        tiles[y + x * height] = MV(tile);
    }
}

} // namespace jmp

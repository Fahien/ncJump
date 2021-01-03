#include "tilemap.h"

#include "game.h"

namespace jmp
{
Tilemap::Tilemap(Game& game, nc::SceneNode& root, const Tileset& tileset)
    : game {game}
    , node {&root}
{
    node.x += tileset.tile_size / 2;
    node.y += tileset.tile_size / 2;

    for (u32 i = 0; i < width; ++i) {
        for (u32 j = 0; j < height; ++j) {
            auto tile = tileset.create_tile(13, game);
            tile.transform.node->x = i * tileset.tile_size;
            tile.transform.node->y = j * tileset.tile_size;
            tile.transform.node->setParent(&node);
            tiles.emplaceBack(MV(tile));
        }
    }
}

void Tilemap::set(u32 x, u32 y, Entity&& tile)
{
    if (x < width && y < height) {
        tile.transform.node->setParent(&node);
        auto& graphics = SingleGraphicsComponent::into(**tile.graphics);
        tile.transform.node->x = x * graphics.sprite->texRect().w;
        tile.transform.node->y = y * graphics.sprite->texRect().h;
        if (tile.physics) {
            tile.physics->body->SetTransform({float(x), float(y)}, 0);
        }

        tiles[y + x * height] = MV(tile);
    }
}

} // namespace jmp

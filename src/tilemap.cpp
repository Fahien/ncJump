#include "tilemap.h"

#include "game.h"

namespace jmp
{
Tilemap::Tilemap(Game& g)
{
    set_tileset(g.tileset);
    set_game(g);
}

void Tilemap::set_game(Game& g)
{
    ASSERT_MSG(!game, "Tilemap already initialized");

    game = &g;
    node = MK<nc::SceneNode>(&game->scene);

    node->x += game->config.size.tile / 2.0f;
    node->y += game->config.size.tile / 2.0f;

    if (tiles.isEmpty()) {
        set_tileset(g.tileset);
    }

    ASSERT(entities.isEmpty());

    for (i32 i = 0; i < width; ++i) {
        for (i32 j = 0; j < height; ++j) {
            entities.emplaceBack();
            set({i, j}, game->tileset, tiles[j + i * height]);
        }
    }
}

void Tilemap::set_tileset(const Tileset& tileset)
{
    for (u32 i = 0; i < width; ++i) {
        for (u32 j = 0; j < height; ++j) {
            tiles.emplaceBack(tileset.tiles[13]);
        }
    }
}

void Tilemap::set(const Vec2i& pos, const Tileset& tileset, Tile tile)
{
    if (pos.x < width && pos.y < height) {
        auto entity = tileset.create_entity(tile, *game);

        entity.transform.node->setParent(node.get());
        auto& graphics = SingleGraphicsComponent::into(**entity.graphics);
        entity.transform.node->x = pos.x * graphics.sprite->texRect().w;
        entity.transform.node->y = pos.y * graphics.sprite->texRect().h;
        if (entity.physics) {
            entity.physics->body->SetTransform({f32(pos.x), f32(pos.y)}, 0);
        }

        tiles[pos.y + pos.x * height] = MV(tile);
        entities[pos.y + pos.x * height] = MV(entity);
    }
}

} // namespace jmp

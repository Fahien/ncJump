#include "tileset.h"
#include "game.h"

namespace jmp
{
Tileset::Tileset(u32 tile_size)
    : tile_size {tile_size}
{
    ASSERT_MSG(tile_size > 0, "Tile size should be greater than zero");
}

Tileset::Tileset(nc::Texture& texture, u32 tile_size)
    : tile_size {tile_size}
{
    set_texture(texture);
}

void Tileset::set_texture(nc::Texture& t)
{
    texture = &t;

    ASSERT_MSG(texture->width() % tile_size == 0, "Wrong texture and tile size");
    ASSERT_MSG(texture->height() % tile_size == 0, "Wrong texture and tile size");

    width = texture->width() / tile_size;
    height = texture->height() / tile_size;

    if (tiles.isEmpty()) {
        for (u32 i = 0; i < width * height; ++i) {
            tiles.emplaceBack(Tile {i});
        }
    }

    sprites.clear();
    for (usize i = 0; i < width * height; ++i) {
        sprites.emplaceBack(create_sprite(i));
    }
}

UNIQUE<nc::Sprite> Tileset::create_sprite(u32 i) const
{
    u32 row = i / width;
    u32 col = i % width;

    u32 x = col * tile_size;
    u32 y = row * tile_size;

    nc::Recti tex_rect;
    tex_rect.x = x;
    tex_rect.y = y;
    tex_rect.w = tile_size;
    tex_rect.h = tile_size;

    auto sprite = MK<nc::Sprite>(texture);
    sprite->setTexRect(MV(tex_rect));

    return sprite;
}

UNIQUE<Entity> Tileset::create_entity(const Tile& tile, Game& game, bool dynamic) const
{
    auto entity = MK<Entity>();

    usize row = tile.id / width;
    usize col = tile.id % width;

    entity->transform.node->x = col * tile_size;
    entity->transform.node->y = row * tile_size;

    nc::Recti tex_rect;
    tex_rect.x = col * tile_size;
    tex_rect.y = row * tile_size;
    tex_rect.w = tile_size;
    tex_rect.h = tile_size;

    auto graphics = MK<SingleGraphicsComponent>(entity->transform, *texture);
    graphics->sprite->setTexture(texture);
    graphics->sprite->setTexRect(MV(tex_rect));
    entity->set_graphics(MV(graphics));

    if (!tile.passable) {
        auto physics =
            PhysicsComponent::tile(game.physics, entity->transform.node->position(), dynamic);
        physics.destructible = tile.destructible;
        entity->set_physics(MV(physics));
    }

    return entity;
}

} // namespace jmp

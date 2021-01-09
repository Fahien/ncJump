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
        for (usize i = 0; i < width * height; ++i) {
            tiles.emplaceBack(Tile());
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

Entity Tileset::create_entity(u32 i, Game& game) const
{
    auto tile = Entity();

    usize row = i / width;
    usize col = i % width;

    tile.transform.node->x = col * tile_size;
    tile.transform.node->y = row * tile_size;

    nc::Recti tex_rect;
    tex_rect.x = col * tile_size;
    tex_rect.y = row * tile_size;
    tex_rect.w = tile_size;
    tex_rect.h = tile_size;

    auto graphics = SingleGraphicsComponent(tile.transform, *texture);
    graphics.sprite->setTexture(texture);
    graphics.sprite->setTexRect(MV(tex_rect));
    tile.graphics = MK<SingleGraphicsComponent>(MV(graphics));

    auto& desc = tiles[i];
    if (!desc.passable) {
        tile.physics = PhysicsComponent::solid_tile(game.physics, tile.transform.node->position());
    }

    return tile;
}

} // namespace jmp

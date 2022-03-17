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

void Tileset::set_texture(nc::Texture& texture)
{
    ASSERT_MSG(texture.width() % tile_size == 0, "Wrong texture and tile size");
    ASSERT_MSG(texture.height() % tile_size == 0, "Wrong texture and tile size");

    width = texture.width() / tile_size;
    height = texture.height() / tile_size;

    if (tiles.isEmpty()) {
        for (u32 i = 0; i < width * height; ++i) {
            tiles.emplaceBack(Tile {i});
        }
    }

    sprites.clear();
    for (u32 i = 0; i < width * height; ++i) {
        sprites.emplaceBack(create_sprite(i, texture));
    }
}

UNIQUE<nc::Sprite> Tileset::create_sprite(u32 i, nc::Texture& texture) const
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

    auto sprite = MK<nc::Sprite>(&texture);
    sprite->setTexRect(MV(tex_rect));

    return sprite;
}

UNIQUE<Entity> Tileset::create_entity(const Tile& tile, Game& game, bool dynamic) const
{
    auto entity = MK<Entity>();

    u32 row = tile.id / width;
    u32 col = tile.id % width;

    entity->transform.node->setPositionX(f32(col * tile_size));
    entity->transform.node->setPositionY(f32(row * tile_size));

    nc::Recti tex_rect;
    tex_rect.x = i32(col * tile_size);
    tex_rect.y = i32(row * tile_size);
    tex_rect.w = i32(tile_size);
    tex_rect.h = i32(tile_size);

    auto gfx_def = GraphicsDef();
    gfx_def.subs.emplaceBack(SubGraphicsDef());
    gfx_def.subs[0].path = path;
    gfx_def.subs[0].rects.emplaceBack(tex_rect);
    gfx_def.subs[0].type = GraphicsType::TILE;

    entity->set_graphics(GraphicsComponent(gfx_def, game.graphics_factory));

    if (!tile.passable) {
        auto phy_def = PhysicsDef();
        phy_def.type = PhysicsType::TILE;
        phy_def.scale = 1.0f / game.config.size.tile;
        phy_def.dynamic = dynamic;
        auto physics = PhysicsComponent(phy_def, game.physics);
        physics.destructible = tile.destructible;
        entity->set_physics(MV(physics));
    }

    return entity;
}

} // namespace jmp

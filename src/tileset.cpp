#include "tileset.h"

namespace jmp
{
Tileset::Tileset(nc::Texture& texture, u32 tile_size)
    : texture {texture}
    , tile_size {tile_size}
    , width {texture.width() / tile_size}
    , height {texture.height() / tile_size}
{
    ASSERT_MSG(texture.width() % tile_size == 0, "Wrong texture and tile size");
    ASSERT_MSG(texture.height() % tile_size == 0, "Wrong texture and tile size");

    for (usize i = 0; i < width * height; ++i) {
        tiles.emplaceBack(create_tile(i));
    }
}

UNIQUE<nc::Sprite> Tileset::create_tile(u32 i) const
{
    auto tile = MK<nc::Sprite>(&texture);

    usize row = i / width;
    usize col = i % width;

    tile->x = col * tile_size;
    tile->y = row * tile_size;

    nc::Recti tex_rect;
    tex_rect.x = col * tile_size;
    tex_rect.y = row * tile_size;
    tex_rect.w = tile_size;
    tex_rect.h = tile_size;

    tile->setTexRect(MV(tex_rect));

    return tile;
}

} // namespace jmp

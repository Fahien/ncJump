#include "tilemap.h"

#include "game.h"

namespace jmp
{
Tilemap::Tilemap(Game& g)
    : width {16}
    , height {8}
{
    set_game(g);
}

u32 Tilemap::get_width() const noexcept
{
    return width;
}

u32 Tilemap::get_height() const noexcept
{
    return height;
}

void Tilemap::set_dimensions(u32 w, u32 h)
{
    auto old_width = i32(width);
    auto old_height = i32(height);

    width = w;
    height = h;

    // Create new columns with uninitialized tile entities
    tile_descs.resize(width);
    tiles.resize(width);

    for (i32 i = 0; i < old_width; ++i) {
        // Resize old columns
        tile_descs[i].resize(height);
        tiles[i].resize(height);

        // Initialize new tiles in old columns
        if (height > old_height) {
            for (i32 j = old_height; j < height; ++j) {
                if (game) {
                    set_tile({i, j}, game->tileset, tile_descs[i][j]);
                }
            }
        }
    }

    // Initialize all new tiles in new columns
    if (width > old_width) {
        for (i32 i = old_width; i < width; ++i) {
            // Resize new columns
            tile_descs[i].resize(height);
            tiles[i].resize(height);

            for (i32 j = 0; j < height; ++j) {
                if (game) {
                    set_tile({i, j}, game->tileset, tile_descs[i][j]);
                }
            }
        }
    }
}

void Tilemap::set_game(Game& g)
{
    ASSERT_MSG(!game, "Tilemap already initialized");

    game = &g;
    // Set map root as child of the game scene node
    node = MK<nc::SceneNode>(&game->scene);
    tiles_root = MK<nc::SceneNode>(node.get());
    entities_root = MK<nc::SceneNode>(node.get());

    background_texture = MK<nc::Texture>(PATH("img/tile/background.png"));
    background_texture->setMagFiltering(nc::Texture::Filtering::NEAREST);
    background_texture->setWrap(nc::Texture::Wrap::REPEAT);
    background_node = MK<nc::Sprite>(&game->scene, background_texture.get());
    background_node->setLayer(0);
    background_node->setAnchorPoint({0.0f, 0.0f});
    auto background_rect =
        nc::Recti(0, 0, background_texture->width() * width, background_texture->height());
    background_node->setTexRect(background_rect);

    // Little offset so to anchor the map to bottom-left corner of the first tile
    node->x += game->config.size.tile / 2.0f;
    node->y += game->config.size.tile / 2.0f;

    // Unless already loaded from somewhere else
    if (tile_descs.empty()) {
        // Create tile prototypes
        tile_descs.resize(width, std::vector<Tile>(height));
    }

    // Create tiles from tile prototypes
    tiles.resize(width);
    for (i32 i = 0; i < width; ++i) {
        tiles[i].resize(height);
        for (i32 j = 0; j < height; ++j) {
            set_tile({i, j}, game->tileset, tile_descs[i][j]);
        }
    }
}

UNIQUE<Entity> Tilemap::create_entity(const Vec2f& pos,
    const Tileset& tileset,
    const Tile& tile,
    const bool dynamic)
{
    auto entity = tileset.create_entity(tile, *game, dynamic);

    auto& graphics = SingleGraphicsComponent::into(*entity->get_graphics());
    entity->transform.node->x = pos.x * graphics.sprite->texRect().w;
    entity->transform.node->y = pos.y * graphics.sprite->texRect().h;
    if (auto& physics = entity->get_physics()) {
        physics->body->SetTransform({f32(pos.x), f32(pos.y)}, 0);
    }

    return entity;
}

void Tilemap::set_tile(const Vec2i& pos, const Tileset& tileset, const Tile& tile)
{
    if (pos.x < width && pos.y < height) {
        auto entity = create_entity(Vec2f(pos.x, pos.y), tileset, tile, false);
        entity->transform.node->setParent(tiles_root.get());
        tile_descs[pos.x][pos.y] = tile;
        tiles[pos.x][pos.y] = MV(entity);
    }
}

void Tilemap::set_entity(const Vec2f& pos, const Tileset& tileset, const Tile& tile)
{
    auto entity = create_entity(pos, tileset, tile, true);
    entity->transform.node->setParent(entities_root.get());
    entities.emplace_back(MV(entity));
}

} // namespace jmp

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

void Tilemap::reset()
{
    create_entities();
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
    u32 old_width = width;
    u32 old_height = height;

    width = w;
    height = h;

    // Create new columns with uninitialized tile entities
    tile_descs.resize(width);
    tiles.resize(width);

    for (u32 i = 0; i < old_width; ++i) {
        // Resize old columns
        tile_descs[i].resize(height);
        tiles[i].resize(height);

        // Initialize new tiles in old columns
        if (height > old_height) {
            for (u32 j = old_height; j < height; ++j) {
                if (game) {
                    auto index = Vec2i(i32(i), i32(j));
                    set_tile(index, game->tileset, tile_descs[i][j]);
                }
            }
        }
    }

    // Initialize all new tiles in new columns
    if (width > old_width) {
        for (u32 i = old_width; i < width; ++i) {
            // Resize new columns
            tile_descs[i].resize(height);
            tiles[i].resize(height);

            for (u32 j = 0; j < height; ++j) {
                if (game) {
                    auto index = Vec2i(i32(i), i32(j));
                    set_tile(index, game->tileset, tile_descs[i][j]);
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
    tiles.resize(usize(width));
    for (u32 i = 0; i < width; ++i) {
        tiles[i].resize(height);
        for (u32 j = 0; j < height; ++j) {
            auto index = Vec2i(i32(i), i32(j));
            set_tile(index, game->tileset, tile_descs[i][j]);
        }
    }

    // Create tiles from tile prototypes
    if (entities.empty()) {
        create_entities();
    }
}

UNIQUE<Entity> Tilemap::create_entity(const Vec2f& pos,
    const Tileset& tileset,
    const Tile& tile,
    const bool dynamic)
{
    auto entity = tileset.create_entity(tile, *game, dynamic);
    entity->set_position(pos);
    return entity;
}

void Tilemap::set_tile(const Vec2i& index, const Tileset& tileset, const Tile& tile)
{
    if (index.x < 0 || index.y < 0 || index.x >= i32(width) || index.y >= i32(height)) {
        LOGE_X("Attempting to place tile out of boundaries: (%d, %d)", index.x, index.y);
        return;
    }

    auto posf = Vec2f(f32(index.x * game->config.size.tile), f32(index.y * game->config.size.tile));
    auto entity = create_entity(posf, tileset, tile, false);
    entity->transform.node->setParent(tiles_root.get());
    tile_descs[index.x][index.y] = tile;
    tiles[index.x][index.y] = MV(entity);
}

void Tilemap::add_entity_from_tile(const Vec2f& pos, const Tileset& tileset, const Tile& tile)
{
    auto entity = create_entity(pos, tileset, tile, true);
    add_entity(MV(entity));
}

void Tilemap::add_entity(UNIQUE<Entity> entity)
{
    entity_defs.emplaceBack(entity->def);
    entity->transform.node->setParent(entities_root.get());
    entities.emplace_back(MV(entity));
}

void Tilemap::delete_entity(u32 entity_index)
{
    entity_defs.erase(nctl::begin(entity_defs) + entity_index);
    entities.erase(std::begin(entities) + entity_index);
}

void Tilemap::create_entities()
{
    // Create entities from entity definitions
    entities.resize(entity_defs.size());
    for (u32 i = 0; i < entity_defs.size(); ++i) {
        auto& def = entity_defs[i];
        entities[i] = game->entity_factory.create(def, game->graphics_factory, game->physics);
        entities[i]->transform.node->setParent(entities_root.get());
    }
}

} // namespace jmp

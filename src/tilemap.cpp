#include "tilemap.h"

#include "game.h"

namespace jmp
{
Tilemap::Tilemap(Game& g)
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

    // Create new columns with uninitialized entities
    tiles.resize(width);
    entities.resize(width);

    for (i32 i = 0; i < old_width; ++i) {
        // Resize old columns
        tiles[i].resize(height);
        entities[i].resize(height);

        // Initialize new entities in old columns
        if (height > old_height) {
            for (i32 j = old_height; j < height; ++j) {
                if (game) {
                    set({i, j}, game->tileset, tiles[i][j]);
                }
            }
        }
    }

    // Initialize all new entities in new columns
    if (width > old_width) {
        for (i32 i = old_width; i < width; ++i) {
            // Resize new columns
            tiles[i].resize(height);
            entities[i].resize(height);

            for (i32 j = 0; j < height; ++j) {
                if (game) {
                    set({i, j}, game->tileset, tiles[i][j]);
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

    // Little offset so to anchor the map to bottom-left corner of the first tile
    node->x += game->config.size.tile / 2.0f;
    node->y += game->config.size.tile / 2.0f;

    // Unless already loaded from somewhere else
    if (tiles.empty()) {
        // Create tile prototypes
        tiles.resize(width, std::vector<Tile>(height));
    }

    // Create entities from tile prototypes
    entities.resize(width);
    for (i32 i = 0; i < width; ++i) {
        entities[i].resize(height);
        for (i32 j = 0; j < height; ++j) {
            set({i, j}, game->tileset, tiles[i][j]);
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

        tiles[pos.x][pos.y] = MV(tile);
        entities[pos.x][pos.y] = MV(entity);
    }
}

} // namespace jmp

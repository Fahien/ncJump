#pragma once

#include "entity.h"
#include "tileset.h"
#include "types.h"

namespace jmp
{
class Game;

/// @brief A tilemap is a grid of cells where concrete tiles can be placed.
class Tilemap
{
public:
    static Tilemap from_json(const char* path);

    /// @brief Creates a tilemap in a uninitialized state.
    /// In order to initialize it, the method `set_game` should be called.
    Tilemap() = default;

    /// @brief Creates and initializes a tilemap
    Tilemap(Game& game);

    u32 get_width() const noexcept;
    u32 get_height() const noexcept;

    /// @brief Resize the tilemap. If new dimensions are greater than old ones, it populates new
    /// tiles and entities with default ones. If tilemap is half-initialized, entities will be half
    /// initialized as well. Calling `set_game()` will initialize them.
    void set_dimensions(u32 width, u32 height);

    void set_game(Game& game);

    /// @param index Cell index, where x is the column and y is the row
    void set_tile(const Vec2i& index, const Tileset& tileset, const Tile& tile);

    /// @param pos Position where to put the entity in scene space
    void set_entity(const Vec2f& pos, const Tileset& tileset, const Tile& tile);

    void add_entity(UNIQUE<Entity> entity);

    Game* game = nullptr;

    /// Root node of the tilemap. The whole tilemap can be transformed using this.
    UNIQUE<nc::SceneNode> node;
    UNIQUE<nc::SceneNode> tiles_root;
    UNIQUE<nc::SceneNode> entities_root;

    UNIQUE<nc::Texture> background_texture;
    UNIQUE<nc::Sprite> background_node;

    /// Grid of tiles descriptions where at each position there is a tile description used to
    /// create the concrete tile in the entities array
    std::vector<std::vector<Tile>> tile_descs;

    /// Grid of concrete tiles where each position corresponds to a cell of the grid
    std::vector<std::vector<UNIQUE<Entity>>> tiles;

    /// Other entities which position is not fixed to cells
    std::vector<UNIQUE<Entity>> entities;

private:
    UNIQUE<Entity> create_entity(const Vec2f& pos, const Tileset& ts, const Tile& t, bool dynamic);

    u32 width = 0;
    u32 height = 0;
};

} // namespace jmp

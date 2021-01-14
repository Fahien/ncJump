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

    void set(const Vec2i& pos, const Tileset& tileset, Tile tile);

    Game* game = nullptr;

    /// Root node of the tilemap. The whole tilemap can be transformed using this.
    UNIQUE<nc::SceneNode> node;

    /// Grid of tiles descriptions where at each position there is a tile description used to
    /// create the concrete tile in the entities array
    std::vector<std::vector<Tile>> tiles;

    /// Grid of concrete tiles where each position corresponds to a cell of the grid
    std::vector<std::vector<Entity>> entities;

private:
    u32 width = 16;
    u32 height = 8;

};

} // namespace jmp

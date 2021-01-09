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

    void set_game(Game& game);
    void set_tileset(const Tileset& tileset);

    void set(const Vec2i& pos, const Tileset& tileset, Tile tile);

    Game* game = nullptr;

    u32 width = 64;
    u32 height = 32;

    /// Root node of the tilemap. The whole tilemap can be transformed using this.
    UNIQUE<nc::SceneNode> node;

    /// List of tiles descriptions where at each position there is a tile description used to
    /// create the concrete tile in the entities array
    VECTOR<Tile> tiles;

    /// List of concrete tiles where each position corresponds to a cell of the grid
    VECTOR<Entity> entities;
};

} // namespace jmp

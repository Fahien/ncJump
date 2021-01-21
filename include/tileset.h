#pragma once

#include <ncine/Sprite.h>
#include <ncine/Texture.h>
#include <nctl/Array.h>

#include "entity.h"
#include "types.h"

namespace jmp
{
class Game;

/// @brief Description of a tile used to create a concrete tile.
/// A concrete tile is defined by an entity and its components.
class Tile
{
public:
    static Tile from_json(const char* file_path);

    u32 id = 0;
    bool passable = true;
    bool destructible = false;
};

/// @brief A tile set has a collection of sprites, one for each tile, used for rendering by the
/// editor, bust most importantly, it has a collection of tile descriptors which are used to create
/// entities and components to populate a tile map.
class Tileset
{
public:
    static Tileset from_json(const char* file_path);

    Tileset(u32 tile_size = 16);
    Tileset(nc::Texture& texture, u32 tile_size = 16);

    void set_texture(nc::Texture& texture);

    /// @brief The tileset is responsible for this as it knows information about the texture
    /// @return A concrete entity based on the tile description at position passed as a parameter
    UNIQUE<Entity> create_entity(const Tile& tile, Game& game, bool dynamic = false) const;

    /// @return A sprite for the tile at position passed as a parameter
    UNIQUE<nc::Sprite> create_sprite(u32 i) const;

    // @todo Do not use pointers
    nc::Texture* texture;

    u32 tile_size = 16;

    u32 width = 0;
    u32 height = 0;

    /// These are needed to be rendered in the editor
    VECTOR<UNIQUE<nc::Sprite>> sprites;

    /// Collection of tiles descriptors
    VECTOR<Tile> tiles;
};

} // namespace jmp

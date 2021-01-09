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
    bool passable = true;
};

/// @brief A tile set has a collection of sprites, one for each tile, used for rendering by the
/// editor, bust most importantly, it has a collection of tile descriptors which are used to create
/// entities and components to populate a tile map.
class Tileset
{
public:
    Tileset(nc::Texture& texture, u32 tile_size = 16);

    /// @return A concrete entity based on the tile description at position passed as a parameter
    Entity create_entity(u32 index, Game& game) const;

    /// @return A sprite for the tile at position passed as a parameter
    UNIQUE<nc::Sprite> create_sprite(u32 i) const;

    // @todo Do not use references
    nc::Texture& texture;

    u32 tile_size = 16;

    u32 width = 0;
    u32 height = 0;

    /// These are needed to be rendered in the editor
    VECTOR<UNIQUE<nc::Sprite>> sprites;

    /// Collection of tiles descriptors
    VECTOR<Tile> tiles;
};

} // namespace jmp

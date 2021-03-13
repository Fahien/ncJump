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
    /// In order to initialize it, the method `set_game()` should be called.
    Tilemap() = default;

    /// @brief Creates and initializes a tilemap
    Tilemap(Game& game);

    /// @brief Resets the state of the tilemap
    void reset();

    u32 get_width() const noexcept;
    u32 get_height() const noexcept;

    inline const VECTOR<EntityDef>& get_entity_defs() const;
    inline VECTOR<EntityDef>& get_entity_defs_mut();

    inline std::vector<UNIQUE<Entity>>& get_entities();

    /// @brief Resizes the tilemap. If new dimensions are greater than old ones, it populates new
    /// tiles and entities with default ones. If tilemap is half-initialized, entities will be half
    /// initialized as well. Calling `set_game()` will initialize them.
    void set_dimensions(u32 width, u32 height);

    /// @brief. At the moment it sets the tilemap's root node and background node as a child of the
    /// game scene node. Sets an offset according to the configured tile size, and creates tile from
    /// their definitions using the game's tileset.
    /// @todo This function can be improved to be more specific
    void set_game(Game& game);

    /// @param index Cell index, where x is the column and y is the row
    void set_tile(const Vec2i& index, const Tileset& tileset, const Tile& tile);

    /// @param pos Position where to put the entity in scene space
    /// @param tileset Tileset to use for the graphics of the tile
    /// @param tile Tile description to use when creating the new entity
    void add_entity_from_tile(const Vec2f& pos, const Tileset& tileset, const Tile& tile);

    void add_entity(UNIQUE<Entity> entity);

    /// @param entity_index Vector position of the entity to remove
    void delete_entity(u32 entity_index);

    Game* game = nullptr;

    Vec2f initial_position = Vec2f(0.0f, 0.0f);

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

private:
    UNIQUE<Entity> create_entity(const Vec2f& pos, const Tileset& ts, const Tile& t, bool dynamic);

    /// @brief Create entities from entity definitions
    void create_entities();

    u32 width = 0;
    u32 height = 0;

    /// List of entity definitions used to populate the entities vector
    VECTOR<EntityDef> entity_defs;

    /// Other entities which position is not fixed to cells
    std::vector<UNIQUE<Entity>> entities;
};

inline const VECTOR<EntityDef>& Tilemap::get_entity_defs() const
{
    return entity_defs;
}

inline VECTOR<EntityDef>& Tilemap::get_entity_defs_mut()
{
    return entity_defs;
}

inline std::vector<UNIQUE<Entity>>& Tilemap::get_entities()
{
    return entities;
}

} // namespace jmp

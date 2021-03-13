#pragma once

#include "types.h"

namespace ncine
{
class Texture;
}

namespace jmp
{
enum class EntityType { TILE, PLAYER, ENEMY };

enum class GraphicsType {
    TILE,
    ANIM,
};

enum class PhysicsType {
    TILE,
    CHAR,
};

enum class ScriptType { WANDERING };

/// @return A vector of rects for a `nc::RectAnimation` from a Texture stripe
VECTOR<Recti> rects_from_stripe(const nc::Texture& texture);

/// @brief Sub-graphics definition, probably associated to a state of the entity
struct SubGraphicsDef {
    /// @return A sub-graphics definition with a placeholder graphics
    static SubGraphicsDef dummy();

    GraphicsType type = GraphicsType::TILE;
    String path = "img/tile/placeholder.png";
    VECTOR<Recti> rects = {};
    u32 layer = 0;
    bool loop = true;
};

/// @brief Definition of a graphics component
struct GraphicsDef {
    /// A graphics component may have multiple sub-graphics
    /// each one related to a different entity state.
    VECTOR<SubGraphicsDef> subs = {};
};

/// @brief Definition of a physics component
struct PhysicsDef {
    PhysicsType type = PhysicsType::TILE;

    /// The physics body may use a different scale for its calculations
    float scale = 1.0f;
    bool dynamic = false;
};

/// @brief Definition of a state component
struct StateDef {
    bool dummy;
};

/// @brief Definition of a script component
struct ScriptDef {
    inline ScriptDef(ScriptType type = ScriptType::WANDERING);

    ScriptType type = ScriptType::WANDERING;
};

inline ScriptDef::ScriptDef(ScriptType type)
    : type {type}
{
}

/// @brief An Entity definitions describes everything needed to create a concrete entity.
/// This is also useful for serialization as it would be easier than trying to serialize
/// an entity with all its components.
struct EntityDef {
    /// @return A definition for a dummy entity
    static EntityDef dummy();

    /// @return An entity definition loaded from a json file, or None if there is no such file
    static OPTION<EntityDef> from_json(const char* path);

    EntityDef(EntityType type = EntityType::TILE);

    EntityType type = EntityType::TILE;

    /// Position of the entity
    Vec2f pos = {0.0f, 0.0f};

    GraphicsDef graphics = {};
    OPTION<PhysicsDef> physics = {};
    OPTION<StateDef> state = {};
    VECTOR<ScriptDef> scripts = {};
};

} // namespace jmp

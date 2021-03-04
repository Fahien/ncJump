#pragma once

#include "types.h"

namespace ncine
{
class Texture;
}

namespace jmp
{
enum class EntityType { NONE = 0, TILE, PLAYER, ENEMY };

enum class GraphicsType {
    TILE,
    ANIM,
};

/// @return A default vector of rects for a `nc::RectAnimation`
inline VECTOR<Recti> default_rects()
{
    auto ret = VECTOR<Recti>();
    ret.pushBack(Recti(0, 0, 16, 16));
    return ret;
}

/// @return A vector of rects for a `nc::RectAnimation` from a Texture stripe
VECTOR<Recti> rects_from_stripe(const nc::Texture& texture);

/// @brief Sub-graphics definition, probably associated to a state of the entity
struct SubGraphicsDef {
    GraphicsType type = GraphicsType::TILE;
    String path = "img/tile/placeholder.png";
    VECTOR<Recti> rects = default_rects();
    bool loop = true;
};

inline VECTOR<SubGraphicsDef> default_subgraphics()
{
    auto ret = VECTOR<SubGraphicsDef>();
    ret.pushBack(SubGraphicsDef());
    return ret;
}

/// @brief Definition of a graphics component
struct GraphicsDef {
    /// A graphics component may have multiple sub-graphics
    /// each one related to a different entity state.
    VECTOR<SubGraphicsDef> subs = default_subgraphics();
};

/// @brief An Entity definitions describes everything needed to create a concrete entity.
/// This is also useful for serialization as it would be easier than trying to serialize
/// an entity with all its components.
struct EntityDef {
    /// Position of the entity
    Vec2f pos = {0.0f, 0.0f};

    GraphicsDef graphics = {};
};

} // namespace jmp

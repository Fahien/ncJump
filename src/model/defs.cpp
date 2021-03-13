#include "model/defs.h"

#include <ncine/Texture.h>

namespace jmp
{
VECTOR<Recti> rects_from_stripe(const nc::Texture& texture)
{
    auto rects = VECTOR<Recti>();

    ASSERT(texture.width() >= texture.height());
    i32 count = texture.width() / texture.height();

    auto sprite_size = Vec2i(texture.width() / count, texture.height());
    for (i32 i = 0; i < count; ++i) {
        rects.pushBack(nc::Recti(i * sprite_size.x, 0, sprite_size.x, sprite_size.y));
    }

    return rects;
}

/// @return A default vector of rects for a `nc::RectAnimation`
inline VECTOR<Recti> default_rects()
{
    auto ret = VECTOR<Recti>();
    ret.pushBack(Recti(0, 0, 16, 16));
    return ret;
}

SubGraphicsDef SubGraphicsDef::dummy()
{
    SubGraphicsDef def = {};
    def.rects = default_rects();
    def.layer = 2;
    return def;
}

EntityDef EntityDef::dummy()
{
    auto ret = EntityDef(EntityType::TILE);
    ret.graphics.subs.emplaceBack(SubGraphicsDef::dummy());
    return ret;
}

EntityDef::EntityDef(EntityType type)
    : type {type}
{
}

} // namespace jmp

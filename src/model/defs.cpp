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

EntityDef::EntityDef(EntityType type)
: type {type}
{
}

} // namespace jmp

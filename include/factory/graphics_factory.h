#pragma once

#include "model/defs.h"
#include "types.h"

#include <ncine/AnimatedSprite.h>

namespace ncine
{
class Texture;
}

namespace jmp
{
class SubGraphics;
class GraphicsComponent;

class GraphicsFactory
{
public:
    GraphicsFactory() = default;

    /// @return A cached texture, creating it if not already in the cache
    nc::Texture& get_or_create(const String& path);

    UNIQUE<SubGraphics> create(const SubGraphicsDef& def);

    nc::AnimatedSprite create_animation(const nctl::String& path,
        nc::RectAnimation::LoopMode loop = nc::RectAnimation::LoopMode::ENABLED);

    nc::AnimatedSprite create_anim(const SubGraphicsDef& def);
private:
    HASHMAP<String, UNIQUE<nc::Texture>> textures = HASHMAP<String, UNIQUE<nc::Texture>>(128);
};

} // namespace jmp

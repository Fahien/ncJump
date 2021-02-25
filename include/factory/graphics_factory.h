#pragma once

#include "types.h"

#include <ncine/AnimatedSprite.h>

namespace ncine
{
class Texture;
}

namespace jmp
{
class GraphicsFactory
{
public:
    nc::AnimatedSprite create_animation(const nctl::String& path,
        nc::RectAnimation::LoopMode loop = nc::RectAnimation::LoopMode::ENABLED);

private:
    VECTOR<UNIQUE<nc::Texture>> textures;
};

} // namespace jmp

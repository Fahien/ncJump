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
    nc::AnimatedSprite create_animation(const nctl::String& path);

private:
    VECTOR<UNIQUE<nc::Texture>> textures;
};

} // namespace jmp

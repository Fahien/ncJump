#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

namespace jmp
{
class Entity
{
public:
    Entity();

private:
    ncine::Texture texture;
    ncine::AnimatedSprite sprite;
};

} // namespace jmp

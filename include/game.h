#pragma once

#include <ncine/SceneNode.h>
#include <ncine/Sprite.h>
#include <ncine/Texture.h>

namespace jmp
{
class Game
{
public:
    Game();

private:
    ncine::SceneNode root;
    ncine::Texture texture;
    ncine::Sprite sprite;
};

} // namespace jmp

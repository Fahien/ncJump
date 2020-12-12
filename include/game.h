#pragma once

#include <ncine/SceneNode.h>

#include "entity.h"

namespace jmp
{
struct Input {
    Vec2f move = {0.0, 0.0};
};

class Game
{
public:
    Game();

    void update(float dt);

    Input input;

    nc::SceneNode& root;
    Entity entity;
};

} // namespace jmp

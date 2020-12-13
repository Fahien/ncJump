#pragma once

#include "entity.h"
#include <ncine/SceneNode.h>

namespace jmp
{
class Game
{
public:
    Game();

    void update(float dt);

private:
    nc::SceneNode &root;
    Entity entity;
};

} // namespace jmp

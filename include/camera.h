#pragma once

#include "types.h"

namespace ncine
{
class SceneNode;
}

namespace jmp
{
class Game;

class Camera
{
public:
    Camera(Game& game, nc::SceneNode& follow);

    void update();

    Vec2f get_position() const;

    Vec2f offset = {};

private:
    Game* game = nullptr;
    nc::SceneNode& node;
    nc::SceneNode& follow;
};

} // namespace jmp

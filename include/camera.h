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
    Camera(Game& game, nc::SceneNode* follow = nullptr);

    void update();

    Vec2f get_position() const;

    Vec2f offset = {};

    void set_follow(nc::SceneNode* follow);

private:
    Game* game = nullptr;

    /// Node to move to the opposite direction of the camera
    nc::SceneNode& node;

    /// Node the camera needs to follow
    nc::SceneNode* follow;
};

} // namespace jmp

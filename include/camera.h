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

    /// @return Basically an inverted position of the scene
    Vec2f get_position() const;

    /// @param follow Sets the node to follow or stops following if `nullptr` is passed
    void set_follow(nc::SceneNode* follow);

    Vec2f offset = {};

    Vec2f target = {};

private:
    Game* game = nullptr;

    /// Node to move to the opposite direction of the camera
    nc::SceneNode& node;

    /// Node the camera needs to follow
    nc::SceneNode* follow;
};

} // namespace jmp

#pragma once

#include <ncine/Camera.h>

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

    ~Camera();

    void update();

    /// @return The current camera position
    Vec2f get_position() const;

    /// @param follow Sets the node to follow or stops following if `nullptr` is passed
    void set_follow(nc::SceneNode* follow);

    Vec2f offset = {};

    Vec2f target = {};

private:
    Game* game = nullptr;

    /// The nCine camera object
    nc::Camera camera;

    /// Node the camera needs to follow
    nc::SceneNode* follow;
};

} // namespace jmp

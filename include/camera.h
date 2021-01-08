#pragma once

#include "types.h"

namespace ncine
{
class SceneNode;
}

namespace jmp
{

class Camera
{
public:
    Camera(nc::SceneNode& node, nc::SceneNode& follow);

    void update();

    Vec2f get_position() const;

private:
    nc::SceneNode& node;
    nc::SceneNode& follow;
};

} // namespace jmp

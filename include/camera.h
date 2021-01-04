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

private:
    nc::SceneNode& node;
    nc::SceneNode& follow;
};

} // namespace jmp

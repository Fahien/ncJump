#pragma once

#include <ncine/SceneNode.h>

#include "types.h"

namespace jmp
{
class TransformComponent {
public:
    TransformComponent(nc::SceneNode& parent);

    // Node representing this transform
    nc::SceneNode node;
};

} // namespace jmp

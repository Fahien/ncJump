#pragma once

#include <ncine/SceneNode.h>

#include "types.h"

namespace jmp
{
class TransformComponent {
public:
    TransformComponent() = default;

    TransformComponent(nc::SceneNode& parent);

    // nCine relies on pointers therefore this must live on the heap for the moment
    UNIQUE<nc::SceneNode> node = MK<nc::SceneNode>();
};

} // namespace jmp

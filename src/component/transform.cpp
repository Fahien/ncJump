#include "component/transform.h"

namespace jmp
{
TransformComponent::TransformComponent(nc::SceneNode& parent)
    : node {MK<nc::SceneNode>(&parent)}
{
    node->x += 4;
    node->y += 2;
}

TransformComponent TransformComponent::clone()
{
    auto ret = TransformComponent();
    *ret.node = node->clone();
    return ret;
}

} // namespace jmp

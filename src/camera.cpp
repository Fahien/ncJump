#include "camera.h"

#include <ncine/SceneNode.h>

#include "config.h"

namespace jmp
{
Camera::Camera(nc::SceneNode& node, nc::SceneNode& follow)
    : node {node}
    , follow {follow}
{
}

Vec2f Camera::get_position() const {
    return node.position();
}

f32 lerp(f32 a, f32 b, f32 t)
{
    return a + t * (b - a);
}

void Camera::update()
{
    const f32 smooth_factor = 0.125f;
    f32 target_x = -follow.x * node.scale().x + 200.0f;
    f32 target_y = -follow.y * node.scale().y + 64.0f;
    f32 smoothed_x = lerp(node.x, target_x, smooth_factor);
    f32 smoothed_y = lerp(node.y, target_y, smooth_factor);
    node.x = smoothed_x;
    node.y = smoothed_y;
}

} // namespace jmp

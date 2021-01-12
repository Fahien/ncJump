#include "camera.h"

#include <ncine/SceneNode.h>

#include "game.h"

namespace jmp
{
Camera::Camera(Game& game, nc::SceneNode& follow)
    : game {&game}
    , node {game.scene}
    , follow {follow}
{
}

Vec2f Camera::get_position() const
{
    return node.position();
}

f32 lerp(f32 a, f32 b, f32 t)
{
    return a + t * (b - a);
}

void Camera::update()
{
    const f32 smooth_factor = 0.125f;
    f32 target_x = -follow.x * node.scale().x + game->config.size.window.width / 2.0f;
    f32 target_y = -follow.y * node.scale().y + game->config.size.window.height / 2.0f;
    f32 smoothed_x = lerp(node.x, target_x, smooth_factor);
    f32 smoothed_y = lerp(node.y, target_y, smooth_factor);

    // Clamp to tilemap borders
    // As we are moving the scene to the opposite direction of the camera max and min are "inverted"
    f32 max_x = 0.0f;
    f32 max_y = 0.0f;
    f32 min_x = -(game->tilemap.width * game->config.size.tile * game->config.scale.scene -
        game->config.size.window.width);
    f32 min_y = -(game->tilemap.height * game->config.size.tile * game->config.scale.scene -
        game->config.size.window.height);
    node.x = std::clamp(smoothed_x, min_x, max_x);
    node.y = std::clamp(smoothed_y, min_y, max_y);
}

} // namespace jmp

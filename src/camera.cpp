#include <algorithm>

#include "camera.h"

#include <ncine/Application.h>
#include <ncine/SceneNode.h>
#include <ncine/Viewport.h>

#include "game.h"

namespace jmp
{
Camera::Camera(Game& game, nc::SceneNode* follow)
    : game {&game}
    , follow {follow}
{
    ncine::theApplication().screenViewport().setCamera(&camera);
}

Camera::~Camera()
{
    ncine::theApplication().screenViewport().setCamera(nullptr);
}

Vec2f Camera::get_position() const
{
    const auto viewValues = camera.viewValues();
    // Need to scale cause the scene node position is already scaled. Do not know why.
    return Vec2f(viewValues.position.x / game->config.scale.scene, viewValues.position.y / game->config.scale.scene);
}

void Camera::set_follow(nc::SceneNode* f)
{
    follow = f;
}

f32 lerp(f32 a, f32 b, f32 t)
{
    return a + t * (b - a);
}

void Camera::update()
{
    if (!follow) {
        return;
    }

    const f32 smooth_factor = 0.125f;

    // Window in game space
    auto window = game->config.get_real_window_size();

    // Target needs to be in game space as well
    auto hard_target = Vec2f(follow->position().x * game->config.scale.scene - window.width / 2.0f,
        follow->position().y * game->config.scale.scene - window.height / 2.0f);
    hard_target -= offset;

     auto viewValues = camera.viewValues();
    // The real target is smoothed
    target.x = lerp(viewValues.position.x, hard_target.x, smooth_factor);
    target.y = lerp(viewValues.position.y, hard_target.y, smooth_factor);

    // Clamp to tilemap borders
    Vec2f max = {0.0f, 0.0f};
    Vec2f min = {0.0f, 0.0f};

    max.x += game->tilemap.get_width() * game->config.size.tile * game->config.scale.scene +
        window.width;
    max.y += game->tilemap.get_height() * game->config.size.tile * game->config.scale.scene +
        window.height;

    // Make sure min values are less than max values
    min.x = std::min(min.x, max.x);
    min.y = std::min(min.y, max.y);

    viewValues.position.x = std::clamp(target.x, min.x, max.x);
    viewValues.position.y = std::clamp(target.y, min.y, max.y);
    camera.setView(viewValues);
}

} // namespace jmp

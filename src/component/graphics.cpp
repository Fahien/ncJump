#include "component/graphics.h"

#include <ncine/FileSystem.h>

#include "component/transform.h"
#include "input.h"

namespace jmp
{
SingleGraphicsComponent& SingleGraphicsComponent::into(GraphicsComponent& g)
{
    return reinterpret_cast<SingleGraphicsComponent&>(g);
}

SingleGraphicsComponent::SingleGraphicsComponent(TransformComponent& transform, nc::Texture& texture)
    : sprite {MK<nc::Sprite>(&*transform.node, &texture)}
{
}

void SingleGraphicsComponent::update(const Input& input)
{
}

CharacterGraphicsComponent::CharacterGraphicsComponent(TransformComponent& transform)
    : idle_texture {PATH("img/hero/herochar_idle_anim_strip_4.png")}
    , idle {&*transform.node, &idle_texture}
    , movement_texture {PATH("img/hero/herochar_run_anim_strip_6.png")}
    , movement {nullptr, &movement_texture}
    , jump_up_texture {PATH("img/hero/herochar_jump_up_anim_strip_3.png")}
    , jump_up {nullptr, &jump_up_texture}
    , jump_down_texture {PATH("img/hero/herochar_jump_down_anim_strip_3.png")}
    , jump_down {nullptr, &jump_down_texture}
{
    // Idle animation
    {
        idle_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(idle_texture.width() / 4.0f, idle_texture.height());

        for (int i = 0; i < 4; ++i) {
            anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        idle.addAnimation(MK<nc::RectAnimation>(anim));
        idle.setPaused(false);
        idle.setLayer(1);
    }

    // Movement animation
    {
        movement_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(movement_texture.width() / 6.0f, movement_texture.height());

        for (int i = 0; i < 6; ++i) {
            anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        movement.addAnimation(MK<nc::RectAnimation>(anim));
        movement.setPaused(false);
        movement.setLayer(1);
    }

    // Jump animation
    {
        jump_up_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto jump_up_anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(jump_up_texture.width() / 3.0f, jump_up_texture.height());

        for (int i = 0; i < 3; ++i) {
            jump_up_anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        jump_up.addAnimation(MK<nc::RectAnimation>(jump_up_anim));
        jump_up.setPaused(false);
        jump_up.setLayer(1);

        jump_down_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto jump_down_anim = nc::RectAnimation(0.125f,
            nc::RectAnimation::LoopMode::ENABLED,
            nc::RectAnimation::RewindMode::FROM_START);

        for (int i = 0; i < 3; ++i) {
            jump_down_anim.addRect(nc::Recti(i * sprite_size.x, 0.0, sprite_size.x, sprite_size.y));
        }

        jump_down.addAnimation(MK<nc::RectAnimation>(jump_down_anim));
        jump_down.setPaused(false);
        jump_down.setLayer(1);
    }
}

void CharacterGraphicsComponent::update(const Input& input)
{
    // Set direction of the sprite
    float movement_x = input.joystick.move.x;
    if (movement_x) {
        bool flipped_x = movement_x < 0.0;
        movement.setFlippedX(flipped_x);
        idle.setFlippedX(flipped_x);
        jump_up.setFlippedX(flipped_x);
        jump_down.setFlippedX(flipped_x);
    }
}

} // namespace jmp

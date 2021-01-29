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

SingleGraphicsComponent::SingleGraphicsComponent(TransformComponent& transform,
    nc::Texture& texture)
    : sprite {MK<nc::Sprite>(&*transform.node, &texture)}
{
    sprite->setLayer(1);
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
    , push_texture {PATH("img/hero/herochar_pushing_foward_anim_strip_6.png")}
    , push {nullptr, &push_texture}
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

        idle.addAnimation(MV(anim));
        idle.setPaused(false);
        idle.setLayer(2);
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

        movement.addAnimation(MV(anim));
        movement.setPaused(false);
        movement.setLayer(2);
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

        jump_up.addAnimation(MV(jump_up_anim));
        jump_up.setPaused(false);
        jump_up.setLayer(2);

        jump_down_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto jump_down_anim = nc::RectAnimation(0.125f,
            nc::RectAnimation::LoopMode::ENABLED,
            nc::RectAnimation::RewindMode::FROM_START);

        for (int i = 0; i < 3; ++i) {
            jump_down_anim.addRect(nc::Recti(i * sprite_size.x, 0.0, sprite_size.x, sprite_size.y));
        }

        jump_down.addAnimation(MV(jump_down_anim));
        jump_down.setPaused(false);
        jump_down.setLayer(2);
    }

    // Push animation
    {
        push_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto push_anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(push_texture.width() / 6.0f, push_texture.height());

        for (int i = 0; i < 6; ++i) {
            push_anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        push.addAnimation(MV(push_anim));
        push.setPaused(false);
        push.setLayer(2);
    }
}

void CharacterGraphicsComponent::update(const Input& input)
{
    // Set direction of the sprite
    f32 movement_x = input.joystick.move.x;
    if (movement_x) {
        bool flipped_x = movement_x < 0.0;
        direction = flipped_x ? Direction::LEFT : Direction::RIGHT;
        movement.setFlippedX(flipped_x);
        idle.setFlippedX(flipped_x);
        jump_up.setFlippedX(flipped_x);
        jump_down.setFlippedX(flipped_x);
        push.setFlippedX(flipped_x);
    }
}

} // namespace jmp

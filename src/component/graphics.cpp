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

CharacterGraphicsComponent& CharacterGraphicsComponent::into(GraphicsComponent& g)
{
    return reinterpret_cast<CharacterGraphicsComponent&>(g);
}

#define INIT_ANIMATION(texture, sprite, count)                                                     \
    {                                                                                              \
        texture.setMagFiltering(nc::Texture::Filtering::NEAREST);                                  \
        auto anim = nc::RectAnimation(0.125,                                                       \
            nc::RectAnimation::LoopMode::ENABLED,                                                  \
            nc::RectAnimation::RewindMode::FROM_START);                                            \
        auto sprite_size = Vec2f(texture.width() / float(count), texture.height());                \
        for (int i = 0; i < count; ++i) {                                                          \
            anim.addRect(nc::Recti(i* sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));         \
        }                                                                                          \
        sprite.addAnimation(MV(anim));                                                             \
        sprite.setPaused(false);                                                                   \
        sprite.setLayer(2);                                                                        \
    }

CharacterGraphicsComponent::CharacterGraphicsComponent(TransformComponent& transform,
    nc::Texture idlet)
    : idle_texture {MV(idlet)}
    , idle {&*transform.node, &idle_texture}
    , movement_texture {PATH("img/hero/herochar_run_anim_strip_6.png")}
    , movement {nullptr, &movement_texture}
    , jump_up_texture {PATH("img/hero/herochar_jump_up_anim_strip_3.png")}
    , jump_up {nullptr, &jump_up_texture}
    , jump_down_texture {PATH("img/hero/herochar_jump_down_anim_strip_3.png")}
    , jump_down {nullptr, &jump_down_texture}
    , push_texture {PATH("img/hero/herochar_pushing_foward_anim_strip_6.png")}
    , push {nullptr, &push_texture}
    , pull_texture {PATH("img/hero/herochar_pushing_foward_anim_strip_6.png")}
    , pull {nullptr, &pull_texture}
{
    INIT_ANIMATION(idle_texture, idle, idle_texture.width() / idle_texture.height());
    INIT_ANIMATION(movement_texture, movement, 6);
    INIT_ANIMATION(jump_up_texture, jump_up, 3);
    INIT_ANIMATION(jump_down_texture, jump_down, 3);
    INIT_ANIMATION(push_texture, push, 6);
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

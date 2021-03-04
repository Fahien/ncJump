#include "component/graphics.h"

#include <ncine/FileSystem.h>

#include "component/transform.h"
#include "config.h"
#include "entity.h"
#include "input.h"

namespace jmp
{
nc::Sprite clone_sprite(const nc::Sprite& sprite)
{
    auto ret = sprite.clone();
    ret.setLayer(sprite.layer());
    return ret;
}

/// @todo Workaround as `AnimatedSprite::clone()` is currently bugged
nc::AnimatedSprite clone_sprite(const nc::AnimatedSprite& sprite)
{
    auto ret = sprite.clone();
    ret.setFrame(0);
    ret.setLayer(sprite.layer());
    return ret;
}

SingleGraphicsComponent& SingleGraphicsComponent::into(GraphicsComponent& g)
{
    return reinterpret_cast<SingleGraphicsComponent&>(g);
}

SingleGraphicsComponent::SingleGraphicsComponent(TransformComponent& transform,
    nc::Texture& texture)
    : sprite {&*transform.node, &texture}
{
    sprite.setLayer(1);
}

UNIQUE<GraphicsComponent> SingleGraphicsComponent::clone() const
{
    auto ret = MK<SingleGraphicsComponent>();
    ret->sprite = clone_sprite(sprite);
    return ret;
}

void SingleGraphicsComponent::set(Entity& entity)
{
    sprite.setParent(entity.transform.node.get());
}

GuiTexture SingleGraphicsComponent::get_guitex(const Config& config) const
{
    GuiTexture tex;

    auto texture = const_cast<nc::Texture*>(sprite.texture());
    tex.id = texture->guiTexId();

    auto rect = sprite.texRect();
    tex.size = ImVec2(rect.w * config.scale.gui, rect.h * config.scale.gui);

    f32 tw = texture->width();
    f32 th = texture->height();
    tex.uvs[0] = ImVec2(rect.x / tw, rect.y / th);
    tex.uvs[1] = ImVec2((rect.x + rect.w) / tw, (rect.y + rect.h) / th);

    return tex;
}

CharacterGraphicsComponent& CharacterGraphicsComponent::into(GraphicsComponent& g)
{
    return reinterpret_cast<CharacterGraphicsComponent&>(g);
}

UNIQUE<GraphicsComponent> CharacterGraphicsComponent::clone() const
{
    auto ret = MK<CharacterGraphicsComponent>();

    ret->direction = direction;
    ret->idle = clone_sprite(idle);
    ret->movement = clone_sprite(movement);
    ret->jump_up = clone_sprite(jump_up);
    ret->jump_down = clone_sprite(jump_down);
    ret->push = clone_sprite(push);
    ret->pull = clone_sprite(pull);
    ret->dying = clone_sprite(dying);

    return ret;
}

void CharacterGraphicsComponent::set(Entity& entity)
{
    idle.setParent(entity.transform.node.get());
}

GuiTexture CharacterGraphicsComponent::get_guitex(const Config& config) const
{
    GuiTexture tex;

    auto texture = const_cast<nc::Texture*>(idle.texture());
    tex.id = texture->guiTexId();

    auto& frame = idle.animations()[idle.animationIndex()];
    auto& rect = frame.rect();
    tex.size = ImVec2(rect.w * config.scale.gui, rect.h * config.scale.gui);

    f32 tw = texture->width();
    f32 th = texture->height();
    tex.uvs[0] = ImVec2(rect.x / tw, rect.y / th);
    tex.uvs[1] = ImVec2((rect.x + rect.w) / tw, (rect.y + rect.h) / th);

    return tex;
}

void CharacterGraphicsComponent::update(const PhysicsComponent& physics, const Input* input)
{
    // Set direction of the sprite
    f32 movement_x = input ? input->joystick.move.x : physics.body->GetLinearVelocity().x;

    if (!closef(movement_x, 0.0f)) {
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

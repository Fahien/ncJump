#include "component/graphics.h"

#include <ncine/FileSystem.h>

#include "component/transform.h"
#include "entity.h"
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

void SingleGraphicsComponent::set(Entity& entity)
{
    sprite->setParent(entity.transform.node.get());
}

CharacterGraphicsComponent& CharacterGraphicsComponent::into(GraphicsComponent& g)
{
    return reinterpret_cast<CharacterGraphicsComponent&>(g);
}

/// @todo Workaround as `AnimatedSprite::clone()` is currently bugged
nc::AnimatedSprite clone_sprite(const nc::AnimatedSprite& sprite)
{
    auto ret = sprite.clone();
    ret.setFrame(0);
    ret.setLayer(sprite.layer());
    return ret;
}

UNIQUE<GraphicsComponent> CharacterGraphicsComponent::clone()
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

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

UNIQUE<GraphicsComponent> CharacterGraphicsComponent::clone()
{
    auto ret = UNIQUE<CharacterGraphicsComponent>(new CharacterGraphicsComponent());

    ret->direction = direction;
    ret->idle = idle.clone();
    ret->idle.setFrame(0);
    ret->movement = movement.clone();
    ret->jump_up = jump_up.clone();
    ret->jump_down = jump_down.clone();
    ret->push = push.clone();
    ret->pull = pull.clone();
    ret->dying = dying.clone();

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

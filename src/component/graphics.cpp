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

void SingleGraphicsComponent::update(const Input& input)
{
}

CharacterGraphicsComponent& CharacterGraphicsComponent::into(GraphicsComponent& g)
{
    return reinterpret_cast<CharacterGraphicsComponent&>(g);
}

void CharacterGraphicsComponent::set(Entity& entity)
{
    idle.setParent(entity.transform.node.get());
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

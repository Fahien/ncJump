#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "types.h"

namespace jmp
{
struct Input;
struct TransformComponent;
class Entity;

struct GraphicsComponent {
    virtual ~GraphicsComponent() = default;

    virtual void set(Entity& entity) = 0;

    virtual void update(const Input& input) = 0;

    inline virtual Direction::Value get_direction() const;
};

inline Direction::Value GraphicsComponent::get_direction() const
{
    return Direction::NONE;
}

// Graphics component with a single static image which never changes
struct SingleGraphicsComponent : public GraphicsComponent {
    static SingleGraphicsComponent& into(GraphicsComponent& g);

    SingleGraphicsComponent(TransformComponent& t, nc::Texture& texture);

    void set(Entity& entity) override;

    void update(const Input& input) override;

    // nCine works with pointers
    UNIQUE<nc::Sprite> sprite;
};

struct CharacterGraphicsComponent : public GraphicsComponent {
    static CharacterGraphicsComponent& into(GraphicsComponent& g);

    CharacterGraphicsComponent() = default;

    void set(Entity& entity) override;

    void update(const Input& input) override;

    inline Direction::Value get_direction() const override;

    Direction::Value direction = Direction::RIGHT;

    nc::AnimatedSprite idle;
    nc::AnimatedSprite movement;
    nc::AnimatedSprite jump_up;
    nc::AnimatedSprite jump_down;
    nc::AnimatedSprite push;
    nc::AnimatedSprite pull;
};

inline Direction::Value CharacterGraphicsComponent::get_direction() const
{
    return direction;
}

} // namespace jmp

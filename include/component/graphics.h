#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "types.h"

namespace jmp
{
struct Input;
struct TransformComponent;

struct GraphicsComponent {
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

    void update(const Input& input) override;

    // nCine works with pointers
    UNIQUE<nc::Sprite> sprite;
};

struct CharacterGraphicsComponent : public GraphicsComponent {
    CharacterGraphicsComponent(TransformComponent& t);

    void update(const Input& input) override;

    inline Direction::Value get_direction() const override;

    Direction::Value direction = Direction::RIGHT;

    nc::Texture idle_texture;
    nc::AnimatedSprite idle;

    nc::Texture movement_texture;
    nc::AnimatedSprite movement;

    nc::Texture jump_up_texture;
    nc::AnimatedSprite jump_up;

    nc::Texture jump_down_texture;
    nc::AnimatedSprite jump_down;

    nc::Texture push_texture;
    nc::AnimatedSprite push;
};

inline Direction::Value CharacterGraphicsComponent::get_direction() const
{
    return direction;
}

} // namespace jmp

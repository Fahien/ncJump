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
};

// Graphics component with a single static image which never changes
struct SingleGraphicsComponent : public GraphicsComponent {
    void update(const Input& input) override;

    nc::Texture idle_texture;
    nc::Sprite idle;
};

struct CharacterGraphicsComponent : public GraphicsComponent {
    CharacterGraphicsComponent(TransformComponent& t);

    void update(const Input& input) override;

    nc::Texture idle_texture;
    nc::AnimatedSprite idle;

    nc::Texture movement_texture;
    nc::AnimatedSprite movement;

    nc::Texture jump_up_texture;
    nc::AnimatedSprite jump_up;

    nc::Texture jump_down_texture;
    nc::AnimatedSprite jump_down;
};

} // namespace jmp

#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "types.h"

namespace jmp
{
struct Input;
class TransformComponent;

class GraphicsComponent {
public:
    virtual void update(const Input& input) = 0;
};

// Graphics component with a single static image which never changes
class SingleGraphicsComponent : public GraphicsComponent {
public:
    static SingleGraphicsComponent& into(GraphicsComponent& g);

    SingleGraphicsComponent(TransformComponent& t, nc::Texture& texture);

    void update(const Input& input) override;

    // nCine works with pointers
    UNIQUE<nc::Sprite> sprite;
};

class CharacterGraphicsComponent : public GraphicsComponent {
public:
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

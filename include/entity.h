#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "types.h"

namespace jmp
{
class Entity
{
public:
    enum class State {
        IDLE = 0,
        MOVE,
    };
    Entity();

    State getState() const noexcept
    {
        return state;
    }
    void setState(State state);

    void move(const Vec2f& mv);

private:
    State state = State::IDLE;

    // Node of this character
    // Animations are children of this node
    nc::SceneNode node;

    // Graphics resources
    nc::Texture idle_texture;
    nc::AnimatedSprite idle;

    nc::Texture movement_texture;
    nc::AnimatedSprite movement;

    nc::Texture jump_up_texture;
    nc::Texture jump_down_texture;
    nc::AnimatedSprite jump_up;
    nc::AnimatedSprite jump_down;
};

const char* to_string(Entity::State state);

} // namespace jmp

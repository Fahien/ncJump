#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

namespace nc = ncine;

namespace jmp
{
class Entity
{
public:
    enum class State {
        IDLE = 0,
    };
    Entity();

    State getState() const noexcept
    {
        return state;
    }

private:
    State state = State::IDLE;

    // Graphics resources
    nc::Texture texture;
    nc::AnimatedSprite idle;
};

const char* to_string(Entity::State state);

} // namespace jmp

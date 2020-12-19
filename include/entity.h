#pragma once

#include <Box2D/Box2D.h>
#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "types.h"

namespace jmp
{
class Input;
class Button;

class Entity
{
public:
    enum class State {
        IDLE = 0,
        MOVE,
        JUMP_UP,
        JUMP_DOWN
    };
    Entity(nc::SceneNode& scene);

    State getState() const noexcept
    {
        return state;
    }
    void setState(State state);

    void move(const Vec2f& mv);
    void jump(const Button& button);
    void update(const Input& input);

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

    // Physics body
    b2Body* body = nullptr;

    f32 air_factor = 1.0f / 16.0f;
    f32 velocity_factor = 6.0f;
    f32 jump_y_factor = 32.0f;
    f32 jump_x_factor = 1.0f;
    f32 max_x_speed = 32.0f;
};

const char* to_string(Entity::State state);

} // namespace jmp

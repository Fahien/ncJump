#pragma once

#include <Box2D/Box2D.h>
#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>
#include <nctl/String.h>

#include "types.h"

namespace jmp
{
class Input;
class Button;
class Entity;

class State
{
public:
    enum Value { IDLE = 0, MOVE, JUMP_UP, JUMP_DOWN };

    static State* get_state(State::Value state);

    nctl::String name;

    virtual void enter(const Input& input, Entity& entity) = 0;
    virtual void handle(const Input& input, Entity& entity) = 0;
    virtual void update(const f32 dt, const Input& input, Entity& entity) = 0;
    virtual void exit(Entity& entity) = 0;
};

class IdleState : public State
{
public:
    IdleState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class MoveState : public State
{
public:
    MoveState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class JumpUpState : public State
{
public:
    JumpUpState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class JumpDownState : public State
{
public:
    JumpDownState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class Entity
{
public:
    Entity(nc::SceneNode& scene);

    void set_state(const Input& input, State::Value state);

    void update(f32 dt, const Input& input);

    State* state = State::get_state(State::IDLE);

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
    f32 velocity_factor = 64.0f;
    f32 jump_y_factor = 360.0f;
    f32 jump_x_factor = 3.0f;
    f32 max_x_speed = 8.0f;
};

const char* to_str(State& state);

} // namespace jmp

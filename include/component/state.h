#pragma once

#include <nctl/String.h>

#include "types.h"

namespace jmp
{
class Input;
class Entity;

class StateComponent {
public:
    virtual void update(f32 dt, const Input& input, Entity& entity) = 0;
};

class SingleStateComponent : public StateComponent {
public:
    void update(f32 dt, const Input& input, Entity& entity) override;
};

class State
{
public:
    enum Value { IDLE = 0, MOVE, JUMP_UP, JUMP_DOWN };

    static State* get_state(State::Value state);

    nctl::String name;
    Value value;

    virtual void enter(const Input& input, Entity& entity) = 0;
    virtual void handle(const Input& input, Entity& entity) = 0;
    virtual void update(const f32 dt, const Input& input, Entity& entity) = 0;
    virtual void exit(Entity& entity) = 0;
};

const char* to_str(State& state);

class CharacterStateComponent : public StateComponent {
public:
    static CharacterStateComponent& into(StateComponent& s);

    CharacterStateComponent();

    void update(f32 dt, const Input& input, Entity& entity) override;

    void set_state(State::Value state, const Input& input, Entity& entity);

    State* state;
};

} // namespace jmp

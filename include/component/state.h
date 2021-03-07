#pragma once

#include <nctl/String.h>

#include "types.h"

namespace jmp
{
class Entity;
class MoveCommand;

class State
{
public:
    enum Value { IDLE = 0, MOVE, JUMP_UP, JUMP_DOWN, PUSH, PULL, DYING, MAX };

    static UNIQUE<State> create(State::Value state);

    nctl::String name;
    Value value;

    virtual void enter(Entity& entity, const MoveCommand* move) = 0;
    virtual void handle(Entity& entity, const MoveCommand& move) = 0;
    virtual void update(Entity& entity) = 0;
    virtual void exit(Entity& entity) = 0;
};

const char* to_str(State& state);

class StateComponent
{
public:
    StateComponent();

    StateComponent(const StateComponent&);
    StateComponent& operator=(const StateComponent&);

    StateComponent(StateComponent&&) = default;
    StateComponent& operator=(StateComponent&&) = default;

    void reset();

    void handle(Entity& entity, const MoveCommand& move);
    void update(Entity& entity);

    State& find_state(State::Value state);

    inline State& get_state();

    /// @param move Optional as a state change is not necessarily triggered by a command
    void set_state(State::Value state, Entity& entity, const MoveCommand* move = nullptr);

private:
    OPTION<u32> state_index = 0;

    ARRAY<UNIQUE<State>, State::MAX> states;
};

State& StateComponent::get_state()
{
    ASSERT(state_index.has_value() && *state_index < states.size());
    return *states[*state_index];
}

} // namespace jmp

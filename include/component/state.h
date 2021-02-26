#pragma once

#include <nctl/String.h>

#include "types.h"

namespace jmp
{
class Entity;
class MoveCommand;

class StateComponent {
public:
    virtual ~StateComponent() = default;

    virtual void reset() = 0;
    virtual UNIQUE<StateComponent> clone() = 0;

    virtual void handle(Entity& entity, const MoveCommand& move) = 0;
    virtual void update(Entity& entity) = 0;
};

class SingleStateComponent : public StateComponent {
public:
    void reset() override;
    UNIQUE<StateComponent> clone() override;

    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
};

class State
{
public:
    enum Value { IDLE = 0, MOVE, JUMP_UP, JUMP_DOWN, PUSH, PULL, DYING };

    static UNIQUE<State> create(State::Value state);

    nctl::String name;
    Value value;

    virtual void enter(Entity& entity, const MoveCommand* move) = 0;
    virtual void handle(Entity& entity, const MoveCommand& move) = 0;
    virtual void update(Entity& entity) = 0;
    virtual void exit(Entity& entity) = 0;
};

const char* to_str(State& state);

class CharacterStateComponent : public StateComponent {
public:
    static CharacterStateComponent& into(StateComponent& s);
    static CharacterStateComponent& get(Entity& e);

    CharacterStateComponent();

    void reset() override;
    UNIQUE<StateComponent> clone() override;

    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;

    /// @param move Optional as a state change is not necessarily triggered by a command
    void set_state(State::Value state, Entity& entity, const MoveCommand* move = nullptr);

    UNIQUE<State> state;
};

} // namespace jmp

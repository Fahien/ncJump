#pragma once

#include "types.h"

namespace jmp
{
class Entity;

class Command
{
public:
    virtual ~Command() = default;

    virtual void execute(Entity& entity) = 0;
};

class MoveCommand : public Command
{
public:
    inline static MoveCommand Jump();
    inline static MoveCommand Pull();

    MoveCommand() = default;
    inline MoveCommand(f32 x, f32 y);

    void execute(Entity& entity) override;

    f32 x = 0.0f;
    f32 y = 0.0f;
    bool jump = false;
    bool pull = false;
};

inline MoveCommand::MoveCommand(f32 x, f32 y)
    : x {x}
    , y {y}
{
}

inline MoveCommand MoveCommand::Jump()
{
    auto ret = MoveCommand();
    ret.jump = true;
    return ret;
}

inline MoveCommand MoveCommand::Pull()
{
    auto ret = MoveCommand();
    ret.pull = true;
    return ret;
}

} // namespace jmp

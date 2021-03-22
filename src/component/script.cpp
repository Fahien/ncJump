#include "component/script.h"
#include "command/command.h"

#include "entity.h"

namespace jmp
{
inline Direction::Value opposite(Direction::Value direction)
{
    switch (direction) {
    case Direction::RIGHT:
        return Direction::LEFT;
    case Direction::LEFT:
        return Direction::RIGHT;
    case Direction::UP:
        return Direction::DOWN;
    case Direction::DOWN:
        return Direction::UP;
    default:
        return Direction::NONE;
    }
}

inline DirectionFlags to_flags(Direction::Value direction)
{
    switch (direction) {
    case Direction::RIGHT:
        return DirectionFlags::RIGHT;
    case Direction::LEFT:
        return DirectionFlags::LEFT;
    case Direction::UP:
        return DirectionFlags::UP;
    case Direction::DOWN:
        return DirectionFlags::DOWN;
    default:
        return DirectionFlags::NONE;
    }
}

UNIQUE<Script> Script::from(const ScriptDef& def)
{
    switch (def.type) {
    case ScriptType::WANDERING:
        return MK<WanderingScript>();
    default:
        return {};
    }
}

void WanderingScript::update(Entity& entity)
{
    auto& physics = entity.get_physics();

    if (any(physics->obstacle & to_flags(direction))) {
        direction = opposite(direction);
    }

    // Move entity
    ASSERT(direction == Direction::LEFT || direction == Direction::RIGHT);
    f32 x = -f32(direction == Direction::LEFT) + f32(direction == Direction::RIGHT);
    MoveCommand(x, 0.0f).execute(entity);
}

} // namespace jmp

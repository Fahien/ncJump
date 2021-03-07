#include "command/command.h"

#include "entity.h"

namespace jmp
{
void MoveCommand::execute(Entity& entity)
{
    if (auto& state = entity.get_state()) {
        state->handle(entity, *this);
    }
}

} // namespace jmp

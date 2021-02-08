#include "command/command.h"

#include "entity.h"

namespace jmp
{
void MoveCommand::execute(Entity& entity)
{
    if (entity.state) {
        entity.state->handle(entity, *this);
    }
}

} // namespace jmp

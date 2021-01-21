#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "input.h"

namespace nc = ncine;

namespace jmp
{
bool Entity::is_player(const Entity& e)
{
    return e.name == "player";
}

Entity::Entity(nc::SceneNode& scene)
    : transform {scene}
{
}

void Entity::update(const f32 dt, const Input& input)
{
    if (graphics) {
        (*graphics)->update(input);
    }

    if (state) {
        (*state)->update(dt, input, *this);
    }

    if (physics) {
        physics->update();
    }
}

} // namespace jmp

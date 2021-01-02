#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "input.h"

namespace nc = ncine;

namespace jmp
{

Entity::Entity(nc::SceneNode& scene)
    : transform {scene}
    , graphics {MK<CharacterGraphicsComponent>(transform)}
    , state {MK<CharacterStateComponent>()}
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
    
    physics->update();
}

} // namespace jmp

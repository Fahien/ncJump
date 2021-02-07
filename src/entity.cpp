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

UNIQUE<Entity> Entity::clone()
{
    auto ret = MK<Entity>();

    ret->name = name;
    ret->transform = transform.clone();

    if (graphics) {
        ret->set_graphics(graphics->clone());
    }

    if (state) {
        ret->state = OPTION<UNIQUE<StateComponent>>((**state).clone());
    }

    if (physics) {
        auto new_physics = PhysicsComponent::character(
            *physics->body->GetWorld(), ret->transform.node->position());
        ret->set_physics(OPTION<PhysicsComponent>(MV(new_physics)));
    }

    return ret;
}

OPTION<PhysicsComponent>& Entity::get_physics()
{
    return physics;
}

void Entity::set_graphics(UNIQUE<GraphicsComponent> gfx)
{
    graphics = MV(gfx);
    if (graphics) {
        graphics->set(*this);
    }
}

void Entity::set_physics(OPTION<PhysicsComponent> ph)
{
    physics = MV(ph);
#ifdef BOX2D_PRE241
    physics->body->SetUserData(this);
#else
    physics->body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
#endif
}

void Entity::update(const f32 dt, const Input& input)
{
    if (graphics) {
        graphics->update(input);
    }

    if (state) {
        (*state)->update(dt, input, *this);
    }

    if (physics) {
        physics->update();
    }
}

} // namespace jmp

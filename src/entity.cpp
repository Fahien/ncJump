#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "command/command.h"
#include "component/script.h"
#include "config.h"
#include "input.h"

namespace nc = ncine;

namespace jmp
{
Entity::~Entity()
{
}

Entity::Entity(nc::SceneNode& scene)
    : transform {scene}
{
}

UNIQUE<Entity> Entity::clone()
{
    auto ret = MK<Entity>();

    ret->def = def;
    ret->transform = transform.clone();

    ret->graphics = graphics;
    ret->state = state;

    if (physics) {
        auto new_physics = physics;
        ret->set_physics(OPTION<PhysicsComponent>(MV(new_physics)));
    }

    for (auto& script : scripts) {
        ret->add_script(script->clone());
    }

    return ret;
}

void Entity::set_enabled(const bool e)
{
    enabled = e;

    transform.node->setEnabled(enabled);

    if (physics) {
        physics->set_enabled(enabled);
    }

    if (state) {
        state->reset();
    }
}

void Entity::set_position(const Vec2f& pos)
{
    transform.node->setPosition(pos);
    if (physics) {
        physics->set_position(pos);
    }
}

OPTION<PhysicsComponent>& Entity::get_physics()
{
    return physics;
}

void Entity::set_graphics(OPTION<GraphicsComponent> gfx)
{
    graphics = MV(gfx);
    if (graphics) {
        graphics->set(*this);
    }
}

void Entity::set_physics(OPTION<PhysicsComponent> ph)
{
    physics = MV(ph);
    if (physics) {
        physics->set_position(transform.node->position());

#ifdef BOX2D_PRE241
        physics->body->SetUserData(this);
#else
        physics->body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
#endif
    }
}

void Entity::set_state(OPTION<StateComponent> st)
{
    state = MV(st);
}

void Entity::update(const f32 dt)
{
    assert(enabled);

    for (auto& command : commands) {
        command->execute(*this);
    }
    commands.clear();

    for (auto& script : scripts) {
        script->update(*this);
    }

    if (graphics && physics) {
        graphics->update(*physics, nullptr);
    }

    if (state) {
        state->update(*this);
    }

    if (physics) {
        physics->update();
    }
}

} // namespace jmp

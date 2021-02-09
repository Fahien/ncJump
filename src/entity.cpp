#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "command/command.h"
#include "input.h"
#include "component/script.h"

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
        ret->state = state->clone();
    }

    if (physics) {
        auto new_physics = PhysicsComponent::character(
            *physics->body->GetWorld(), ret->transform.node->position());
        ret->set_physics(OPTION<PhysicsComponent>(MV(new_physics)));
    }

    for (auto& script : scripts) {
        ret->add_script(script->clone());
    }

    return ret;
}

void Entity::set_position(const Vec2f& pos)
{
    transform.node->setPosition(pos);
    if (physics) {
        auto bpos = b2Vec2(pos.x, pos.y);
        physics->body->SetTransform(bpos, 0);
    }
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
    physics->body->SetUserData(this);
}

/// @todo Do we still need input as a parameter here?
void Entity::update(const f32 dt, const Input& input)
{
    for (auto& command : commands) {
        command->execute(*this);
    }
    commands.clear();

    for (auto& script : scripts) {
        script->update(*this);
    }

    if (graphics) {
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

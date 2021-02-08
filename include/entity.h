#pragma once

#include <Box2D/Box2D.h>
#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>
#include <nctl/String.h>

#include "component/graphics.h"
#include "component/physics.h"
#include "component/state.h"
#include "component/transform.h"

namespace jmp
{
class Input;
class Command;

class Entity
{
public:
    static bool is_player(const Entity& e);
    static inline Entity& from(b2Fixture& fixture);

    Entity() = default;

    Entity(nc::SceneNode& scene);

    UNIQUE<Entity> clone();

    /// @brief Propagates this position to required components
    void set_position(const Vec2f& position);

    inline UNIQUE<GraphicsComponent>& get_graphics();
    OPTION<PhysicsComponent>& get_physics();

    void set_graphics(UNIQUE<GraphicsComponent> graphics);
    void set_physics(OPTION<PhysicsComponent> physics);

    inline void command(UNIQUE<Command> command);

    void update(f32 dt, const Input& input);

    nctl::String name = "tile";

    TransformComponent transform;

    UNIQUE<StateComponent> state;

private:
    UNIQUE<GraphicsComponent> graphics;
    OPTION<PhysicsComponent> physics;

    VECTOR<UNIQUE<Command>> commands;
};

Entity& Entity::from(b2Fixture& fixture)
{
    return *reinterpret_cast<Entity*>(fixture.GetBody()->GetUserData());
}

inline UNIQUE<GraphicsComponent>& Entity::get_graphics()
{
    return graphics;
}

inline void Entity::command(UNIQUE<Command> command)
{
    commands.pushBack(MV(command));
}

} // namespace jmp

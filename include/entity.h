#pragma once

#include <box2d/box2d.h>
#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>
#include <nctl/String.h>

#include "component/graphics_component.h"
#include "component/physics.h"
#include "component/state.h"
#include "component/transform.h"
#include "model/defs.h"

namespace jmp
{
struct Input;
class Command;
class Script;
class Config;

class Entity
{
public:
    static inline bool is_player(const Entity& e);
    static inline bool is_enemy(const Entity& e);
    static inline Entity& from(b2Fixture& fixture);

    Entity() = default;

    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;

    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    /// @brief Destructs the entity
    /// Needs to be in the source to destroy forward declared members
    ~Entity();

    Entity(nc::SceneNode& scene);

    UNIQUE<Entity> clone();

    /// @return Whether this entity is enabled or not
    inline bool is_enabled() const;

    /// @brief Enable or disable this entity
    void set_enabled(bool e);

    /// @brief Propagates this position to required components
    /// @param position Coordinates in scene space
    void set_position(const Vec2f& position, const Config& config);

    /// @return Position of this entity, convenient method
    inline Vec2f get_position() const;

    inline OPTION<GraphicsComponent>& get_graphics();
    OPTION<PhysicsComponent>& get_physics();

    void set_graphics(OPTION<GraphicsComponent> graphics);
    void set_physics(OPTION<PhysicsComponent> physics);

    inline const VECTOR<UNIQUE<Command>>& get_commands() const;
    inline void add_command(UNIQUE<Command> command);
    inline void add_script(UNIQUE<Script> script);

    void update(f32 dt);

    EntityType type = EntityType::NONE;

    TransformComponent transform;

    UNIQUE<StateComponent> state;

private:
    bool enabled = true;

    OPTION<GraphicsComponent> graphics;
    OPTION<PhysicsComponent> physics;

    VECTOR<UNIQUE<Command>> commands;
    VECTOR<UNIQUE<Script>> scripts;
};

Entity& Entity::from(b2Fixture& fixture)
{
#ifdef BOX2D_PRE241
    return *reinterpret_cast<Entity*>(fixture.GetBody()->GetUserData());
#else
    return *reinterpret_cast<Entity*>(fixture.GetBody()->GetUserData().pointer);
#endif
}

bool Entity::is_player(const Entity& e)
{
    return e.type == EntityType::PLAYER;
}

bool Entity::is_enemy(const Entity& e)
{
    return e.type == EntityType::ENEMY;
}

inline bool Entity::is_enabled() const
{
    return enabled;
}

inline Vec2f Entity::get_position() const
{
    return transform.node->position();
}

inline OPTION<GraphicsComponent>& Entity::get_graphics()
{
    return graphics;
}

inline const VECTOR<UNIQUE<Command>>& Entity::get_commands() const
{
    return commands;
}

inline void Entity::add_command(UNIQUE<Command> command)
{
    commands.pushBack(MV(command));
}

inline void Entity::add_script(UNIQUE<Script> script)
{
    scripts.pushBack(MV(script));
}

} // namespace jmp

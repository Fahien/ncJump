#pragma once

#include <box2d/box2d.h>
#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>
#include <nctl/String.h>

#include "component/graphics.h"
#include "component/physics.h"
#include "component/state.h"
#include "component/transform.h"

namespace jmp
{
struct Input;
class Command;
class Script;
class Config;

class Entity
{
public:
    enum class Type {
        NONE = 0,
        TILE,
        PLAYER,
        ENEMY
    };

    static inline bool is_player(const Entity& e);
    static inline bool is_enemy(const Entity& e);
    static inline Entity& from(b2Fixture& fixture);

    Entity() = default;

    Entity(nc::SceneNode& scene);

    UNIQUE<Entity> clone();

    /// @brief Propagates this position to required components
    /// @param position Coordinates in scene space
    void set_position(const Vec2f& position, const Config& config);

    /// @return Position of this entity, convenient method
    inline Vec2f get_position() const;

    inline UNIQUE<GraphicsComponent>& get_graphics();
    OPTION<PhysicsComponent>& get_physics();

    void set_graphics(UNIQUE<GraphicsComponent> graphics);
    void set_physics(OPTION<PhysicsComponent> physics);

    inline void add_command(UNIQUE<Command> command);
    inline void add_script(UNIQUE<Script> script);

    void update(f32 dt, const Input& input);

    Type type = Type::NONE;

    TransformComponent transform;

    UNIQUE<StateComponent> state;

private:
    UNIQUE<GraphicsComponent> graphics;
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
    return e.type == Type::PLAYER;
}

bool Entity::is_enemy(const Entity& e)
{
    return e.type == Type::ENEMY;
}

inline Vec2f Entity::get_position() const
{
    return transform.node->position();
}

inline UNIQUE<GraphicsComponent>& Entity::get_graphics()
{
    return graphics;
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

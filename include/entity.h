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

class Entity
{
public:
    static bool is_player(const Entity& e);
    static inline Entity& from(b2Fixture& fixture);

    Entity() = default;

    Entity(nc::SceneNode& scene);

    inline UNIQUE<GraphicsComponent>& get_graphics();
    OPTION<PhysicsComponent>& get_physics();

    void set_graphics(UNIQUE<GraphicsComponent> graphics);
    void set_physics(OPTION<PhysicsComponent> physics);

    void update(f32 dt, const Input& input);

    nctl::String name = "tile";

    TransformComponent transform;

    OPTION<UNIQUE<StateComponent>> state;

private:
    UNIQUE<GraphicsComponent> graphics;
    OPTION<PhysicsComponent> physics;
};

Entity& Entity::from(b2Fixture& fixture)
{
#ifdef BOX2D_PRE241
    return *reinterpret_cast<Entity*>(fixture.GetBody()->GetUserData());
#else
    return *reinterpret_cast<Entity*>(fixture.GetBody()->GetUserData().pointer);
#endif
}

inline UNIQUE<GraphicsComponent>& Entity::get_graphics()
{
    return graphics;
}

} // namespace jmp

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
class Input;

class Entity
{
public:
    static bool is_player(const Entity& e);
    static inline Entity& from(b2Fixture& fixture);

    Entity() = default;

    Entity(nc::SceneNode& scene);

    OPTION<PhysicsComponent>& get_physics();

    void set_physics(OPTION<PhysicsComponent> physics);

    void update(f32 dt, const Input& input);

    nctl::String name = "tile";

    TransformComponent transform;

    OPTION<UNIQUE<GraphicsComponent>> graphics;

    OPTION<UNIQUE<StateComponent>> state;

private:
    OPTION<PhysicsComponent> physics;
};

Entity& Entity::from(b2Fixture& fixture)
{
    return *reinterpret_cast<Entity*>(fixture.GetBody()->GetUserData().pointer);
}

} // namespace jmp

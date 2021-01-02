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
class Button;
class Entity;

class Entity
{
public:
    Entity(nc::SceneNode& scene);

    void update(f32 dt, const Input& input);

    TransformComponent transform;

    OPTION<UNIQUE<GraphicsComponent>> graphics;

    OPTION<PhysicsComponent> physics = std::nullopt;

    OPTION<UNIQUE<StateComponent>> state;
};

} // namespace jmp

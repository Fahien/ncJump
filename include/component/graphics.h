#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "types.h"

namespace jmp
{
class Config;
struct Input;
class TransformComponent;
class PhysicsComponent;
class Entity;

/// @brief Texture values that can be used by the GUI
class GuiTexture
{
public:
    void* id = nullptr;
    ImVec2 size = ImVec2(0.0f, 0.0f);
    ImVec2 uvs[2] = {ImVec2(0.0f, 0.0f), ImVec2(0.0f, 0.0f)};
};

class GraphicsComponent {
public:
    virtual ~GraphicsComponent() = default;

    virtual UNIQUE<GraphicsComponent> clone() const = 0;

    virtual void set(Entity& entity) = 0;

    virtual void update(const PhysicsComponent& physics, const Input* input) = 0;

    /// @brief Updates the sprite representing this graphics component.
    /// It is useful when the sprite is detached from the scene graph.
    virtual void update_sprite(f32 dt) = 0;

    /// @return A texture representation that can be used by the GUI
    virtual GuiTexture get_guitex(const Config& config) const = 0;

    inline virtual Direction::Value get_direction() const;
};

inline Direction::Value GraphicsComponent::get_direction() const
{
    return Direction::NONE;
}

// Graphics component with a single static image which never changes
class SingleGraphicsComponent : public GraphicsComponent {
public:
    static SingleGraphicsComponent& into(GraphicsComponent& g);

    SingleGraphicsComponent() = default;
    SingleGraphicsComponent(TransformComponent& t, nc::Texture& texture);

    UNIQUE<GraphicsComponent> clone() const override;

    void set(Entity& entity) override;

    inline void update(const PhysicsComponent& physics, const Input* input) override;

    inline void update_sprite(f32 dt) override;
    GuiTexture get_guitex(const Config& config) const override;

    // nCine works with pointers
    nc::Sprite sprite;
};

inline void SingleGraphicsComponent::update(const PhysicsComponent& physics, const Input* input)
{
}

inline void SingleGraphicsComponent::update_sprite(f32 dt)
{
    sprite.update(dt);
}

class CharacterGraphicsComponent : public GraphicsComponent {
public:
    static CharacterGraphicsComponent& into(GraphicsComponent& g);

    CharacterGraphicsComponent() = default;

    UNIQUE<GraphicsComponent> clone() const override;

    void set(Entity& entity) override;

    void update(const PhysicsComponent& physics, const Input* input) override;

    inline void update_sprite(f32 dt) override;
    GuiTexture get_guitex(const Config& config) const override;

    inline Direction::Value get_direction() const override;

    Direction::Value direction = Direction::RIGHT;

    nc::AnimatedSprite idle;
    nc::AnimatedSprite movement;
    nc::AnimatedSprite jump_up;
    nc::AnimatedSprite jump_down;
    nc::AnimatedSprite push;
    nc::AnimatedSprite pull;
    nc::AnimatedSprite dying;
};

inline void CharacterGraphicsComponent::update_sprite(f32 dt)
{
    // The sprite representing this component is "idle"
    idle.update(dt);
}

inline Direction::Value CharacterGraphicsComponent::get_direction() const
{
    return direction;
}

} // namespace jmp

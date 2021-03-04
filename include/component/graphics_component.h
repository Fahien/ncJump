#pragma once

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "model/defs.h"
#include "types.h"

namespace jmp
{
class Config;
struct Input;
class TransformComponent;
class PhysicsComponent;
class Entity;
class GraphicsFactory;

/// @brief Texture values that can be used by the GUI
class GuiTexture
{
public:
    void* id = nullptr;
    ImVec2 size = ImVec2(0.0f, 0.0f);
    ImVec2 uvs[2] = {ImVec2(0.0f, 0.0f), ImVec2(0.0f, 0.0f)};
};

class SubGraphics
{
public:
    virtual ~SubGraphics() = default;

    virtual UNIQUE<SubGraphics> clone() const = 0;

    /// @return A reference to a sprite, which could be of any kind
    virtual nc::Sprite& get_sprite_mut() = 0;

    /// @return A reference to a sprite, which could be of any kind
    virtual const nc::Sprite& get_sprite() const = 0;

    void set(Entity& entity);

    /// @return A texture representation that can be used by the GUI
    GuiTexture get_guitex(const Config& config) const;

    /// @brief Updates the sprite representing this graphics component.
    /// It is useful when the sprite is detached from the scene graph.
    inline void update_sprite(f32 dt);
};

inline void SubGraphics::update_sprite(f32 dt)
{
    get_sprite_mut().update(dt);
}

/// @brief Single static image which never changes
class StaticSubGraphics : public SubGraphics
{
public:
    static StaticSubGraphics& into(SubGraphics& g);

    StaticSubGraphics() = default;
    StaticSubGraphics(const SubGraphicsDef& def, GraphicsFactory& factory);

    UNIQUE<SubGraphics> clone() const override;

    inline nc::Sprite& get_sprite_mut() override;
    inline const nc::Sprite& get_sprite() const override;

private:
    nc::Sprite sprite;
};

inline nc::Sprite& StaticSubGraphics::get_sprite_mut()
{
    return sprite;
}

inline const nc::Sprite& StaticSubGraphics::get_sprite() const
{
    return sprite;
}

class AnimSubGraphics : public SubGraphics
{
public:
    static AnimSubGraphics& into(SubGraphics& g);

    AnimSubGraphics() = default;
    AnimSubGraphics(const SubGraphicsDef& def, GraphicsFactory& factory);

    UNIQUE<SubGraphics> clone() const override;

    inline nc::Sprite& get_sprite_mut() override;
    inline const nc::Sprite& get_sprite() const override;

    nc::AnimatedSprite anim;
};

inline nc::Sprite& AnimSubGraphics::get_sprite_mut()
{
    return anim;
}

inline const nc::Sprite& AnimSubGraphics::get_sprite() const
{
    return anim;
}

class GraphicsComponent
{
public:
    GraphicsComponent() = default;
    GraphicsComponent(const GraphicsDef& def, GraphicsFactory& factory);

    GraphicsComponent(const GraphicsComponent&);
    GraphicsComponent& operator=(const GraphicsComponent&);

    GraphicsComponent(GraphicsComponent&&) = default;
    GraphicsComponent& operator=(GraphicsComponent&&) = default;

    void set(Entity& entity);

    /// @brief Change current sub-graphics
    /// @param index Index of the new sub-graphics in the `subs` array
    void set_current(const OPTION<u32> index);

    /// @return The current sub-graphics, nullptr otherwise
    SubGraphics* get_current_mut();

    /// @return The current sub-graphics, nullptr otherwise
    const SubGraphics* get_current() const;

    void update(const PhysicsComponent& physics, const Input* input);

    Direction::Value direction = Direction::NONE;

    /// A graphics component may have multiple sub-graphics
    /// each one related to a different entity state.
    VECTOR<UNIQUE<SubGraphics>> subs;

private:
    /// Index of the current sub-graphics
    OPTION<u32> current_index = 0;
};

} // namespace jmp

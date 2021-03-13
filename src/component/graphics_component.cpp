#include "component/graphics_component.h"

#include <ncine/FileSystem.h>

#include "component/transform.h"
#include "config.h"
#include "entity.h"
#include "factory/graphics_factory.h"
#include "input.h"

namespace jmp
{
void SubGraphics::set(Entity& entity)
{
    get_sprite_mut().setParent(entity.transform.node.get());
}

nc::Sprite clone_sprite(const nc::Sprite& sprite)
{
    auto ret = sprite.clone();
    ret.setLayer(sprite.layer());
    return ret;
}

/// @todo Workaround as `AnimatedSprite::clone()` is currently bugged
nc::AnimatedSprite clone_sprite(const nc::AnimatedSprite& sprite)
{
    auto ret = sprite.clone();
    ret.setFrame(0);
    ret.setLayer(sprite.layer());
    return ret;
}

StaticSubGraphics& StaticSubGraphics::into(SubGraphics& g)
{
    return reinterpret_cast<StaticSubGraphics&>(g);
}

StaticSubGraphics::StaticSubGraphics(const SubGraphicsDef& def, GraphicsFactory& factory)
    : sprite {&factory.get_or_create(def.path)}
{
    sprite.setLayer(def.layer);
    ASSERT(def.rects.size() > 0);
    sprite.setTexRect(def.rects[0]);
}

UNIQUE<SubGraphics> StaticSubGraphics::clone() const
{
    auto ret = MK<StaticSubGraphics>();
    ret->sprite = clone_sprite(sprite);
    return ret;
}

AnimSubGraphics& AnimSubGraphics::into(SubGraphics& g)
{
    return reinterpret_cast<AnimSubGraphics&>(g);
}

AnimSubGraphics::AnimSubGraphics(const SubGraphicsDef& def, GraphicsFactory& factory)
    : anim {factory.create_anim(def)}
{
}

UNIQUE<SubGraphics> AnimSubGraphics::clone() const
{
    auto ret = MK<AnimSubGraphics>();
    ret->anim = clone_sprite(anim);
    return ret;
}

GraphicsComponent::GraphicsComponent(const GraphicsDef& def, GraphicsFactory& factory)
{
    for (auto& sub : def.subs) {
        subs.pushBack(factory.create(sub));
    }
}

GraphicsComponent::GraphicsComponent(const GraphicsComponent& o)
    : direction {o.direction}
    , current_index {o.current_index}
{
    for (auto& sub : o.subs) {
        subs.pushBack(sub->clone());
    }
}

GraphicsComponent& GraphicsComponent::operator=(const GraphicsComponent& o)
{
    direction = o.direction;

    nc::SceneNode* parent = nullptr;

    if (auto current = get_current_mut()) {
        parent = current->get_sprite_mut().parent();
    }

    current_index = o.current_index;

    subs.clear();
    for (auto& sub : o.subs) {
        subs.pushBack(sub->clone());
    }

    if (auto current = get_current_mut()) {
        current->get_sprite_mut().setParent(parent);
    }

    return *this;
}

void GraphicsComponent::set(Entity& entity)
{
    if (auto current = get_current_mut()) {
        current->set(entity);
    }
}

void GraphicsComponent::set_current(const OPTION<u32> index)
{
    ASSERT(index < subs.size());

    nc::SceneNode* parent = nullptr;

    if (auto current = get_current_mut()) {
        auto& sprite = current->get_sprite_mut();
        parent = sprite.parent();
        sprite.setParent(nullptr);
    }

    current_index = index;
    if (index) {
        ASSERT(*index < subs.size());
        get_current_mut()->get_sprite_mut().setParent(parent);
    }
}

SubGraphics* GraphicsComponent::get_current_mut()
{
    if (current_index && *current_index < subs.size()) {
        return subs[*current_index].get();
    }
    return nullptr;
}

const SubGraphics* GraphicsComponent::get_current() const
{
    if (current_index && *current_index < subs.size()) {
        return subs[*current_index].get();
    }
    return nullptr;
}

GuiTexture SubGraphics::get_guitex(const Config& config) const
{
    GuiTexture tex;

    auto& sprite = get_sprite();

    auto texture = const_cast<nc::Texture*>(sprite.texture());
    tex.id = texture->guiTexId();

    auto rect = sprite.texRect();
    tex.size = ImVec2(rect.w * config.scale.gui, rect.h * config.scale.gui);

    f32 tw = texture->width();
    f32 th = texture->height();
    tex.uvs[0] = ImVec2(rect.x / tw, rect.y / th);
    tex.uvs[1] = ImVec2((rect.x + rect.w) / tw, (rect.y + rect.h) / th);

    return tex;
}

void GraphicsComponent::update(const PhysicsComponent& physics, const Input* input)
{
    // Set direction of the sprite
    f32 movement_x = input ? input->joystick.move.x : physics.body->GetLinearVelocity().x;

    if (!closef(movement_x, 0.0f)) {
        bool flipped_x = movement_x < 0.0;
        direction = flipped_x ? Direction::LEFT : Direction::RIGHT;
        if (auto current = get_current_mut()) {
            current->get_sprite_mut().setFlippedX(flipped_x);
        }
    }
}

} // namespace jmp

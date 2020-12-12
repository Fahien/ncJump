#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "types.h"

namespace nc = ncine;

namespace jmp
{
const char* to_string(Entity::State state)
{
    switch (state) {
    case Entity::State::IDLE:
        return "IDLE";
    case Entity::State::MOVE:
        return "MOVE";
    default:
        FATAL_ASSERT_MSG(false, "Failed to get string for character state");
    }
}


Entity::Entity()
    : node {&nc::theApplication().rootNode()}
    , idle_texture {PATH("img/hero/herochar_idle_anim_strip_4.png")}
    , idle {&node, &idle_texture}
    , movement_texture {PATH("img/hero/herochar_run_anim_strip_6.png")}
    , movement {nullptr, &movement_texture}
{
    node.x += 100;
    node.y += 50;

    // Idle animation
    {
        idle_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(idle_texture.width() / 4.0f, idle_texture.height());

        for (int i = 0; i < 4; ++i) {
            anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        idle.addAnimation(nctl::makeUnique<nc::RectAnimation>(anim));
        idle.setPaused(false);
    }

    // Movement animation
    {
        movement_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(movement_texture.width() / 6.0f, movement_texture.height());

        for (int i = 0; i < 6; ++i) {
            anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        movement.addAnimation(nctl::makeUnique<nc::RectAnimation>(anim));
        movement.setPaused(false);
    }
}

void Entity::setState(State state)
{
    switch (state) {
    case State::IDLE:
        node.removeChildNode(&movement);
        node.addChildNode(&idle);
        break;
    case State::MOVE:
        node.removeChildNode(&idle);
        node.addChildNode(&movement);
        break;
    default:
        FATAL_ASSERT_MSG(false, "Failed to set state for a character");
    }

    this->state = state;
}

void Entity::move(const Vec2f& mv)
{
    // States that allow moving
    if (state == State::IDLE || state == State::MOVE) {
        node.x += mv.x;
        node.y += mv.y;

        if (state == State::IDLE && (mv.x != 0.0 || mv.y != 0.0)) {
            setState(State::MOVE);
        }

        if (state == State::MOVE) {
            if (mv.x == 0.0 && mv.y == 0.0) {
                setState(State::IDLE);
            } else {
                // Set direction of sprite
                bool flipped_x = mv.x < 0.0;
                movement.setFlippedX(flipped_x);
                idle.setFlippedX(flipped_x);
            }
        }
    }
}

} // namespace jmp

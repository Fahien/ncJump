#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "input.h"

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
    case Entity::State::JUMP_UP:
        return "JUMP_UP";
    case Entity::State::JUMP_DOWN:
        return "JUMP_DOWN";
    default:
        FATAL_ASSERT_MSG(false, "Failed to get string for character state");
    }
}

Entity::Entity(nc::SceneNode& scene)
    : node {&scene}
    , idle_texture {PATH("img/hero/herochar_idle_anim_strip_4.png")}
    , idle {&node, &idle_texture}
    , movement_texture {PATH("img/hero/herochar_run_anim_strip_6.png")}
    , movement {nullptr, &movement_texture}
    , jump_up_texture {PATH("img/hero/herochar_jump_up_anim_strip_3.png")}
    , jump_down_texture {PATH("img/hero/herochar_jump_down_anim_strip_3.png")}
    , jump_up {nullptr, &jump_up_texture}
    , jump_down {nullptr, &jump_down_texture}
{
    node.x += 4;
    node.y += 2;

    // Idle animation
    {
        idle_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(idle_texture.width() / 4.0f, idle_texture.height());

        for (int i = 0; i < 4; ++i) {
            anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        idle.addAnimation(MK<nc::RectAnimation>(anim));
        idle.setPaused(false);
        idle.setLayer(1);
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

        movement.addAnimation(MK<nc::RectAnimation>(anim));
        movement.setPaused(false);
        movement.setLayer(1);
    }

    // Jump animation
    {
        jump_up_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto jump_up_anim = nc::RectAnimation(
            0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

        auto sprite_size = Vec2f(jump_up_texture.width() / 3.0f, jump_up_texture.height());

        for (int i = 0; i < 3; ++i) {
            jump_up_anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
        }

        jump_up.addAnimation(MK<nc::RectAnimation>(jump_up_anim));
        jump_up.setPaused(false);
        jump_up.setLayer(1);

        jump_down_texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

        auto jump_down_anim = nc::RectAnimation(0.125f,
            nc::RectAnimation::LoopMode::ENABLED,
            nc::RectAnimation::RewindMode::FROM_START);

        for (int i = 0; i < 3; ++i) {
            jump_down_anim.addRect(nc::Recti(i * sprite_size.x, 0.0, sprite_size.x, sprite_size.y));
        }

        jump_down.addAnimation(MK<nc::RectAnimation>(jump_down_anim));
        jump_down.setPaused(false);
        jump_down.setLayer(1);
    }
}

void Entity::setState(State state)
{
    if (state == this->state) {
        return;
    }

    // Remove old sprite
    switch (this->state) {
    case State::IDLE:
        node.removeChildNode(&idle);
        break;
    case State::MOVE:
        node.removeChildNode(&movement);
        break;
    case State::JUMP_UP:
        node.removeChildNode(&jump_up);
        break;
    case State::JUMP_DOWN:
        node.removeChildNode(&jump_down);
        break;
    default:
        FATAL_ASSERT_MSG(false, "Failed to set state for a character");
    }

    // Add new sprite
    switch (state) {
    case State::IDLE:
        node.addChildNode(&idle);
        break;
    case State::MOVE:
        node.addChildNode(&movement);
        break;
    case State::JUMP_UP:
        node.addChildNode(&jump_up);
        break;
    case State::JUMP_DOWN:
        node.addChildNode(&jump_down);
        break;
    default:
        FATAL_ASSERT_MSG(false, "Failed to set state for a character");
    }

    this->state = state;
}

void Entity::move(const Vec2f& mv)
{
    // States that allow moving
    if (state == State::IDLE || state == State::MOVE || state == State::JUMP_UP ||
        state == State::JUMP_DOWN) {
        float force_factor = velocity_factor;
        if (state == State::JUMP_UP || state == State::JUMP_DOWN) {
            force_factor /= 32.0f;
        }
        auto force = b2Vec2(force_factor * mv.x, 0.0f * mv.y);
        bool wake = true; // @todo Learn more
        body->ApplyLinearImpulse(force, body->GetWorldCenter(), wake);

        if (state == State::IDLE && (mv.x != 0.0 || mv.y != 0.0)) {
            setState(State::MOVE);
        }

        if (state == State::MOVE || state == State::JUMP_UP || state == State::JUMP_DOWN) {
            if (state == State::MOVE && mv.x == 0.0 && mv.y == 0.0) {
                setState(State::IDLE);
            } else {
                // Set direction of sprite
                bool flipped_x = mv.x < 0.0;
                movement.setFlippedX(flipped_x);
                idle.setFlippedX(flipped_x);
                jump_up.setFlippedX(flipped_x);
                jump_down.setFlippedX(flipped_x);
            }
        }
    }
}

void Entity::jump(const Button& button)
{
    auto vel = body->GetLinearVelocity();
    if (state == State::JUMP_UP) {
        if (vel.y <= 0.0f) {
            setState(State::JUMP_DOWN);
        }
    } else if (state == State::JUMP_DOWN) {
        if (vel.y == 0.0f) {
            setState(State::MOVE);
        }
    } else if (button.just_down) {
        auto impulse = b2Vec2(jump_x_factor, jump_y_factor);
        body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
        setState(State::JUMP_UP);
    }
}

void Entity::update(const Input& input)
{
    if (input.joystick.a.just_down) {
        jump(input.joystick.a);
    } else {
        move(input.joystick.move);
    }
}
} // namespace jmp

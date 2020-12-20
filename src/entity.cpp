#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "input.h"

namespace nc = ncine;

namespace jmp
{
State* State::get_state(State::Value state)
{
    static UNIQUE<State> states[] = {
        MK<IdleState>(),
        MK<MoveState>(),
        MK<JumpUpState>(),
        MK<JumpDownState>(),
    };

    return states[state].get();
}

IdleState::IdleState()
{
    name = "IDLE";
}

void IdleState::enter(const Input& input, Entity& entity)
{
    entity.node.addChildNode(&entity.idle);
}

void IdleState::handle(const Input& input, Entity& entity)
{
    // Can move
    if (input.joystick.move.x != 0.0) {
        entity.set_state(input, State::MOVE);
    }

    // Can jump
    if (input.joystick.a.just_down) {
        entity.set_state(input, State::JUMP_UP);
    }
}

void IdleState::update(const f32 dt, const Input& input, Entity& entity)
{
    // Nothing
}

void IdleState::exit(Entity& entity)
{
    entity.node.removeChildNode(&entity.idle);
}

MoveState::MoveState()
{
    name = "MOVE";
}

void MoveState::enter(const Input& input, Entity& entity)
{
    entity.node.addChildNode(&entity.movement);
}

void MoveState::handle(const Input& input, Entity& entity)
{
    // Can go quiet
    if (input.joystick.move.x == 0.0) {
        entity.set_state(input, State::IDLE);
    }

    // Can jump
    if (input.joystick.a.just_down) {
        entity.set_state(input, State::JUMP_UP);
    }
}

void MoveState::update(const f32 dt, const Input& input, Entity& entity)
{
    auto force = b2Vec2(entity.velocity_factor * input.joystick.move.x, 0.0f);
    entity.body->ApplyLinearImpulse(force, entity.body->GetWorldCenter(), true);
}

void MoveState::exit(Entity& entity)
{
    entity.node.removeChildNode(&entity.movement);
}

JumpUpState::JumpUpState()
{
    name = "JUMP_UP";
}

void JumpUpState::handle(const Input& input, Entity& entity)
{
    // @todo Double jump?
    if (entity.body->GetLinearVelocity().y <= 0.0f) {
        entity.set_state(input, State::JUMP_DOWN);
    }
}

void JumpUpState::enter(const Input& input, Entity& entity)
{
    entity.node.addChildNode(&entity.jump_up);

    auto force = b2Vec2(entity.velocity_factor * input.joystick.move.x, entity.jump_y_factor);
    entity.body->ApplyLinearImpulse(force, entity.body->GetWorldCenter(), true);
}

void JumpUpState::update(const f32 dt, const Input& input, Entity& entity)
{
    // Can move a bit
    auto force = b2Vec2(entity.jump_x_factor * input.joystick.move.x, 0.0);
    entity.body->ApplyLinearImpulse(force, entity.body->GetWorldCenter(), true);
}

void JumpUpState::exit(Entity& entity)
{
    entity.node.removeChildNode(&entity.jump_up);
}

JumpDownState::JumpDownState()
{
    name = "JUMP_DOWN";
}

void JumpDownState::enter(const Input& input, Entity& entity)
{
    entity.node.addChildNode(&entity.jump_down);
}

void JumpDownState::handle(const Input& input, Entity& entity)
{
    if (entity.body->GetLinearVelocity().y == 0.0f) {
        entity.set_state(input, State::MOVE);
    }
}

void JumpDownState::update(const f32, const Input& input, Entity& entity)
{
    // Can move a bit
    auto force = b2Vec2(entity.jump_x_factor * input.joystick.move.x, 0.0);
    entity.body->ApplyLinearImpulse(force, entity.body->GetWorldCenter(), true);
}

void JumpDownState::exit(Entity& entity)
{
    entity.node.removeChildNode(&entity.jump_down);
}

const char* to_str(State& state)
{
    return state.name.data();
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

void Entity::set_state(const Input& input, State::Value value)
{
    auto new_state = State::get_state(value);
    if (state == new_state) {
        return;
    }

    state->exit(*this);
    new_state->enter(input, *this);

    state = new_state;
}

void Entity::update(const f32 dt, const Input& input)
{
    // Set direction of sprite
    float movement_x = body->GetLinearVelocity().x;
    if (movement_x) {
        bool flipped_x = movement_x < 0.0;
        movement.setFlippedX(flipped_x);
        idle.setFlippedX(flipped_x);
        jump_up.setFlippedX(flipped_x);
        jump_down.setFlippedX(flipped_x);
    }

    state->handle(input, *this);
    state->update(dt, input, *this);

    // Apply air resistance
    auto vel = -body->GetLinearVelocity();
    auto vel_len = vel.LengthSquared();
    vel.x *= air_factor * vel_len;
    vel.y *= air_factor * vel_len;
    body->ApplyForceToCenter(vel, false);
}

} // namespace jmp

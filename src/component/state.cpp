#include "component/state.h"

#include "entity.h"
#include "input.h"

namespace jmp
{
class IdleState : public State
{
public:
    IdleState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class MoveState : public State
{
public:
    MoveState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class JumpUpState : public State
{
public:
    JumpUpState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

class JumpDownState : public State
{
public:
    JumpDownState();
    void enter(const Input& input, Entity& entity) override;
    void handle(const Input& input, Entity& entity) override;
    void update(const f32 dt, const Input& input, Entity& entity) override;
    void exit(Entity& entity) override;
};

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

#define CHAR_GFX(e) reinterpret_cast<CharacterGraphicsComponent&>(*e.graphics.value().get())
#define CHAR_STT(e) reinterpret_cast<CharacterStateComponent&>(*e.state.value().get())

void IdleState::enter(const Input& input, Entity& entity)
{
    entity.transform.node.addChildNode(&CHAR_GFX(entity).idle);
}

void can_fall(const Input& input, Entity& entity)
{
    // Can fall if there is no platform below
    bool should_fall = true;

    for (auto edge = entity.physics->body->GetContactList(); edge; edge = edge->next) {
        auto normal = edge->contact->GetManifold()->localNormal;
        if (edge->contact->GetFixtureA() == entity.physics->body->GetFixtureList()) {
            normal = -normal;
        }
        if (normal.y > 0.5f) {
            should_fall = false;
            break;
        }
    }

    if (should_fall) {
        CHAR_STT(entity).set_state(State::JUMP_DOWN, input, entity);
    }
}

void IdleState::handle(const Input& input, Entity& entity)
{
    // Can move
    if (input.joystick.move.x != 0.0) {
        CHAR_STT(entity).set_state(State::MOVE, input, entity);
    }

    // Can jump
    if (input.joystick.a.just_down) {
        CHAR_STT(entity).set_state(State::JUMP_UP, input, entity);
    }

    can_fall(input, entity);
}

void IdleState::update(const f32 dt, const Input& input, Entity& entity)
{
    // Nothing
}

void IdleState::exit(Entity& entity)
{
    entity.transform.node.removeChildNode(&CHAR_GFX(entity).idle);
}

MoveState::MoveState()
{
    name = "MOVE";
}

void MoveState::enter(const Input& input, Entity& entity)
{
    entity.transform.node.addChildNode(&CHAR_GFX(entity).movement);
}

void MoveState::handle(const Input& input, Entity& entity)
{
    // Can go quiet
    if (input.joystick.move.x == 0.0) {
        CHAR_STT(entity).set_state(State::IDLE, input, entity);
    }

    // Can jump
    if (input.joystick.a.just_down) {
        CHAR_STT(entity).set_state(State::JUMP_UP, input, entity);
    }

    can_fall(input, entity);
}

void can_move_on_x(const Input& input, Entity& entity, f32 x_factor)
{
    f32 x_velocity = entity.physics->body->GetLinearVelocity().x;

    bool opposite_move = (input.joystick.move.x < 0 && x_velocity >= 0) ||
        (input.joystick.move.x >= 0 && x_velocity < 0);
    bool within_limit =
        fabs(entity.physics->body->GetLinearVelocity().x) < entity.physics->max_x_speed;

    if (opposite_move || within_limit) {
        auto force = b2Vec2(x_factor * input.joystick.move.x, 0.0f);
        entity.physics->body->ApplyLinearImpulse(
            force, entity.physics->body->GetWorldCenter(), true);
    }
}

void MoveState::update(const f32 dt, const Input& input, Entity& entity)
{
    can_move_on_x(input, entity, entity.physics->velocity_factor);
}

void MoveState::exit(Entity& entity)
{
    entity.transform.node.removeChildNode(&CHAR_GFX(entity).movement);
}

JumpUpState::JumpUpState()
{
    name = "JUMP_UP";
}

void JumpUpState::handle(const Input& input, Entity& entity)
{
    // @todo Double jump?
    if (entity.physics->body->GetLinearVelocity().y <= 0.0f) {
        CHAR_STT(entity).set_state(State::JUMP_DOWN, input, entity);
    }
}

void JumpUpState::enter(const Input& input, Entity& entity)
{
    entity.transform.node.addChildNode(&CHAR_GFX(entity).jump_up);

    auto force = b2Vec2(
        entity.physics->jump_x_factor * input.joystick.move.x, entity.physics->jump_y_factor);
    entity.physics->body->ApplyLinearImpulse(force, entity.physics->body->GetWorldCenter(), true);
}

void JumpUpState::update(const f32 dt, const Input& input, Entity& entity)
{
    // Can move a bit
    can_move_on_x(input, entity, entity.physics->jump_x_factor);
}

void JumpUpState::exit(Entity& entity)
{
    entity.transform.node.removeChildNode(&CHAR_GFX(entity).jump_up);
}

JumpDownState::JumpDownState()
{
    name = "JUMP_DOWN";
}

void JumpDownState::enter(const Input& input, Entity& entity)
{
    entity.transform.node.addChildNode(&CHAR_GFX(entity).jump_down);
}

void JumpDownState::handle(const Input& input, Entity& entity)
{
    for (auto edge = entity.physics->body->GetContactList(); edge; edge = edge->next) {
        auto& normal = edge->contact->GetManifold()->localNormal;
        if (normal.y != 0.0) {
            LOGI_X("Normal (%f, %f)", normal.x, normal.y);
            CHAR_STT(entity).set_state(State::MOVE, input, entity);
            break;
        }
    }
}

void JumpDownState::update(const f32, const Input& input, Entity& entity)
{
    // Can move a bit
    can_move_on_x(input, entity, entity.physics->jump_x_factor);
}

void JumpDownState::exit(Entity& entity)
{
    entity.transform.node.removeChildNode(&CHAR_GFX(entity).jump_down);
}

const char* to_str(State& state)
{
    return state.name.data();
}

CharacterStateComponent& CharacterStateComponent::into(StateComponent& s)
{
    return reinterpret_cast<CharacterStateComponent&>(s);
}

CharacterStateComponent::CharacterStateComponent()
    : state {State::get_state(State::IDLE)}
{
}

void CharacterStateComponent::update(const f32 dt, const Input& input, Entity& entity)
{
    state->handle(input, entity);
    state->update(dt, input, entity);
}


void CharacterStateComponent::set_state(State::Value value, const Input& input, Entity& entity)
{
    auto new_state = State::get_state(value);
    if (state == new_state) {
        return;
    }

    state->exit(entity);
    new_state->enter(input, entity);

    state = new_state;
}

} // namespace jmp

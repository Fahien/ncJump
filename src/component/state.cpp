#include "component/state.h"

#include "entity.h"
#include "input.h"

namespace jmp
{
#define DEF_STATE(NewState)                                                                        \
    class NewState : public State                                                                  \
    {                                                                                              \
    public:                                                                                        \
        NewState();                                                                                \
        void enter(const Input& input, Entity& entity) override;                                   \
        void handle(const Input& input, Entity& entity) override;                                  \
        void update(const f32 dt, const Input& input, Entity& entity) override;                    \
        void exit(Entity& entity) override;                                                        \
    }

DEF_STATE(IdleState);
DEF_STATE(MoveState);
DEF_STATE(JumpUpState);
DEF_STATE(JumpDownState);
DEF_STATE(PushState);

#undef DEF_STATE

State* State::get_state(State::Value state)
{
    static UNIQUE<State> states[] = {
        MK<IdleState>(),
        MK<MoveState>(),
        MK<JumpUpState>(),
        MK<JumpDownState>(),
        MK<PushState>(),
    };

    return states[state].get();
}

IdleState::IdleState()
{
    name = "IDLE";
    value = Value::IDLE;
}

#define CHAR_GFX(e) reinterpret_cast<CharacterGraphicsComponent&>(*e.graphics.value().get())
#define CHAR_STT(e) reinterpret_cast<CharacterStateComponent&>(*e.state.value().get())

void IdleState::enter(const Input& input, Entity& entity)
{
    entity.transform.node->addChildNode(&CHAR_GFX(entity).idle);
}

void can_stop(const Input& input, Entity& entity)
{
    if (input.joystick.move.x == 0.0) {
        CHAR_STT(entity).set_state(State::IDLE, input, entity);
    }
}

void can_move(const Input& input, Entity& entity)
{
    if (input.joystick.move.x != 0.0) {
        CHAR_STT(entity).set_state(State::MOVE, input, entity);
    }
}

void can_jump(const Input& input, Entity& entity)
{
    if (input.joystick.a.just_down) {
        CHAR_STT(entity).set_state(State::JUMP_UP, input, entity);
    }
}

void can_fall(const Input& input, Entity& entity)
{
    // Can fall if there is no platform below
    bool no_obstacle_down = !any(entity.get_physics()->obstacle & DirectionFlags::DOWN);

    bool should_fall =
        no_obstacle_down && entity.get_physics()->body->GetLinearVelocity().y < -0.125f;

    if (should_fall) {
        CHAR_STT(entity).set_state(State::JUMP_DOWN, input, entity);
    }
}

void IdleState::handle(const Input& input, Entity& entity)
{
    can_move(input, entity);
    can_jump(input, entity);
    can_fall(input, entity);
}

void IdleState::update(const f32 dt, const Input& input, Entity& entity)
{
    // Nothing
}

void IdleState::exit(Entity& entity)
{
    entity.transform.node->removeChildNode(&CHAR_GFX(entity).idle);
}

MoveState::MoveState()
{
    name = "MOVE";
    value = Value::MOVE;
}

void MoveState::enter(const Input& input, Entity& entity)
{
    entity.transform.node->addChildNode(&CHAR_GFX(entity).movement);
}

void can_push(const Input& input, Entity& entity)
{
    // Check this entity collisions and find something on the right or on the left
    bool left_obstacle = any(entity.get_physics()->obstacle & DirectionFlags::LEFT);
    bool left_push = input.joystick.move.x < 0.0f;

    bool right_obstacle = any(entity.get_physics()->obstacle & DirectionFlags::RIGHT);
    bool right_push = input.joystick.move.x > 0.0f;

    if ((left_obstacle && left_push) || (right_obstacle && right_push)) {
        CHAR_STT(entity).set_state(State::PUSH, input, entity);
    }
}

void MoveState::handle(const Input& input, Entity& entity)
{
    can_stop(input, entity);
    can_jump(input, entity);
    can_fall(input, entity);
    can_push(input, entity);
}

void can_move_on_x(const Input& input, Entity& entity, f32 x_factor)
{
    f32 x_velocity = entity.get_physics()->body->GetLinearVelocity().x;

    // Applying a force to move in the opposite direction of current velocity is always allowed
    bool opposite_move = (input.joystick.move.x < 0 && x_velocity >= 0) ||
        (input.joystick.move.x >= 0 && x_velocity < 0);

    // Make sure current velocity is within limits, otherwise do not apply further force
    bool within_limit =
        fabs(entity.get_physics()->body->GetLinearVelocity().x) < entity.get_physics()->max_x_speed;

    if (opposite_move || within_limit) {
        auto force = b2Vec2(x_factor * input.joystick.move.x, 0.0f);
        entity.get_physics()->body->ApplyLinearImpulse(
            force, entity.get_physics()->body->GetWorldCenter(), true);
    }
}

void MoveState::update(const f32 dt, const Input& input, Entity& entity)
{
    can_move_on_x(input, entity, entity.get_physics()->velocity_factor);
}

void MoveState::exit(Entity& entity)
{
    entity.transform.node->removeChildNode(&CHAR_GFX(entity).movement);
}

JumpUpState::JumpUpState()
{
    name = "JUMP_UP";
    value = Value::JUMP_UP;
}

void JumpUpState::handle(const Input& input, Entity& entity)
{
    // @todo Double jump?
    if (entity.get_physics()->body->GetLinearVelocity().y <= 0.0f) {
        CHAR_STT(entity).set_state(State::JUMP_DOWN, input, entity);
    }
}

void JumpUpState::enter(const Input& input, Entity& entity)
{
    entity.get_physics()->body->GetFixtureList()->SetFriction(0.0f);

    entity.transform.node->addChildNode(&CHAR_GFX(entity).jump_up);

    auto force = b2Vec2(0.0f, entity.get_physics()->jump_y_factor);
    entity.get_physics()->body->ApplyLinearImpulse(
        force, entity.get_physics()->body->GetWorldCenter(), true);
}

/// @brief Jump higher when jump button is kept down
void can_jump_higher(const Input& input, Entity& entity)
{
    if (input.joystick.a.down) {
        auto force = b2Vec2(0.0f, entity.get_physics()->jump_y_factor / 2.0);
        entity.get_physics()->body->ApplyForceToCenter(force, true);
    }
}

void JumpUpState::update(const f32 dt, const Input& input, Entity& entity)
{
    // Can move a bit
    can_move_on_x(input, entity, entity.get_physics()->jump_x_factor);
    can_jump_higher(input, entity);
}

void JumpUpState::exit(Entity& entity)
{
    entity.get_physics()->body->GetFixtureList()->SetFriction(3.0f);
    entity.transform.node->removeChildNode(&CHAR_GFX(entity).jump_up);
}

JumpDownState::JumpDownState()
{
    name = "JUMP_DOWN";
    value = Value::JUMP_DOWN;
}

void JumpDownState::enter(const Input& input, Entity& entity)
{
    entity.get_physics()->body->GetFixtureList()->SetFriction(0.0f);
    entity.transform.node->addChildNode(&CHAR_GFX(entity).jump_down);
}

void JumpDownState::handle(const Input& input, Entity& entity)
{
    bool obstacle_down = any(entity.get_physics()->obstacle & DirectionFlags::DOWN);
    bool not_falling = (entity.get_physics()->body->GetLinearVelocity().y == 0);

    if (obstacle_down || not_falling) {
        CHAR_STT(entity).set_state(State::MOVE, input, entity);
    }
}

void JumpDownState::update(const f32, const Input& input, Entity& entity)
{
    // Can move a bit
    can_move_on_x(input, entity, entity.get_physics()->jump_x_factor);

    // Make sure it goes down
    if (entity.get_physics()->body->GetLinearVelocity().y > -1.0) {
        entity.get_physics()->body->ApplyForceToCenter({0.0, -100.0f}, true);
    }
}

void JumpDownState::exit(Entity& entity)
{
    auto fixture = entity.get_physics()->body->GetFixtureList();
    fixture->SetFriction(3.0f);

    for (auto edge = entity.get_physics()->body->GetContactList(); edge; edge = edge->next) {
        edge->contact->ResetFriction();
    }

    entity.transform.node->removeChildNode(&CHAR_GFX(entity).jump_down);
}

PushState::PushState()
{
    name = "PUSH";
    value = Value::PUSH;
}

void PushState::enter(const Input& input, Entity& entity)
{
    entity.transform.node->addChildNode(&CHAR_GFX(entity).push);
}

void can_stop_pushing(const Input& input, Entity& entity)
{
    if (!any(entity.get_physics()->obstacle & (DirectionFlags::LEFT | DirectionFlags::RIGHT))) {
        CHAR_STT(entity).set_state(State::MOVE, input, entity);
    }
}

void PushState::handle(const Input& input, Entity& entity)
{
    can_stop(input, entity);
    can_stop_pushing(input, entity);
    can_jump(input, entity);
    can_fall(input, entity);
}

void PushState::update(const f32, const Input& input, Entity& entity)
{
    can_move_on_x(input, entity, entity.get_physics()->velocity_factor);
}

void PushState::exit(Entity& entity)
{
    entity.transform.node->removeChildNode(&CHAR_GFX(entity).push);
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

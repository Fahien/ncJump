#include "component/state.h"

#include "command/command.h"
#include "entity.h"

namespace jmp
{
class IdleState : public State
{
public:
    IdleState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;
};

class MoveState : public State
{
public:
    MoveState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;

    bool moving = true;
};

class JumpUpState : public State
{
public:
    JumpUpState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;
};

class JumpDownState : public State
{
public:
    JumpDownState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;

    bool landed = false;
};

class PushState : public State
{
public:
    PushState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;

    bool moving = true;
};

class PullState : public State
{
public:
    PullState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;

    void destroy_joint(b2World& world);

    bool pulling = true;
    b2Joint* joint = nullptr;
};

class DyingState : public State
{
public:
    DyingState();
    void enter(Entity& entity, const MoveCommand* move = nullptr) override;
    void handle(Entity& entity, const MoveCommand& move) override;
    void update(Entity& entity) override;
    void exit(Entity& entity) override;
};

State& CharacterStateComponent::find_state(State::Value value)
{
    ASSERT_MSG_X(value >= 0 && value < State::MAX, "Invalid state value: %d", value);
    return *states[value];
}

IdleState::IdleState()
{
    name = "IDLE";
    value = Value::IDLE;
}

#define CHAR_GFX(e) reinterpret_cast<CharacterGraphicsComponent&>(*e.get_graphics())

void IdleState::enter(Entity& entity, const MoveCommand* move)
{
    entity.get_graphics()->set_current(State::IDLE);
}

bool close(const b2Vec2& a, const b2Vec2& b)
{
    return closef(a.x, b.x) && closef(a.y, b.y);
}

/// @brief The entities will go idle if it has been quiet for at least two frames.
/// This will avoid problems related to the fact that when entering a new dynamic state
/// it could still be in a state of quiet, waiting for forces to be applied by the new state.
/// @param moving Whether it was moving in the previous frame or not
/// @return Wether the entity is moving now or not
bool can_stop(const bool moving, Entity& entity)
{
    const auto zero = b2Vec2(0.0f, 0.0f);
    bool moving_now = !close(entity.get_physics()->body->GetLinearVelocity(), zero);
    if (!moving && !moving_now) {
        CharacterStateComponent::get(entity).set_state(State::IDLE, entity);
    }
    return moving_now;
}

void can_move(const MoveCommand& move, Entity& entity)
{
    if (!closef(move.x, 0.0f)) {
        CharacterStateComponent::get(entity).set_state(State::MOVE, entity, &move);
    }
}

void can_jump(const MoveCommand& move, Entity& entity)
{
    if (move.jump) {
        CharacterStateComponent::get(entity).set_state(State::JUMP_UP, entity, &move);
    }
}

void can_fall(Entity& entity)
{
    // Can fall if there is no platform below
    bool no_obstacle_down = !any(entity.get_physics()->obstacle & DirectionFlags::DOWN);
    bool falling = entity.get_physics()->body->GetLinearVelocity().y < -0.125f;
    bool should_fall = no_obstacle_down && falling;

    if (should_fall) {
        CharacterStateComponent::get(entity).set_state(State::JUMP_DOWN, entity);
    }
}

bool check_pull(Entity& entity, const std::vector<b2Body*>& obstacles)
{
    for (auto other : obstacles) {
        if (other->GetType() != b2_dynamicBody) {
            continue;
        }

        b2DistanceJointDef joint;
        auto body = entity.get_physics()->body;
        joint.Initialize(body, other, body->GetPosition(), other->GetPosition());
        joint.collideConnected = true;

        CharacterStateComponent::get(entity).set_state(State::PULL, entity);
        auto pull_state =
            reinterpret_cast<PullState&>(CharacterStateComponent::get(entity).get_state());

        pull_state.destroy_joint(*body->GetWorld());
        pull_state.joint = other->GetWorld()->CreateJoint(&joint);

        return true;
    }

    return false;
}

void can_pull(const MoveCommand& move, Entity& entity)
{
    if (!move.pull) {
        return;
    }

    auto& obstacles_dir = entity.get_physics()->obstacles_dir;

    if (check_pull(entity, obstacles_dir[Direction::LEFT])) {
        entity.get_graphics()->get_current_mut()->get_sprite_mut().setFlippedX(true);
        return;
    }

    if (check_pull(entity, obstacles_dir[Direction::RIGHT])) {
        entity.get_graphics()->get_current_mut()->get_sprite_mut().setFlippedX(false);
        return;
    }
}

void IdleState::handle(Entity& entity, const MoveCommand& move)
{
    can_move(move, entity);
    can_jump(move, entity);
    can_pull(move, entity);
}

void IdleState::update(Entity& entity)
{
    can_fall(entity);
}

void IdleState::exit(Entity& entity)
{
}

MoveState::MoveState()
{
    name = "MOVE";
    value = Value::MOVE;
}

void MoveState::enter(Entity& entity, const MoveCommand* move)
{
    entity.get_graphics()->set_current(State::MOVE);
    moving = true;
}

void can_push(const MoveCommand& move, Entity& entity)
{
    // Check this entity collisions and find something on the right or on the left
    bool left_obstacle = any(entity.get_physics()->obstacle & DirectionFlags::LEFT);
    bool left_push = move.x < 0.0f;

    bool right_obstacle = any(entity.get_physics()->obstacle & DirectionFlags::RIGHT);
    bool right_push = move.x > 0.0f;

    if ((left_obstacle && left_push) || (right_obstacle && right_push)) {
        CharacterStateComponent::get(entity).set_state(State::PUSH, entity, &move);
    }
}

void can_move_on_x(const f32 move_x, Entity& entity, f32 x_factor)
{
    f32 x_velocity = entity.get_physics()->body->GetLinearVelocity().x;

    // Applying a force to move in the opposite direction of current velocity is always allowed
    bool opposite_move = (move_x < 0 && x_velocity >= 0) || (move_x >= 0 && x_velocity < 0);

    // Make sure current velocity is within limits, otherwise do not apply further force
    bool within_limit =
        fabs(entity.get_physics()->body->GetLinearVelocity().x) < entity.get_physics()->max_x_speed;

    if (opposite_move || within_limit) {
        auto force = b2Vec2(x_factor * move_x, 0.0f);
        entity.get_physics()->body->ApplyLinearImpulse(
            force, entity.get_physics()->body->GetWorldCenter(), true);
    }
}

void MoveState::handle(Entity& entity, const MoveCommand& move)
{
    can_move_on_x(move.x, entity, entity.get_physics()->velocity_factor);
    can_jump(move, entity);
    can_push(move, entity);
}

void MoveState::update(Entity& entity)
{
    moving = can_stop(moving, entity);
    can_fall(entity);
}

void MoveState::exit(Entity& entity)
{
}

JumpUpState::JumpUpState()
{
    name = "JUMP_UP";
    value = Value::JUMP_UP;
}

/// @brief Jump higher when jump button is kept down
void can_jump_higher(const bool jump, Entity& entity)
{
    if (jump) {
        auto force = b2Vec2(0.0f, entity.get_physics()->jump_y_factor / 2.0);
        entity.get_physics()->body->ApplyForceToCenter(force, true);
    }
}

void JumpUpState::handle(Entity& entity, const MoveCommand& move)
{
    // Can move a bit
    can_move_on_x(move.x, entity, entity.get_physics()->jump_x_factor);
    can_jump_higher(move.jump, entity);

    // @todo Double jump?
}

void JumpUpState::enter(Entity& entity, const MoveCommand* move)
{
    entity.get_physics()->body->GetFixtureList()->SetFriction(0.0f);

    entity.get_graphics()->set_current(State::JUMP_UP);

    auto force = b2Vec2(0.0f, entity.get_physics()->jump_y_factor);
    entity.get_physics()->body->ApplyLinearImpulse(
        force, entity.get_physics()->body->GetWorldCenter(), true);
}

void can_stop_jumping(Entity& entity)
{
    bool going_down = entity.get_physics()->body->GetLinearVelocity().y <= 0.0f;
    if (going_down) {
        CharacterStateComponent::get(entity).set_state(State::JUMP_DOWN, entity);
    }
}

void JumpUpState::update(Entity& entity)
{
    can_stop_jumping(entity);
    can_fall(entity);
}

void JumpUpState::exit(Entity& entity)
{
    entity.get_physics()->body->GetFixtureList()->SetFriction(3.0f);
}

JumpDownState::JumpDownState()
{
    name = "JUMP_DOWN";
    value = Value::JUMP_DOWN;
}

void JumpDownState::enter(Entity& entity, const MoveCommand* move)
{
    landed = false;
    entity.get_physics()->body->GetFixtureList()->SetFriction(0.0f);
    entity.get_graphics()->set_current(State::JUMP_DOWN);
}

void JumpDownState::handle(Entity& entity, const MoveCommand& move)
{
    // Can move a bit
    can_move_on_x(move.x, entity, entity.get_physics()->jump_x_factor);
}

void JumpDownState::update(Entity& entity)
{
    // Wait one frame before stopping falling as it may happen that it just started falling down
    bool landed_now = (entity.get_physics()->body->GetLinearVelocity().y >= -0.001f);
    bool still_landed = landed && landed_now;

    if (still_landed) {
        CharacterStateComponent::get(entity).set_state(State::MOVE, entity);
        return;
    }

    // Make sure it goes down
    if (entity.get_physics()->body->GetLinearVelocity().y > -1.0) {
        entity.get_physics()->body->ApplyForceToCenter({0.0, -100.0f}, true);
    }

    landed = landed_now;
}

void JumpDownState::exit(Entity& entity)
{
    auto fixture = entity.get_physics()->body->GetFixtureList();
    fixture->SetFriction(3.0f);

    for (auto edge = entity.get_physics()->body->GetContactList(); edge; edge = edge->next) {
        edge->contact->ResetFriction();
    }
}

PushState::PushState()
{
    name = "PUSH";
    value = Value::PUSH;
}

void PushState::enter(Entity& entity, const MoveCommand* move)
{
    moving = true;
    entity.get_graphics()->set_current(State::PUSH);
}

void can_stop_pushing(Entity& entity)
{
    if (!any(entity.get_physics()->obstacle & (DirectionFlags::LEFT | DirectionFlags::RIGHT))) {
        CharacterStateComponent::get(entity).set_state(State::MOVE, entity);
    }
}

void PushState::handle(Entity& entity, const MoveCommand& move)
{
    can_move_on_x(move.x, entity, entity.get_physics()->velocity_factor);
    can_jump(move, entity);
    can_pull(move, entity);
}

void PushState::update(Entity& entity)
{
    moving = can_stop(moving, entity);
    can_stop_pushing(entity);
    can_fall(entity);
}

void PushState::exit(Entity& entity)
{
}

PullState::PullState()
{
    name = "PULL";
    value = Value::PULL;
}

void PullState::destroy_joint(b2World& world)
{
    if (joint) {
        world.DestroyJoint(joint);
        joint = nullptr;
    }
}

void PullState::enter(Entity& entity, const MoveCommand* move)
{
    pulling = true;

    // @todo Comment
    auto& gfx = entity.get_graphics();
    auto& idle_anim = gfx->subs[State::IDLE]->get_sprite();
    auto& pull_anim = gfx->subs[State::PULL]->get_sprite_mut();
    pull_anim.setFlippedX(idle_anim.isFlippedX());

    gfx->set_current(State::PULL);
}

void can_stop_pulling(const bool pulling, Entity& entity)
{
    if (!pulling) {
        CharacterStateComponent::get(entity).set_state(State::IDLE, entity);
    }
}

void PullState::handle(Entity& entity, const MoveCommand& move)
{
    can_move_on_x(move.x, entity, entity.get_physics()->velocity_factor);
    can_jump(move, entity);

    if (move.pull) {
        pulling = true;
    }
}

void PullState::update(Entity& entity)
{
    can_stop_pulling(pulling, entity);
    can_fall(entity);

    // reset
    pulling = false;
}

void PullState::exit(Entity& entity)
{
    destroy_joint(*entity.get_physics()->body->GetWorld());
}

DyingState::DyingState()
{
    name = "Dying";
    value = Value::DYING;
}

void DyingState::enter(Entity& entity, const MoveCommand* move)
{
    // Disable physics body while the animation finishes
    if (auto& physics = entity.get_physics()) {
        physics->set_enabled(false);
    }

    entity.get_graphics()->set_current(State::DYING);
}

void DyingState::handle(Entity& entity, const MoveCommand& move)
{
}

void DyingState::update(Entity& entity)
{
    auto& gfx = AnimSubGraphics::into(*entity.get_graphics()->get_current_mut());
    if (gfx.anim.animations()[0].isPaused()) {
        exit(entity);
        entity.set_enabled(false);
    }
}

void DyingState::exit(Entity& entity)
{
    auto& gfx = AnimSubGraphics::into(*entity.get_graphics()->get_current_mut());
    gfx.anim.setFrame(0);
    gfx.anim.setPaused(false);
    entity.get_graphics()->set_current(None);
}

const char* to_str(State& state)
{
    return state.name.data();
}

CharacterStateComponent& CharacterStateComponent::into(StateComponent& s)
{
    return reinterpret_cast<CharacterStateComponent&>(s);
}

CharacterStateComponent& CharacterStateComponent::get(Entity& e)
{
    return into(*e.state);
}

ARRAY<UNIQUE<State>, State::MAX> create_states()
{
    auto ret = ARRAY<UNIQUE<State>, State::MAX>();
    ret.pushBack(MK<IdleState>());
    ret.pushBack(MK<MoveState>());
    ret.pushBack(MK<JumpUpState>());
    ret.pushBack(MK<JumpDownState>());
    ret.pushBack(MK<PushState>());
    ret.pushBack(MK<PullState>());
    ret.pushBack(MK<DyingState>());
    return ret;
}

CharacterStateComponent::CharacterStateComponent()
    : states {create_states()}
{
    reset();
}

void CharacterStateComponent::reset()
{
    state = &find_state(State::IDLE);
}

/// @todo Nothing special to clone even though PullState keeps track of a physics joint which
/// is for a specific interaction. That forces a different approach for state objects which
/// would not involve singletons as we do now with the `get_state` function.
UNIQUE<StateComponent> CharacterStateComponent::clone()
{
    auto ret = MK<CharacterStateComponent>();
    return ret;
}

void CharacterStateComponent::handle(Entity& entity, const MoveCommand& command)
{
    state->handle(entity, command);
}

void CharacterStateComponent::update(Entity& entity)
{
    state->update(entity);
}

void CharacterStateComponent::set_state(State::Value value, Entity& entity, const MoveCommand* move)
{
    if (state) {
        if (state->value == value) {
            return;
        }

        LOGI_X("Exiting state %s", to_str(*state));
        state->exit(entity);
    }

    auto& new_state = find_state(value);
    new_state.enter(entity, move);
    state = &new_state;
}

} // namespace jmp

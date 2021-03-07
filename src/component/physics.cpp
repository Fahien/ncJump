#include "component/physics.h"

#include "config.h"
#include "physics.h"

namespace jmp
{
b2Body* tile_body(b2World& world, const bool dynamic)
{
    auto def = b2BodyDef();
    def.type = dynamic ? b2_dynamicBody : b2_staticBody;
    def.fixedRotation = true;

    auto body = world.CreateBody(&def);

    auto box = b2PolygonShape();
    box.SetAsBox(0.5f, 0.5f);

    auto fixture_def = b2FixtureDef();
    fixture_def.shape = &box;
    fixture_def.density = dynamic ? 16.0f : 0.0f;
    fixture_def.friction = 2.0f;
    body->CreateFixture(&fixture_def);

    return body;
}

b2Body* character_body(b2World& world, const bool dynamic)
{
    auto hero_def = b2BodyDef();
    hero_def.type = dynamic ? b2_dynamicBody : b2_staticBody;
    hero_def.angularDamping = 1024.0f;

    auto body = world.CreateBody(&hero_def);

    auto hero_box = b2CircleShape();
    hero_box.m_radius = 0.48f;

    auto hero_fixture_def = b2FixtureDef();
    hero_fixture_def.shape = &hero_box;
    hero_fixture_def.density = 16.0f;
    hero_fixture_def.friction = 30.0f;

    body->CreateFixture(&hero_fixture_def);
    return body;
}

b2Body* create_body(const PhysicsDef& def, b2World& world)
{
    switch (def.type) {
    case PhysicsType::TILE:
        return tile_body(world, def.dynamic);
    case PhysicsType::CHAR:
        return character_body(world, def.dynamic);
    default:
        ASSERT_MSG(false, "Unknown body type");
        return nullptr;
    }
}

PhysicsComponent::PhysicsComponent(const PhysicsDef& def, PhysicsSystem& system)
    : def {def}
    , body {create_body(def, system.world)}
{
}

PhysicsComponent::PhysicsComponent(const PhysicsComponent& o)
    : def {o.def}
    , body {create_body(def, *o.body->GetWorld())}
    , air_factor {o.air_factor}
    , velocity_factor {o.velocity_factor}
    , jump_y_factor {o.jump_y_factor}
    , jump_x_factor {o.jump_x_factor}
    , max_x_speed {o.max_x_speed}
    , destructible {o.destructible}
{
}

PhysicsComponent& PhysicsComponent::operator=(const PhysicsComponent& o)
{
    def = o.def;
    body = create_body(def, *o.body->GetWorld());
    air_factor = o.air_factor;
    velocity_factor = o.velocity_factor;
    jump_y_factor = o.jump_y_factor;
    jump_x_factor = o.jump_x_factor;
    max_x_speed = o.max_x_speed;
    destructible = o.destructible;

    return *this;
}

PhysicsComponent::PhysicsComponent(PhysicsComponent&& o)
    : def {o.def}
    , body {o.body}
    , obstacle {o.obstacle}
    , air_factor {o.air_factor}
    , velocity_factor {o.velocity_factor}
    , jump_y_factor {o.jump_y_factor}
    , jump_x_factor {o.jump_x_factor}
    , max_x_speed {o.max_x_speed}
    , destructible {o.destructible}
{
    o.body = nullptr;
}

PhysicsComponent& PhysicsComponent::operator=(PhysicsComponent&& o) noexcept
{
    std::swap(def, o.def);
    std::swap(body, o.body);
    std::swap(obstacle, o.obstacle);
    std::swap(air_factor, o.air_factor);
    std::swap(velocity_factor, o.velocity_factor);
    std::swap(jump_y_factor, o.jump_y_factor);
    std::swap(jump_x_factor, o.jump_x_factor);
    std::swap(max_x_speed, o.max_x_speed);
    std::swap(destructible, o.destructible);

    return *this;
}

PhysicsComponent::~PhysicsComponent()
{
    if (body) {
        body->GetWorld()->DestroyBody(body);
    }
}

Vec2f PhysicsComponent::get_position() const
{
    auto bpos = body->GetPosition();
    return Vec2f(bpos.x / def.scale, bpos.y / def.scale);
}

void PhysicsComponent::set_position(const Vec2f& pos)
{
    auto bpos = b2Vec2(pos.x * def.scale, pos.y * def.scale);
    body->SetTransform(bpos, 0);
}

void PhysicsComponent::set_enabled(const bool e)
{
    enabled = e;
#ifdef BOX2D_PRE241
    body->SetActive(enabled);
#else
    body->SetEnabled(enabled);
#endif
}

void PhysicsComponent::update()
{
    assert(body && "Physics component has no body");

    // Clear list of obstacles from previous frame
    for (auto& obstacles : obstacles_dir) {
        obstacles.clear();
    }

    // Update some variables
    obstacle = DirectionFlags::NONE;
    for (auto edge = body->GetContactList(); edge; edge = edge->next) {
        // Suppose the other body is A
        b2Body* other = edge->contact->GetFixtureA()->GetBody();
        auto normal = edge->contact->GetManifold()->localNormal;

        if (other == body) {
            other = edge->contact->GetFixtureB()->GetBody();
            // Weird normal adjustment
            normal = -normal;
        }

        if (normal.x < -0.9f) {
            obstacle |= DirectionFlags::RIGHT;
            obstacles_dir[Direction::RIGHT].emplace_back(other);
        }

        if (normal.x > 0.9f) {
            obstacle |= DirectionFlags::LEFT;
            obstacles_dir[Direction::LEFT].emplace_back(other);
        }

        if (normal.y > 0.9f) {
            obstacle |= DirectionFlags::DOWN;
            obstacles_dir[Direction::DOWN].emplace_back(other);
        }

        if (normal.y < -0.9f) {
            obstacle |= DirectionFlags::UP;
            obstacles_dir[Direction::UP].emplace_back(other);
        }
    }

    // Apply air resistance
    auto vel = -body->GetLinearVelocity();
    auto vel_len = vel.LengthSquared();
    vel.x *= air_factor * vel_len;
    vel.y *= air_factor * vel_len;
    body->ApplyForceToCenter(vel, false);
}

} // namespace jmp

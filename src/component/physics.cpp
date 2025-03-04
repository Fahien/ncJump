#include "component/physics.h"

#include "config.h"
#include "physics.h"

namespace jmp
{
b2BodyId tile_body(b2WorldId world, const bool dynamic)
{
    b2BodyDef def = b2DefaultBodyDef();
    def.type = dynamic ? b2_dynamicBody : b2_staticBody;
    def.fixedRotation = true;

    b2BodyId body = b2CreateBody(world, &def);

    b2Polygon box = b2MakeBox(0.5f, 0.5f);

    b2ShapeDef shape = b2DefaultShapeDef();
    shape.density = dynamic ? 16.0f : 0.0f;
    shape.friction = 0.9f;

    b2ShapeId shapeId = b2CreatePolygonShape(body, &shape, &box);

    return body;
}

b2BodyId character_body(b2WorldId world, const bool dynamic)
{
    b2BodyDef hero_def = b2DefaultBodyDef();
    hero_def.type = dynamic ? b2_dynamicBody : b2_staticBody;
    hero_def.fixedRotation = true;

    b2BodyId body = b2CreateBody(world, &hero_def);

    b2Circle hero_box = b2Circle();
    hero_box.radius = 0.48f;

    b2ShapeDef hero_shape_def = b2DefaultShapeDef();
    hero_shape_def.density = 16.0f;
    hero_shape_def.friction = 0.9f;

    b2ShapeId shapeId = b2CreateCircleShape(body, &hero_shape_def, &hero_box);

    return body;
}

b2BodyId create_body(const PhysicsDef& def, b2WorldId world)
{
    switch (def.type) {
    case PhysicsType::TILE:
        return tile_body(world, def.dynamic);
    case PhysicsType::CHAR:
        return character_body(world, def.dynamic);
    default:
        ASSERT_MSG(false, "Unknown body type");
        return b2_nullBodyId;
    }
}

PhysicsComponent::PhysicsComponent(const PhysicsDef& def, PhysicsSystem& system)
    : def {def}
    , world(system.world)
    , body {create_body(def, system.world)}
    , speed {def.speed}
{
}

PhysicsComponent::PhysicsComponent(const PhysicsComponent& o)
    : def {o.def}
    , world {o.world}
    , body {create_body(def, o.world)}
    , air_factor {o.air_factor}
    , speed {o.speed}
    , jump_y_factor {o.jump_y_factor}
    , jump_x_factor {o.jump_x_factor}
    , max_x_speed {o.max_x_speed}
    , friction {o.friction}
    , destructible {o.destructible}
{
}

PhysicsComponent& PhysicsComponent::operator=(const PhysicsComponent& o)
{
    def = o.def;
    world = o.world;
    body = create_body(def, o.world);
    air_factor = o.air_factor;
    speed = o.speed;
    jump_y_factor = o.jump_y_factor;
    jump_x_factor = o.jump_x_factor;
    max_x_speed = o.max_x_speed;
    friction = o.friction;
    destructible = o.destructible;

    return *this;
}

PhysicsComponent::PhysicsComponent(PhysicsComponent&& o)
    : def {o.def}
    , world {o.world}
    , body {o.body}
    , obstacle {o.obstacle}
    , air_factor {o.air_factor}
    , speed {o.speed}
    , jump_y_factor {o.jump_y_factor}
    , jump_x_factor {o.jump_x_factor}
    , max_x_speed {o.max_x_speed}
    , friction {o.friction}
    , destructible {o.destructible}
{
    o.world = b2_nullWorldId;
    o.body = b2_nullBodyId;
}

PhysicsComponent& PhysicsComponent::operator=(PhysicsComponent&& o) noexcept
{
    std::swap(def, o.def);
    std::swap(world, o.world);
    std::swap(body, o.body);
    std::swap(obstacle, o.obstacle);
    std::swap(air_factor, o.air_factor);
    std::swap(speed, o.speed);
    std::swap(jump_y_factor, o.jump_y_factor);
    std::swap(jump_x_factor, o.jump_x_factor);
    std::swap(max_x_speed, o.max_x_speed);
    std::swap(friction, o.friction);
    std::swap(destructible, o.destructible);

    return *this;
}

PhysicsComponent::~PhysicsComponent()
{
    if (B2_IS_NON_NULL(body)) {
        b2DestroyBody(body);
        body = b2_nullBodyId;
    }
}

Vec2f PhysicsComponent::get_position() const
{
    const b2Vec2 bpos = b2Body_GetPosition(body);
    return Vec2f(bpos.x / def.scale, bpos.y / def.scale);
}

void PhysicsComponent::set_position(const Vec2f& pos)
{
    const b2Vec2 bpos = {pos.x * def.scale, pos.y * def.scale};
    b2Body_SetTransform(body, bpos, b2Rot_identity);
}

void PhysicsComponent::set_friction(float f)
{
    friction = f;
    b2ShapeId shape = b2_nullShapeId;
    const int numShapes = b2Body_GetShapes(body, &shape, 1);
    ASSERT(numShapes == 1);
    b2Shape_SetFriction(shape, friction);
}

void PhysicsComponent::set_enabled(const bool e)
{
    enabled = e;
    enabled ? b2Body_Enable(body) : b2Body_Disable(body);
}

void PhysicsComponent::update(const b2ContactData &contact)
{
    b2BodyId other = b2Shape_GetBody(contact.shapeIdB);
    ASSERT(B2_IS_NON_NULL(body));

    const b2Vec2 normal = get_normal(contact);

    // Skip abnormal values
    if (normal.x > 1.0 || normal.y > 1.0 || normal.x < -1.0 || normal.y < -1.0) {
        return;
    }

    const f32 threshold = 0.80f;

    if (normal.x < -threshold) {
        obstacle |= DirectionFlags::RIGHT;
        obstacles_dir[Direction::RIGHT].emplace_back(other);
    }

    if (normal.x > threshold) {
        obstacle |= DirectionFlags::LEFT;
        obstacles_dir[Direction::LEFT].emplace_back(other);
    }

    if (normal.y > threshold) {
        obstacle |= DirectionFlags::DOWN;
        obstacles_dir[Direction::DOWN].emplace_back(other);
    }

    if (normal.y < -threshold) {
        obstacle |= DirectionFlags::UP;
        obstacles_dir[Direction::UP].emplace_back(other);
    }
}

void PhysicsComponent::reset()
{
    // Clear list of obstacles from previous frame
    for (auto& obstacles : obstacles_dir) {
        obstacles.clear();
    }
    obstacle = DirectionFlags::NONE;
}

void PhysicsComponent::update()
{
    reset();

    b2ContactData contacts[16];
    const int numContacts = b2Body_GetContactData(body, contacts, 16);
    for (i32 i = 0; i < numContacts; i++) {
        update(contacts[i]);
    }

    // Apply air resistance
    b2Vec2 vel = -b2Body_GetLinearVelocity(body);
    const float vel_len = b2LengthSquared(vel);
    vel.x *= air_factor * vel_len;
    vel.y *= air_factor * vel_len;
    b2Body_ApplyForceToCenter(body, vel, false);
}

b2Vec2 PhysicsComponent::get_normal(const b2ContactData& contact) const
{
    const b2Vec2 normal = contact.manifold.normal;

    // Box2D specific check needed to get the correct normal
    b2ShapeId shape = b2_nullShapeId;
    b2Body_GetShapes(body, &shape, 1);
    if (B2_ID_EQUALS(contact.shapeIdA, shape)) {
        return -normal;
    }

    return normal;
}

} // namespace jmp

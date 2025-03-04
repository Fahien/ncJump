#include <algorithm>

#include "system/physics/destruction.h"

#include "config.h"
#include "entity.h"
#include "tilemap.h"

// @todo Remove once game has been bypassed in tilemap
#include "game.h"

namespace jmp
{
DestructionListener::DestructionListener(Config& config, nc::SceneNode& scene, nc::Texture& texture)
    : config {config}
    , particles {&scene, 256, &texture}
{
    particles.setLayer(3);
    particles.setInLocalSpace(true);
}

void DestructionListener::setWorld(b2WorldId worldId)
{
    world = worldId;
}

void DestructionListener::check_kill(const b2ContactData& contact, b2ShapeId a, b2ShapeId b)
{
    Entity* player = nullptr;
    Entity* enemy = nullptr;

    b2Vec2 normal = contact.manifold.normal;

    if (Entity::is_player(Entity::from(a))) {
        normal = -normal;
        player = &Entity::from(a);
        enemy = &Entity::from(b);
    } else {
        player = &Entity::from(b);
        enemy = &Entity::from(a);
    }

    if (normal.y > 0.2f) {
        // Kill enemy
        player->get_state()->set_state(State::JUMP_UP, *player);
        to_destroy.pushBack(enemy);
    } else {
        // Kill player
        to_destroy.pushBack(player);
    }
}

void DestructionListener::check_destruction(Entity& entity)
{
    if (entity.get_physics()->destructible) {
        to_destroy.pushBack(&entity);
    }
}

b2ShapeId get_entity_or_null_if(const b2ContactBeginTouchEvent& contact, bool (*check)(const Entity&))
{
    b2ShapeId a = contact.shapeIdA;
    if (check(Entity::from(a))) {
        return a;
    }

    b2ShapeId b = contact.shapeIdB;
    if (check(Entity::from(b))) {
        return b;
    }

    return b2_nullShapeId;
}

b2ShapeId get_player_or_null(const b2ContactBeginTouchEvent& contact)
{
    return get_entity_or_null_if(contact, Entity::is_player);
}

b2ShapeId get_enemy_or_null(const b2ContactBeginTouchEvent& contact)
{
    return get_entity_or_null_if(contact, Entity::is_enemy);
}

void DestructionListener::PostSolve()
{
    ASSERT(B2_IS_NON_NULL(world));
    b2ContactEvents contactEvents = b2World_GetContactEvents(world);

    for (i32 i = 0; i < contactEvents.beginCount; i++)
    {
        const b2ContactBeginTouchEvent& hitEvent = contactEvents.beginEvents[i];
        float impulse_factor = 1.0f;

        const b2ShapeId player_shape = get_player_or_null(hitEvent);
        if (B2_IS_NON_NULL(player_shape)) {
           auto& player = Entity::from(player_shape);

            // If the player is jumping, impulse is scaled up!
            if (player.get_state()->get_state().value == State::JUMP_UP) {
                impulse_factor = 3.0f;
            }

            const b2ShapeId enemy_shape = get_enemy_or_null(hitEvent);
            if (B2_IS_NON_NULL(enemy_shape)) {
                // Check if we need to kill the enemy or the player
                auto& enemy = Entity::from(enemy_shape);

                b2ContactData contactData;
                b2Shape_GetContactData(enemy_shape, &contactData, 1);

                check_kill(contactData, contactData.shapeIdA, contactData.shapeIdB);

                if (contactData.manifold.points[0].normalImpulse * impulse_factor > 200.0f) {
                    auto& entity_a = Entity::from(contactData.shapeIdA);
                    check_destruction(entity_a);
                    auto& entity_b = Entity::from(contactData.shapeIdB);
                    check_destruction(entity_b);
                }

                return;
            }
        }
    }
}

void DestructionListener::emit_particles(Entity& entity)
{
    nc::ParticleInitializer init;
    init.setAmount(4);
    init.setLife(0.2f, 0.3f);
    auto pos = entity.transform.node->position();
    pos.x += config.size.tile * 0.5f;
    pos.y += config.size.tile * 0.5f;
    init.setPositionAndRadius(pos, 0.0f);
    init.setVelocityAndRadius({0.0f, 0.0f}, 32.0f);

    auto& sprite = entity.get_graphics()->get_current()->get_sprite();
    auto rect = sprite.texRect();
    rect.x += rect.w / 4.0f;
    rect.y += rect.h / 4.0f;
    rect.w /= 2.0f;
    rect.h /= 2.0f;

    particles.setTexRect(rect);
    particles.emitParticles(init);
}

void DestructionListener::update(Tilemap& tilemap)
{
    PostSolve();

    for (auto entity : to_destroy) {
        // Look within tiles grid
        for (i32 i = 0; i < tilemap.tiles.size(); ++i) {
            for (i32 j = 0; j < tilemap.tiles[i].size(); ++j) {
                auto& tile = tilemap.tiles[i][j];
                if (tile.get() == entity) {
                    // Remove this tile
                    // @todo Bypass game, by storing the tileset in the tilemap
                    emit_particles(*entity);
                    tilemap.set_tile({i, j}, tilemap.game->tileset, Tile());

                    break;
                }
            }
        }

        // Check if it is an enemy or the player
        if (Entity::is_enemy(*entity) || Entity::is_player(*entity)) {
            entity->get_state()->set_state(State::DYING, *entity);
            continue;
        }

        // Look within free entities
        auto& entities = tilemap.get_entities();
        auto it = std::find_if(std::begin(entities),
            std::end(entities),
            [entity](UNIQUE<Entity>& e) { return e.get() == entity; });
        if (it != std::end(entities)) {
            emit_particles(*entity);
            entities.erase(it);
        }
    }
    to_destroy.clear();
}

} // namespace jmp

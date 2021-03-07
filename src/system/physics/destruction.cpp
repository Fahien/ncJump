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

inline b2WorldManifold get_world_manifold(const b2Contact& contact)
{
    b2WorldManifold wm;
    contact.GetWorldManifold(&wm);
    return wm;
}

void DestructionListener::check_kill(const b2Contact& contact, b2Fixture& a, b2Fixture& b)
{
    Entity* player = nullptr;
    Entity* enemy = nullptr;

    auto normal = get_world_manifold(contact).normal;

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

b2Fixture* get_entity_or_null_if(b2Contact& contact, bool (*check)(const Entity&))
{
    auto a = contact.GetFixtureA();
    if (check(Entity::from(*a))) {
        return a;
    }

    auto b = contact.GetFixtureB();
    if (check(Entity::from(*b))) {
        return b;
    }

    return nullptr;
}

b2Fixture* get_player_or_null(b2Contact& contact)
{
    return get_entity_or_null_if(contact, Entity::is_player);
}

b2Fixture* get_enemy_or_null(b2Contact& contact)
{
    return get_entity_or_null_if(contact, Entity::is_enemy);
}

void DestructionListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    float impulse_factor = 1.0f;

    if (auto player_fixture = get_player_or_null(*contact)) {
        auto& player = Entity::from(*player_fixture);

        if (auto enemy_fixture = get_enemy_or_null(*contact)) {
            // Check if we need to kill the enemy or the player
            auto& enemy = Entity::from(*enemy_fixture);
            check_kill(*contact, *contact->GetFixtureA(), *contact->GetFixtureB());
            return;
        }

        // If the player is jumping, impulse is scaled up!
        if (player.get_state()->get_state().value == State::JUMP_UP) {
            impulse_factor = 3.0f;
        }
    }

    if (impulse->normalImpulses[0] * impulse_factor > 200.0f) {
        auto& entity_a = Entity::from(*contact->GetFixtureA());
        check_destruction(entity_a);
        auto& entity_b = Entity::from(*contact->GetFixtureB());
        check_destruction(entity_b);
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
        auto it = std::find_if(std::begin(tilemap.entities),
            std::end(tilemap.entities),
            [entity](UNIQUE<Entity>& e) { return e.get() == entity; });
        if (it != std::end(tilemap.entities)) {
            emit_particles(*entity);
            tilemap.entities.erase(it);
        }
    }
    to_destroy.clear();
}

} // namespace jmp

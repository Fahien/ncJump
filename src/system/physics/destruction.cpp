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

void DestructionListener::check_destruction(b2Fixture& fixture)
{
    auto& entity = Entity::from(fixture);
    if (entity.get_physics()->destructible) {
        to_destroy.pushBack(&entity);
    }
}

Entity* get_player_or_null(b2Contact& contact)
{
    auto& a = Entity::from(*contact.GetFixtureA());
    auto& b = Entity::from(*contact.GetFixtureB());

    if (Entity::is_player(a)) {
        return &a;
    } else if (Entity::is_player(b)) {
        return &b;
    }

    return nullptr;
}

void DestructionListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    float impulse_factor = 1.0f;

    if (auto player = get_player_or_null(*contact)) {
        bool jumping =
            CharacterStateComponent::into(*player->state->get()).state->value == State::JUMP_UP;
        impulse_factor = jumping ? 3.0f : 1.0f;
    }

    if (impulse->normalImpulses[0] * impulse_factor > 200.0f) {
        check_destruction(*contact->GetFixtureA());
        check_destruction(*contact->GetFixtureB());
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

    auto& single_graphics = SingleGraphicsComponent::into(**entity.graphics);
    auto rect = single_graphics.sprite->texRect();
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

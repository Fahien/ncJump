#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "serialization/config.h"
#include "serialization/tilemap.h"
#include "serialization/tileset.h"

namespace jmp
{
Tileset create_tileset(nc::Texture& texture)
{
    auto ret = Tileset::from_json(PATH("tileset.json"));
    ret.set_texture(texture);

    return ret;
}

Tilemap create_tilemap(Game& game)
{
    auto ret = Tilemap::from_json(PATH("tilemap.json"));
    ret.set_game(game);
    return ret;
}

Game::Game(Config& config)
    : config {config}
    , root {nc::theApplication().rootNode()}
    , scene {&root}
    , physics {}
    , entity {scene}
    , camera {*this, *entity.transform.node.get()}
    , resource {PATH("img/tile/tileset.png")}
    , tileset {create_tileset(resource)}
    , tilemap {create_tilemap(*this)}
    , editor {*this}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale(config.scale.global);
    scene.setScale(config.scale.scene);

    // @todo Refactor that
    entity.physics = PhysicsComponent::character(physics, entity.transform.node->position());
    entity.graphics = MK<CharacterGraphicsComponent>(entity.transform);
    entity.state = MK<CharacterStateComponent>();
}

Game::~Game()
{
    save(config, PATH("config.json"));
    save(tileset, PATH("tileset.json"));
    save(tilemap, PATH("tilemap.json"));
}

void Game::update(const f32 dt)
{
    // Update game state
    physics.update(dt);

    entity.update(dt, input);
    editor.update();

    // @todo Move this somewhere else? Possibly PhysicsSystem?
    // Update entity from body
    auto& pos = entity.physics->body->GetPosition();
    entity.transform.node->x = config.size.tile * pos.x + config.size.tile / 2.0f;
    entity.transform.node->y = config.size.tile * pos.y + config.size.tile / 2.0f;
    // Update tilemap entities from their bodies
    for (auto& entity : tilemap.entities) {
        auto& pos = entity.physics->body->GetPosition();
        entity.transform.node->x = config.size.tile * pos.x ;
        entity.transform.node->y = config.size.tile * pos.y ;
    }

    input.reset();

    camera.update();
}

} // namespace jmp

#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "serialization/config.h"

namespace jmp
{
Game::Game(Config& config)
    : config {config}
    , root {nc::theApplication().rootNode()}
    , scene {&root}
    , physics {}
    , entity {scene}
    , camera {scene, *entity.transform.node.get()}
    , resource {PATH("img/tile/tileset.png")}
    , tileset {resource, config.size.tile}
    , tilemap {*this, scene, tileset}
    , editor {*this}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale(config.scale.global);
    scene.setScale(config.scale.scene);

    // @todo Refactor that
    entity.physics = PhysicsComponent::character(physics, entity.transform.node->position());
    entity.graphics = MK<CharacterGraphicsComponent>(entity.transform);
    entity.state = MK<CharacterStateComponent>();

    save(config, PATH("config.json"));
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

    input.reset();

    camera.update();
}

} // namespace jmp

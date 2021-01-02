#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

namespace jmp
{
Game::Game(Config& config)
    : config {config}
    , root {nc::theApplication().rootNode()}
    , scene {&root}
    , entity {scene}
    , resource {PATH("img/tile/tileset.png")}
    , tileset {resource, config.size.tile}
    , tilemap {scene, tileset}
    , physics {entity.transform.node.x, entity.transform.node.y}
    , editor {*this}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale(config.scale.global);
    scene.setScale(config.scale.scene);

    // @todo Refactor that
    entity.physics = PhysicsComponent();
    entity.physics->body = physics.hero_body;
}

void Game::update(const f32 dt)
{
    // Update game state
    entity.update(dt, input);
    editor.update();
    physics.update(dt);

    // @todo Move this somewhere else?
    // Update entity from box
    auto& pos = physics.hero_body->GetPosition();
    entity.transform.node.x = config.size.tile * pos.x + config.size.tile / 2.0f;
    entity.transform.node.y = config.size.tile * pos.y + config.size.tile / 2.0f;

    input.reset();
}

} // namespace jmp

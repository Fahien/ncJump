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
    , physics {entity.node.x, entity.node.y}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale({config.scale.global});
    scene.setScale({config.scale.scene});

    // @todo Refactor that
    entity.body = physics.hero_body;
}

void Game::update(const float dt)
{
    // Update game state
    entity.update(dt, input);
    editor.update(*this);
    physics.update(dt);

    // @todo Move this somewhere else?
    // Update entity from box
    auto& pos = physics.hero_body->GetPosition();
    entity.node.x = config.size.tile * pos.x + config.size.tile / 2.0f;
    entity.node.y = config.size.tile * pos.y + config.size.tile / 2.0f;

    input.reset();
}

} // namespace jmp

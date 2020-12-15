#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

namespace jmp
{
Game::Game()
    : root {nc::theApplication().rootNode()}
    , scene {&root}
    , entity {scene}
    , resource {PATH("img/tile/tileset.png")}
    , tileset {resource, 16}
    , tilemap {scene, tileset}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale({4.0f, 4.0f});
    scene.setScale({2.0f, 2.0f});
}

void Game::update(const float dt)
{
    // Update game state
    entity.move(input.move);
    editor.update(*this);
}

} // namespace jmp

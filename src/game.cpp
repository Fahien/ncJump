#include "game.h"

#include <ncine/Application.h>

namespace nc = ncine;

namespace jmp
{
Game::Game()
    : root {&nc::theApplication().rootNode()}
{
    root->setScale({8.0f, 8.0f});
}

void Game::update(const float dt)
{
}

} // namespace jmp

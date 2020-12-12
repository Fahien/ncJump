#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

namespace nc = ncine;

namespace jmp
{
#define PATH(path) nc::fs::joinPath(nc::fs::dataPath(), path).data()

Game::Game()
    : root {&nc::theApplication().rootNode()}
    , texture {PATH("img/font/fonts.png")}
    , sprite {&root, &texture}
{
}

} // namespace jmp

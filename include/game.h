#pragma once

#include <ncine/SceneNode.h>

#include "entity.h"
#include "tileset.h"
#include "tilemap.h"
#include "editor.h"

namespace jmp
{
struct Button {
    bool down = false;
    Vec2i pos = {0, 0};
};

struct Input {
    Button left = {};
    Vec2f move = {0.0, 0.0};
};

class Game
{
public:
    Game();

    void update(float dt);

    Input input;

    nc::SceneNode& root;

    /// Root node of the game scene
    nc::SceneNode scene;

    Entity entity;

    nc::Texture resource;

    Tileset tileset;

    Tilemap tilemap;

    Editor editor;
};

} // namespace jmp

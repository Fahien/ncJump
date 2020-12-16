#pragma once

#include <ncine/SceneNode.h>

#include "physics.h"
#include "config.h"
#include "editor.h"
#include "entity.h"
#include "tilemap.h"
#include "tileset.h"

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
    Game(Config& config);

    void update(f32 dt);

    Config config;

    Input input;

    nc::SceneNode& root;

    /// Root node of the game scene
    nc::SceneNode scene;

    Entity entity;

    nc::Texture resource;

    Tileset tileset;

    Tilemap tilemap;

    Physics physics;

    Editor editor;
};

} // namespace jmp

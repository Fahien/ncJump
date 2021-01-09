#pragma once

#include <ncine/SceneNode.h>

#include "camera.h"
#include "config.h"
#include "editor.h"
#include "entity.h"
#include "input.h"
#include "physics.h"
#include "tilemap.h"
#include "tileset.h"

namespace jmp
{
class Game
{
public:
    Game(Config& config);
    ~Game();

    void update(f32 dt);

    Config config;

    Input input;

    nc::SceneNode& root;

    /// Root node of the game scene
    nc::SceneNode scene;

    Physics physics;

    Entity entity;

    Camera camera;

    nc::Texture resource;

    Tileset tileset;

    Tilemap tilemap;

    Editor editor;
};

} // namespace jmp

#pragma once

#include <ncine/SceneNode.h>

#include "physics.h"
#include "config.h"
#include "editor.h"
#include "entity.h"
#include "input.h"
#include "tilemap.h"
#include "tileset.h"
#include "camera.h"

namespace jmp
{
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

    Camera camera;

    nc::Texture resource;

    Physics physics;

    Tileset tileset;

    Tilemap tilemap;

    Editor editor;
};

} // namespace jmp

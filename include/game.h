#pragma once

#include <ncine/SceneNode.h>

#include "camera.h"
#include "config.h"
#include "editor.h"
#include "entity.h"
#include "factory/entity_factory.h"
#include "factory/graphics_factory.h"
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
    void postUpdate();

    Config config;

    Input input;

    nc::SceneNode& root;

    /// Root node of the game scene
    nc::SceneNode scene;

    /// Generic shared assets
    nc::Texture resource;

    PhysicsSystem physics;

    GraphicsFactory graphics_factory;
    EntityFactory entity_factory;

    Tileset tileset;

    Tilemap tilemap;

    Entity entity;

    Camera camera;

    Editor editor;
};

} // namespace jmp

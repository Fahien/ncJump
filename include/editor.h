#pragma once

#include "types.h"

namespace jmp
{
class Config;
class Game;
class Camera;
class Entity;
struct Input;
class Tileset;
class Tilemap;
class StateComponent;
class PhysicsComponent;
class EntityFactory;

class Editor
{
public:
    Editor(Game& g);

    void update();

private:
    enum class Mode {
        NONE,
        TILE,
        ENTITY,
    };

    void reset();

    void set_mode(Mode mode);
    void set_selected_tile(OPTION<u32> s);
    void set_selected_entity(OPTION<u32> s);

    void update_menu();
    void update_config(Config& config);
    void update_camera(Camera& camera, Config& config);
    void update_state(StateComponent& state);
    void update_physics(PhysicsComponent& physics);
    void update_entity(Entity& entity);
    void update_input(Input& input);
    void update_tileset(Tileset& tileset);
    void update_selected_tile(Tileset& tileset);
    void update_entities(EntityFactory& factory);
    void update_tilemap(Tilemap& tilemap);
    void update_collisions(Tilemap& tilemap);

    void place_selected_tile();
    void place_selected_entity();

    Game& game;

    /// Tilemap place mode
    Mode mode = Mode::NONE;

    /// Current selected entity among the ones in the scene
    OPTION<u32> current_entity = None;

    /// Tile to be placed in the scene
    OPTION<u32> selected_tile = None;
    /// Entity to be placed in the scene
    OPTION<u32> selected_entity = None;

    /// Whether to draw debug shapes for physics bodies
    bool show_bodies = false;
};

} // namespace jmp

#pragma once

#include "types.h"

namespace jmp
{
class Config;
class Game;
class Entity;
struct Input;
class Tileset;
class Tilemap;
class StateComponent;
class PhysicsComponent;

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

    void update_menu();
    void update_config(Config& config);
    void update_state(StateComponent& state);
    void update_physics(PhysicsComponent& physics);
    void update_entity(Entity& entity);
    void update_input(Input& input);
    void update_tileset(Tileset& tileset);
    void update_selected_tile(Tileset& tileset);
    void update_tilemap();
    void place_selected_tile();

    Game& game;

    /// Tilemap place mode
    Mode mode = Mode::NONE;

    /// Negative means no tile selected
    i32 selected_tile = -1;
};

} // namespace jmp

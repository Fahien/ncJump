#pragma once

#include "types.h"

namespace jmp
{
class Game;
class Entity;
class Input;
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
        TILE,
        ENTITY,
    };

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
    Mode mode = Mode::TILE;

    /// Negative means no tile selected
    i32 selected_tile = -1;
};

} // namespace jmp

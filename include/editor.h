#pragma once

#include "types.h"

namespace jmp
{
class Game;
class Entity;
class Input;
class Tileset;
class Tilemap;

class Editor
{
public:
    Editor();

    void update(Game& g);

private:
    void update_entity(Entity& entity);
    void update_input(Input& input);
    void update_tileset(Tileset& tileset);
    void update_tilemap(Game& game);

    /// Negative means no tile selected
    i32 selected_tile = -1;
};

} // namespace jmp

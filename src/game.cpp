#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

#include "command/command.h"
#include "serialization/config.h"
#include "serialization/tilemap.h"
#include "serialization/tileset.h"

namespace jmp
{
Tileset create_tileset(nc::Texture& texture)
{
    auto ret = Tileset::from_json(PATH("tileset.json"));
    ret.set_texture(texture);

    return ret;
}

Tilemap create_tilemap(Game& game)
{
    auto ret = Tilemap::from_json(PATH("tilemap.json"));
    ret.set_game(game);
    return ret;
}

Game::Game(Config& config)
    : config {config}
    , root {nc::theApplication().rootNode()}
    , scene {&root}
    , resource {PATH("img/tile/tileset.png")}
    , physics {*this}
    , graphics_factory {}
    , entity_factory {*this}
    , entity {scene}
    , camera {*this, entity.transform.node.get()}
    , tileset {create_tileset(resource)}
    , tilemap {create_tilemap(*this)}
    , editor {*this}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale(config.scale.global);
    scene.setScale(config.scale.scene);

    // @todo Refactor that
    entity.set_physics(
        PhysicsComponent::character(physics.world, entity.transform.node->position()));

    auto graphics = MK<CharacterGraphicsComponent>();
    graphics->idle = graphics_factory.create_animation("img/hero/herochar_idle_anim_strip_4.png");
    graphics->movement =
        graphics_factory.create_animation("img/hero/herochar_run_anim_strip_6.png");
    graphics->jump_up =
        graphics_factory.create_animation("img/hero/herochar_jump_up_anim_strip_3.png");
    graphics->jump_down =
        graphics_factory.create_animation("img/hero/herochar_jump_down_anim_strip_3.png");
    graphics->push =
        graphics_factory.create_animation("img/hero/herochar_pushing_foward_anim_strip_6.png");
    graphics->pull =
        graphics_factory.create_animation("img/hero/herochar_pushing_foward_anim_strip_6.png");
    entity.set_graphics(MV(graphics));
    entity.state = MK<CharacterStateComponent>();
    entity.name = "player";
}

Game::~Game()
{
    save(config, PATH("config.json"));
    save(tileset, PATH("tileset.json"));
    save(tilemap, PATH("tilemap.json"));
}

/// @todo Move this handling somewhere else?
void handle_input(const Input& input, Entity& entity)
{
    if (input.joystick.move.x != 0.0f) {
        entity.command(MK<MoveCommand>(input.joystick.move.x, 0.0f));
    }

    if (input.joystick.a.down) {
        entity.command(MK<MoveCommand>(MoveCommand::Jump()));
    }

    if (input.joystick.x.down) {
        entity.command(MK<MoveCommand>(MoveCommand::Pull()));
    }
}

void Game::update(const f32 dt)
{
    // Update game state
    physics.update(dt, tilemap);

    handle_input(input, entity);
    entity.update(dt, input);

    editor.update();

    // @todo Move this somewhere else? Possibly PhysicsSystem?
    // Update entity from body
    auto& pos = entity.get_physics()->body->GetPosition();
    entity.transform.node->x = config.size.tile * pos.x + config.size.tile / 2.0f;
    entity.transform.node->y = config.size.tile * pos.y + config.size.tile / 2.0f;
    // Update tilemap entities from their bodies
    for (auto& entity : tilemap.entities) {
        if (auto& physics = entity->get_physics()) {
            auto& pos = physics->body->GetPosition();
            entity->transform.node->x = config.size.tile * pos.x;
            entity->transform.node->y = config.size.tile * pos.y;
        }
    }

    input.reset();

    camera.update();
}

} // namespace jmp

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

Entity create_player(nc::SceneNode& parent, Physics& physics, GraphicsFactory& graphics_factory)
{
    auto entity = Entity(parent);

    entity.type = Entity::Type::PLAYER;

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
    graphics->dying = graphics_factory.create_animation(
        "img/hero/herochar_death_anim_strip_8.png", nc::RectAnimation::LoopMode::DISABLED);
    entity.set_graphics(MV(graphics));

    entity.state = MK<CharacterStateComponent>();

    return entity;
}

Game::Game(Config& config)
    : config {config}
    , root {nc::theApplication().rootNode()}
    , scene {&root}
    , resource {PATH("img/tile/tileset.png")}
    , physics {*this}
    , graphics_factory {}
    , entity_factory {*this}
    , tileset {create_tileset(resource)}
    , tilemap {create_tilemap(*this)}
    , entity {create_player(*tilemap.node, physics, graphics_factory)}
    , camera {*this, entity.transform.node.get()}
    , editor {*this}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale(config.scale.global);
    scene.setScale(config.scale.scene);
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
        entity.add_command(MK<MoveCommand>(input.joystick.move.x, 0.0f));
    }

    if (input.joystick.a.down) {
        entity.add_command(MK<MoveCommand>(MoveCommand::Jump()));
    }

    if (input.joystick.x.down) {
        entity.add_command(MK<MoveCommand>(MoveCommand::Pull()));
    }
}

void Game::update(const f32 dt)
{
    // Update game state
    physics.update(dt, tilemap);

    if (entity.is_enabled()) {
        handle_input(input, entity);
    }

    editor.update();

    if (entity.is_enabled()) {
        entity.update(dt);

        // Only for the controlled character, we change its graphics according to the input
        entity.get_graphics()->update(*entity.get_physics(), &input);

        // @todo Move this somewhere else? Possibly PhysicsSystem?
        // Update entity from body
        auto& pos = entity.get_physics()->body->GetPosition();
        entity.transform.node->x = config.size.tile * pos.x;
        entity.transform.node->y = config.size.tile * pos.y;
    }

    // Update tilemap entities from their bodies
    for (auto& entity : tilemap.entities) {
        if (!entity->is_enabled()) {
            continue;
        }

        entity->update(dt);

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

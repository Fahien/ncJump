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

Entity create_player(Config& config,
    Tilemap& tilemap,
    PhysicsSystem& physics,
    GraphicsFactory& graphics_factory)
{
    auto entity = Entity(*tilemap.node);

    entity.type = EntityType::PLAYER;

    entity.transform.node->setPosition(tilemap.initial_position);

    auto phy_def = PhysicsDef();
    phy_def.type = PhysicsType::CHAR;
    phy_def.scale = 1.0f / config.size.tile;
    phy_def.dynamic = true;
    entity.set_physics(PhysicsComponent(phy_def, physics));

    auto gfx_def = GraphicsDef();
    gfx_def.subs.setSize(State::MAX);

    SubGraphicsDef* sub_def = nullptr;

    sub_def = &gfx_def.subs[State::IDLE];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_idle_anim_strip_4.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));

    sub_def = &gfx_def.subs[State::MOVE];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_run_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));

    sub_def = &gfx_def.subs[State::JUMP_UP];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_jump_up_anim_strip_3.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));

    sub_def = &gfx_def.subs[State::JUMP_DOWN];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_jump_down_anim_strip_3.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));

    sub_def = &gfx_def.subs[State::PUSH];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_pushing_foward_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));

    sub_def = &gfx_def.subs[State::PULL];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_idle_anim_strip_4.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));

    sub_def = &gfx_def.subs[State::DYING];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/hero/herochar_death_anim_strip_8.png");
    sub_def->rects = rects_from_stripe(graphics_factory.get_or_create(sub_def->path));
    sub_def->loop = false;

    auto gfx = GraphicsComponent(gfx_def, graphics_factory);
    entity.set_graphics(MV(gfx));

    entity.state = MK<CharacterStateComponent>();

    entity.set_position(tilemap.initial_position);

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
    , entity {create_player(config, tilemap, physics, graphics_factory)}
    , camera {*this, entity.transform.node.get()}
    , editor {*this}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
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
        entity.transform.node->setPosition(entity.get_physics()->get_position());
    }

    // Update tilemap entities from their bodies
    for (auto& entity : tilemap.entities) {
        if (!entity->is_enabled()) {
            continue;
        }

        entity->update(dt);

        if (auto& physics = entity->get_physics()) {
            entity->transform.node->setPosition(physics->get_position());
        }
    }

    input.reset();

    camera.update();
}

} // namespace jmp

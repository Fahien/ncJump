#include "factory/entity_factory.h"

#include <ncine/Texture.h>

#include "component/graphics_component.h"
#include "component/script.h"
#include "entity.h"
#include "game.h"
#include "model/defs.h"

namespace jmp
{
UNIQUE<Entity> make_mushroom(b2World& world, GraphicsFactory& factory)
{
    auto mushroom = MK<Entity>();
    mushroom->add_script(MK<WanderingScript>());
    return mushroom;
}

UNIQUE<Entity> EntityFactory::create(const EntityDef& def,
    GraphicsFactory& graphics_factory,
    PhysicsSystem& physics_system)
{
    return MK<Entity>(def, graphics_factory, physics_system);
}

Entity::Entity(const EntityDef& def,
    GraphicsFactory& graphics_factory,
    PhysicsSystem& physics_system)
    : def {def}
{
    auto graphics_component = GraphicsComponent(def.graphics, graphics_factory);
    set_graphics(MV(graphics_component));

    if (def.physics) {
        set_physics(PhysicsComponent(*def.physics, physics_system));
    }

    if (def.state) {
        set_state(StateComponent());
    }

    for (auto& script : def.scripts) {
        add_script(Script::from(script));
    }

    set_position(def.pos);
}

EntityDef create_mushroom_def(const Config& config, GraphicsFactory& gf)
{
    auto def = EntityDef(EntityType::ENEMY);

    def.graphics.subs.setSize(State::MAX);

    SubGraphicsDef* sub_def = nullptr;

    sub_def = &def.graphics.subs[State::IDLE];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    sub_def = &def.graphics.subs[State::MOVE];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_walk_anim_strip_8.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    sub_def = &def.graphics.subs[State::JUMP_UP];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    sub_def = &def.graphics.subs[State::JUMP_DOWN];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    sub_def = &def.graphics.subs[State::PUSH];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    sub_def = &def.graphics.subs[State::PULL];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    sub_def = &def.graphics.subs[State::DYING];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_death_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));
    sub_def->loop = false;

    auto phy_def = PhysicsDef();
    phy_def.type = PhysicsType::CHAR;
    phy_def.scale = 1.0f / config.size.tile;
    phy_def.dynamic = true;

    def.physics = phy_def;

    def.state = StateDef();

    def.scripts.pushBack(ScriptDef(ScriptType::WANDERING));

    return def;
}

EntityDef EntityFactory::create_player_def(const Config& config, GraphicsFactory& graphics_factory)
{
    auto def = EntityDef(EntityType::PLAYER);

    def.physics = PhysicsDef();
    def.physics->type = PhysicsType::CHAR;
    def.physics->scale = 1.0f / config.size.tile;
    def.physics->dynamic = true;

    auto& gfx_def = def.graphics;
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

    def.state = StateDef();

    return def;
}

void EntityFactory::add(EntityDef def, GraphicsFactory& graphics_factory)
{
    graphics.pushBack(graphics_factory.create(def.graphics.subs[0]));
    entities.pushBack(MV(def));
}

EntityFactory::EntityFactory(Game& game)
{
    add(EntityDef::dummy(), game.graphics_factory);
    add(create_mushroom_def(game.config, game.graphics_factory), game.graphics_factory);

    ASSERT(entities.size());
}

} // namespace jmp

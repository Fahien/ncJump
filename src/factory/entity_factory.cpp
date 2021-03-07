#include "factory/entity_factory.h"

#include <ncine/Texture.h>

#include "component/graphics_component.h"
#include "component/script.h"
#include "model/defs.h"
#include "entity.h"
#include "game.h"

namespace jmp
{
UNIQUE<Entity> make_mushroom(b2World& world, GraphicsFactory& factory)
{
    auto mushroom = MK<Entity>();

    mushroom->type = EntityType::ENEMY;

    mushroom->set_state(StateComponent());

    mushroom->add_script(MK<WanderingScript>());

    return mushroom;
}

UNIQUE<Entity> EntityFactory::create(const EntityDef& def, const Config& config, GraphicsFactory& graphics_factory, PhysicsSystem& physics_system)
{
    auto ret = MK<Entity>();
    ret->type = def.type;

    auto graphics_component = GraphicsComponent(def.graphics, graphics_factory);
    ret->set_graphics(MV(graphics_component));

    if (def.physics) {
        ret->set_physics(PhysicsComponent(*def.physics, physics_system));
    }

    if (def.state) {
        ret->set_state(StateComponent());
    }

    ret->set_position(def.pos);

    return ret;
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

    sub_def = &def.graphics.subs[State::JUMP_DOWN];
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

    return def;
}

void EntityFactory::add(EntityDef def, GraphicsFactory& graphics_factory)
{
    graphics.pushBack(graphics_factory.create(def.graphics.subs[0]));
    entities.pushBack(MV(def));
}

EntityFactory::EntityFactory(Game& game)
{
    add(EntityDef(EntityType::TILE), game.graphics_factory);
    add(create_mushroom_def(game.config, game.graphics_factory), game.graphics_factory);

    ASSERT(entities.size());
}

} // namespace jmp

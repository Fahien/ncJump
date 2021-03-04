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

    mushroom->set_physics(PhysicsComponent::character(world));

    mushroom->state = MK<CharacterStateComponent>();

    mushroom->add_script(MK<WanderingScript>());

    return mushroom;
}

UNIQUE<Entity> EntityFactory::create(const EntityDef& def, const Config& config, GraphicsFactory& graphics_factory)
{
    auto ret = MK<Entity>();

    auto graphics_component = GraphicsComponent(def.graphics, graphics_factory);
    ret->set_graphics(MV(graphics_component));

    ret->set_position(def.pos, config);

    return ret;
}

EntityDef create_mushroom_def(GraphicsFactory& gf)
{
    auto def = EntityDef();
    
    SubGraphicsDef* sub_def = nullptr;

    sub_def = &def.graphics.subs[State::IDLE];
    sub_def->type = GraphicsType::ANIM;
    sub_def->path = String("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    sub_def->rects = rects_from_stripe(gf.get_or_create(sub_def->path));

    return def;
}

void EntityFactory::add(EntityDef def, GraphicsFactory& graphics_factory)
{
    graphics.pushBack(graphics_factory.create(def.graphics.subs[0]));
    entities.pushBack(MV(def));
}

EntityFactory::EntityFactory(Game& game)
{
    add(EntityDef(), game.graphics_factory);
    add(create_mushroom_def(game.graphics_factory), game.graphics_factory);

    ASSERT(entities.size());
}

} // namespace jmp

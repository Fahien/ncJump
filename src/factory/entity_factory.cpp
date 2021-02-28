#include "factory/entity_factory.h"

#include <ncine/Texture.h>

#include "component/graphics.h"
#include "component/script.h"
#include "entity.h"
#include "game.h"

namespace jmp
{
UNIQUE<Entity> make_mushroom(b2World& world, GraphicsFactory& factory)
{
    auto mushroom = MK<Entity>();

    mushroom->type = Entity::Type::ENEMY;

    mushroom->set_physics(PhysicsComponent::character(world));

    auto graphics = MK<CharacterGraphicsComponent>();
    graphics->idle =
        factory.create_animation("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    graphics->movement =
        factory.create_animation("img/enemy/mushroom/mushroom_walk_anim_strip_8.png");
    graphics->jump_down =
        factory.create_animation("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png");
    graphics->dying = factory.create_animation("img/enemy/mushroom/mushroom_death_anim_strip_6.png",
        nc::RectAnimation::LoopMode::DISABLED);
    mushroom->set_graphics(MV(graphics));

    mushroom->state = MK<CharacterStateComponent>();

    mushroom->add_script(MK<WanderingScript>());

    return mushroom;
}

EntityFactory::EntityFactory(Game& game)
{
    entities.pushBack(make_mushroom(game.physics.world, game.graphics_factory));
    assert(entities.size());
}

} // namespace jmp

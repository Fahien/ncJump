#include "factory/entity_factory.h"

#include <ncine/Texture.h>

#include "component/graphics.h"
#include "entity.h"

namespace jmp
{
UNIQUE<Entity> make_mushroom(Physics& physics)
{
    auto mushroom = MK<Entity>();

    mushroom->name = "Mushroom";

    mushroom->set_physics(
        OPTION(PhysicsComponent::character(physics, mushroom->transform.node->position())));

    auto idle_texture = nc::Texture(PATH("img/enemy/mushroom/mushroom_crushed_anim_strip_6.png"));
    mushroom->graphics =
        OPTION(MK<CharacterGraphicsComponent>(mushroom->transform, MV(idle_texture)));

    return mushroom;
}

EntityFactory::EntityFactory(Physics& physics)
    : physics {physics}
{
    entities.pushBack(make_mushroom(physics));
    assert(entities.size());
}

} // namespace jmp

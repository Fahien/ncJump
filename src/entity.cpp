#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

namespace nc = ncine;

namespace jmp
{
#define PATH(path) nc::fs::joinPath(nc::fs::dataPath(), path).data()

Entity::Entity()
    : texture {PATH("img/hero/herochar_idle_anim_strip_4.png")}
    , sprite {&nc::theApplication().rootNode(), &texture}
{
    texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

    sprite.x += 100;
    sprite.y += 50;

    auto anim = nc::RectAnimation(
        0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

    auto sprite_size = nc::Vector2f(texture.width() / 4.0f, texture.height());

    for (int i = 0; i < 4; ++i) {
        anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
    }

    sprite.addAnimation(nctl::makeUnique<nc::RectAnimation>(anim));
    sprite.setPaused(false);
}

} // namespace jmp

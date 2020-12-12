#include "entity.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>

namespace nc = ncine;

namespace jmp
{
const char* to_string(Entity::State state)
{
    switch (state) {
    case Entity::State::IDLE:
        return "IDLE";
    default:
        FATAL_ASSERT_MSG(false, "Failed to get string for character state");
    }
}

#define PATH(path) nc::fs::joinPath(nc::fs::dataPath(), path).data()

Entity::Entity()
    : texture {PATH("img/hero/herochar_idle_anim_strip_4.png")}
    , idle {&nc::theApplication().rootNode(), &texture}
{
    texture.setMagFiltering(nc::Texture::Filtering::NEAREST);

    idle.x += 100;
    idle.y += 50;

    auto anim = nc::RectAnimation(
        0.125, nc::RectAnimation::LoopMode::ENABLED, nc::RectAnimation::RewindMode::FROM_START);

    auto sprite_size = nc::Vector2f(texture.width() / 4.0f, texture.height());

    for (int i = 0; i < 4; ++i) {
        anim.addRect(nc::Recti(i * sprite_size.x, 0.0f, sprite_size.x, sprite_size.y));
    }

    idle.addAnimation(nctl::makeUnique<nc::RectAnimation>(anim));
    idle.setPaused(false);
}

} // namespace jmp

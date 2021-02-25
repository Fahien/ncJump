#include "factory/graphics_factory.h"

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

namespace jmp
{
nc::AnimatedSprite GraphicsFactory::create_animation(const nctl::String& path,
    nc::RectAnimation::LoopMode loop)
{
    // @todo Cache texture
    auto texture = MK<nc::Texture>(PATH(path));
    texture->setMagFiltering(nc::Texture::Filtering::NEAREST);

    assert(texture->width() >= texture->height());
    i32 count = texture->width() / texture->height();

    auto anim = nc::RectAnimation(0.125f, loop, nc::RectAnimation::RewindMode::FROM_START);
    auto sprite_size = Vec2i(texture->width() / count, texture->height());
    for (i32 i = 0; i < count; ++i) {
        anim.addRect(nc::Recti(i * sprite_size.x, 0, sprite_size.x, sprite_size.y));
    }

    auto sprite = nc::AnimatedSprite(nullptr, texture.get());
    sprite.addAnimation(MV(anim));
    sprite.setPaused(false);
    sprite.setLayer(2);

    textures.pushBack(MV(texture));
    return sprite;
}

} // namespace jmp

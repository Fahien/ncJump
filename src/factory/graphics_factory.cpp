#include "factory/graphics_factory.h"

#include <ncine/AnimatedSprite.h>
#include <ncine/Texture.h>

#include "component/graphics_component.h"

namespace jmp
{
nc::Texture& GraphicsFactory::get_or_create(const String& path)
{
    if (auto texture = textures.find(path)) {
        return **texture;
    }

    auto texture = MK<nc::Texture>(PATH(path));
    // Pixel perfect
    texture->setMagFiltering(nc::Texture::Filtering::NEAREST);

    textures.insert(path, MV(texture));
    return **textures.find(path);
}

UNIQUE<SubGraphics> GraphicsFactory::create(const SubGraphicsDef& def)
{
    switch (def.type) {
    case GraphicsType::TILE:
        return MK<StaticSubGraphics>(def, *this);
    case GraphicsType::ANIM:
        return MK<AnimSubGraphics>(def, *this);
    default:
        return {};
    }
}

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

    textures.insert(path, MV(texture));
    return sprite;
}

nc::AnimatedSprite GraphicsFactory::create_anim(const SubGraphicsDef& def)
{
    auto& texture = get_or_create(def.path);

    nc::RectAnimation::LoopMode loop =
        def.loop ? nc::RectAnimation::LoopMode::ENABLED : nc::RectAnimation::LoopMode::DISABLED;
    auto anim = nc::RectAnimation(0.125f, loop, nc::RectAnimation::RewindMode::FROM_START);
    for (auto& rect : def.rects) {
        anim.addRect(rect);
    }

    auto sprite = nc::AnimatedSprite(&texture);
    sprite.addAnimation(MV(anim));
    sprite.setPaused(false);
    sprite.setLayer(def.layer);

    return sprite;
}
} // namespace jmp

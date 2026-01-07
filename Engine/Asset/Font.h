#pragma once

#include "Asset.h"
#include "Entity.h"

#define ASCII_PRINTABLE_ATLAS_SIZE 96
#define ASCII_PRINTABLE_INDEX_OFFSET 32

#define FONT_ATLAS_COLUMNS 16
#define FONT_ATLAS_ROWS 6

namespace font {
    struct Font : asset::Asset {
        asset::Sprite* sprites[ASCII_PRINTABLE_ATLAS_SIZE];
        int8_t postSpacing;
    };

    struct Character {
        render::SpriteInstance* spriteInstance;
        int8_t postSpacing;
    };

    class Text : public entity::Entity {
    public:
        Text() = default;

        Text(
            const std::string& textString,
            const render::SpriteInstance* characterSprites,
            const slurp::Vec2<float>& position
        );
    };

    void loadFontData(Font* font, const asset::Bitmap* bitmap);

    Text createText(
        Font* font,
        const std::string& textString,
        const slurp::Vec2<float>& position,
        int32_t zOrder
    );
}

#include "Font.h"

#include <cstring>

#include "AssetLoader.h"
#include "Bitmap.h"
#include "SpriteInstance.h"

namespace font {
    Text::Text(
        const std::string& textString,
        const render::SpriteInstance* characterSprites,
        const slurp::Vec2<float>& position
    ): Entity(
        textString,
        render::RenderInfo(
            textString.size(),
            characterSprites
        ),
        physics::PhysicsInfo(position),
        collision::CollisionInfo()
    ) {}

    void loadFontData(Font* font, const asset::Bitmap* bitmap) {
        asset::BitmapSheet sheet = sliceBitmap(bitmap, FONT_ATLAS_COLUMNS, FONT_ATLAS_ROWS);

        render::object_id shaderProgramId = asset::loadShaderProgram(
            DEFAULT_SPRITE_VERTEX_SHADER_FILE_NAME,
            DEFAULT_SPRITE_FRAGMENT_SHADER_FILE_NAME
        )->programId;
        asset::Sprite* sprites = memory::AssetLoader->allocateN<asset::Sprite>(sheet.numBitmaps);

        for (int i = 0; i < ASCII_PRINTABLE_ATLAS_SIZE; i++) {
            asset::Sprite* sprite = &sprites[i];
            render::loadSpriteData(sprite, &sheet.bitmaps[i], shaderProgramId);
            font->sprites[i] = sprite;
        }

        font->postSpacing = 0;
        font->isLoaded = true;
    }

    Text createText(Font* font, const std::string& textString, const slurp::Vec2<float>& position, int32_t zOrder) {
        render::SpriteInstance* characterSprites = memory::Permanent->allocateN<render::SpriteInstance>(
            textString.size(),
            true
        );
        for (size_t i = 0; i < textString.size(); i++) {
            uint8_t c = textString[i] - ASCII_PRINTABLE_INDEX_OFFSET;

            new(&characterSprites[i]) render::SpriteInstance(
                font->sprites[c],
                zOrder,
                {static_cast<float>(font->postSpacing), 0.f}
            );
        }

        return Text(textString, characterSprites, position);;
    }
}

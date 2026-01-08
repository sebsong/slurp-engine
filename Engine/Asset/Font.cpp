#include "Font.h"

#include <cstring>

#include "AssetLoader.h"
#include "Bitmap.h"
#include "SpriteInstance.h"

namespace font {
    static uint8_t getCharacterIndex(char character) {
        return static_cast<uint8_t>(character) - ASCII_PRINTABLE_INDEX_OFFSET;
    }

    void Font::setCharacterPostSpacing(char character, int8_t postSpacing) {
        characterPostSpacing[getCharacterIndex(character)] = postSpacing;
    }

    Text::Text(
        Font* font,
        const std::string& textString,
        const render::SpriteInstance* characterSprites,
        const slurp::Vec2<float>& position,
        int32_t zOrder
    ): Entity(
           textString,
           render::RenderInfo(
               textString.size(),
               characterSprites
           ),
           physics::PhysicsInfo(position),
           collision::CollisionInfo()
       ),
       _font(font),
       _zOrder(zOrder) {}

    static render::SpriteInstance* getCharacterSprites(Font* font, const std::string& textString, int32_t zOrder) {
        render::SpriteInstance* characterSprites = memory::Permanent->allocateN<render::SpriteInstance>(
            textString.size()
        );

        float xOffset = 0.f;
        for (size_t i = 0; i < textString.size(); i++) {
            uint8_t c = getCharacterIndex(textString[i]);

            render::SpriteInstance* characterSprite = &characterSprites[i];
            new(characterSprite) render::SpriteInstance(
                font->sprites[c],
                zOrder,
                {xOffset, 0.f}
            );

            xOffset += characterSprite->dimensions.x + font->postSpacing + font->characterPostSpacing[c];
        }

        return characterSprites;
    }

    void Text::setText(const std::string& textString) {
        render::SpriteInstance* characterSprites = getCharacterSprites(_font, textString, _zOrder);
        // TODO: clean up old sprites
        renderInfo.sprites = characterSprites;
        renderInfo.numSprites = textString.size();
    }

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

        font->isLoaded = true;
    }

    Text createText(Font* font, const std::string& textString, const slurp::Vec2<float>& position, int32_t zOrder) {
        render::SpriteInstance* characterSprites = getCharacterSprites(font, textString, zOrder);
        return Text(font, textString, characterSprites, position, zOrder);;
    }
}

#include "Font.h"

#include "AssetLoader.h"
#include "Bitmap.h"
#include "SpriteInstance.h"

namespace font {
    Text::Text(
        std::string&& textString,
        const render::SpriteInstance* characterSprites,
        const slurp::Vec2<float>& position
    ): Entity(
        std::move(textString),
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
            font->characters[i] = Character{
                sprite,
                0
            };
        }

        font->postSpacing = 0;
        font->isLoaded = true;
    }

    Text createText(Font* font, const std::string& textString, const slurp::Vec2<float>& position, int32_t zOrder) {

        for (int i = 0; i < textString.size(); i++) {
            char c = textString[i];
            Character character = font->characters[c];
            character.postSpacing = font->postSpacing;
            text.characters[i] = character;
        }

        return Text(textString);;
    }
}

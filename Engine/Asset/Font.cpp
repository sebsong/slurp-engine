#include "Font.h"

#include "AssetLoader.h"
#include "Bitmap.h"
#include "SpriteInstance.h"

namespace font {
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
}

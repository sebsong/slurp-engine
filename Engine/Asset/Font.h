#pragma once

#include "Asset.h"

#define ASCII_PRINTABLE_ATLAS_SIZE 96
#define ASCII_PRINTABLE_INDEX_OFF 32

#define FONT_ATLAS_COLUMNS 16
#define FONT_ATLAS_ROWS 6

namespace font {
    struct Character {
        asset::Sprite* sprite;
        int postSpacing;
    };

    struct Font : asset::Asset {
        Character characters[ASCII_PRINTABLE_ATLAS_SIZE];
        int postSpacing;
    };

    struct Text {
        types::vector_arena<Character> characters;
    };

    void loadFontData(Font* font, const asset::Bitmap* bitmap);
}

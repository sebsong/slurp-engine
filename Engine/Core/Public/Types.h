#pragma once

                                            //  3 -> 0b00000011
#define IS_SIGN_AND_MAGNITUDE (-1 & 3) == 1 // -1 -> 0b10000001
#define IS_ONES_COMPLEMENT (-1 & 3) == 2    // -1 -> 0b11111110
#define IS_TWOS_COMPLEMENT (-1 & 3) == 3    // -1 -> 0b11111111

namespace slurp {
    typedef unsigned char byte;
    typedef uint32_t bool32;
}

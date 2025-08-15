#pragma once

#include <cstdint>

#include "Debug.h"

/* Negative number representations */
//  3 -> 0b00000011
#define IS_SIGN_AND_MAGNITUDE (-1 & 3) == 1 // -1 -> 0b10000001
#define IS_ONES_COMPLEMENT (-1 & 3) == 2    // -1 -> 0b11111110
#define IS_TWOS_COMPLEMENT (-1 & 3) == 3    // -1 -> 0b11111111

#define BITS_PER_BYTE 8

namespace bit_twiddle {
    inline uint64_t maxSignedValue(uint8_t numBytes) {
        ASSERT(numBytes <= 8);

        return (static_cast<int64_t>(1) << (numBytes * BITS_PER_BYTE - 1)) - 1;
    }

    inline int64_t upsizeInt(int64_t sourceNum, int8_t sourceNumBytes, int8_t targetNumBytes) {
        ASSERT(sourceNumBytes <= targetNumBytes);
        ASSERT(targetNumBytes <= 8);
        ASSERT(IS_TWOS_COMPLEMENT);

        if (sourceNumBytes == targetNumBytes) {
            return sourceNum;
        }

        uint8_t sourceNumBits = sourceNumBytes * BITS_PER_BYTE;
        int64_t sourceSignBitMask = static_cast<int64_t>(1) << (sourceNumBits - 1);
        if (sourceNum & sourceSignBitMask) {
            uint8_t targetNumBits = targetNumBytes * BITS_PER_BYTE;
            int64_t targetTwosComplementMask = ~static_cast<int64_t>(0) << sourceNumBits;
            uint64_t targetSelectorMask =
                    ~static_cast<uint64_t>(0) >> (sizeof(uint64_t) * BITS_PER_BYTE - targetNumBits);
            return (targetTwosComplementMask | sourceNum) & targetSelectorMask;
        }

        return sourceNum;
    }

    inline int64_t multiplyPartialInt(int64_t num, int8_t numBytes, int64_t multiplier) {
        ASSERT(numBytes <= 8);

        if (numBytes == sizeof(num)) {
            return num * multiplier;
        }

        int8_t numBits = numBytes * BITS_PER_BYTE;
        uint64_t targetSelectorMask =
                ~static_cast<uint64_t>(0) >> (sizeof(uint64_t) * BITS_PER_BYTE - numBits);

        return (num * multiplier) & targetSelectorMask;
    }
}

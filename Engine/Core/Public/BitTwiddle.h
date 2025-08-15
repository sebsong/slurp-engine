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

    inline int64_t getSignBitMask(uint8_t numBytes) {
        ASSERT(numBytes <= 8);
        return static_cast<int64_t>(1) << ((numBytes * BITS_PER_BYTE) - 1);
    }

    inline int64_t getSelectorMask(uint8_t numBytes) {
        ASSERT(numBytes <= 8);
        return ~static_cast<uint64_t>(0) >> ((sizeof(uint64_t) - numBytes) * BITS_PER_BYTE);
    }

    inline int64_t upsizeInt(int64_t sourceNum, int8_t sourceNumBytes, int8_t targetNumBytes) {
        ASSERT(sourceNumBytes <= targetNumBytes);
        ASSERT(targetNumBytes <= 8);
        ASSERT(IS_TWOS_COMPLEMENT);

        if (sourceNumBytes == targetNumBytes) {
            return sourceNum;
        }

        uint8_t sourceNumBits = sourceNumBytes * BITS_PER_BYTE;
        int64_t sourceSignBitMask = getSignBitMask(sourceNumBytes);
        if (sourceNum & sourceSignBitMask) {
            int64_t targetTwosComplementMask = ~static_cast<int64_t>(0) << sourceNumBits;
            uint64_t targetSelectorMask = getSelectorMask(targetNumBytes);
            return (targetTwosComplementMask | sourceNum) & targetSelectorMask;
        }

        return sourceNum;
    }

    inline bool isNegativePartial(int64_t num, uint8_t numBytes) {
        ASSERT(numBytes <= sizeof(num));
        int64_t signBitMask = getSignBitMask(numBytes);
        return num & signBitMask;
    }

    inline int64_t negatePartialInt(int64_t num, uint8_t numBytes) {
        ASSERT(numBytes <= sizeof(num));
        ASSERT(IS_TWOS_COMPLEMENT);

        uint64_t selectorMask = getSelectorMask(numBytes);

        int64_t result = num;
        if (isNegativePartial(num, numBytes)) {
            result = ~(result - 1);
        } else {
            result = (~result) + 1;
        }
        return result & selectorMask;
    }

    inline int64_t multiplyPartialInt(const int64_t& num, uint8_t numBytes, const double& multiplier) {
        ASSERT(numBytes <= sizeof(num));

        if (numBytes == sizeof(num)) {
            return num * multiplier;
        }

        int64_t result = num;
        if (isNegativePartial(num, numBytes)) {
            return negatePartialInt(negatePartialInt(result, numBytes) * multiplier, numBytes);
        }
        uint64_t targetSelectorMask = getSelectorMask(numBytes);
        return static_cast<int64_t>(num * multiplier) & targetSelectorMask;
    }
}

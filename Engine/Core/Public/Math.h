#pragma once

namespace math {
    template<typename T>
        requires std::integral<T>
    T round(float num) {
        return static_cast<T>(num + 0.5f);
    }

    inline bool inRange(int n, int min, int max) {
        return min <= n && n <= max;
    }

    template<typename T>
    T getClamped(T n, T min, T max) {
        return std::max(min, std::min(n, max));
    }

    inline double getHypotenuse(float a, float b) {
        return std::sqrt(std::pow(a, 2) + std::pow(b, 2));
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

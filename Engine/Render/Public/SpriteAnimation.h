#pragma once
#include "Asset.h"

namespace asset {
    struct SpriteAnimation : Asset {
        std::string sourceFileName;
        bool isPlaying;
        bool shouldLoop;
        bool playReversed;
        float frameDuration;
        float currentFrameDuration;
        uint8_t numFrames;
        uint8_t currentFrameIndex;
        render::object_id* textureIds;

        void play(bool shouldLoop, float totalDuration, bool playReversed = false);

        void update(float dt);

        void stop();
    };

    void loadSpriteAnimationData(SpriteAnimation* animation, const Bitmap* bitmap, uint8_t numFrames);
}

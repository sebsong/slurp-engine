#include "SpriteAnimation.h"

#include "Bitmap.h"

namespace asset {
    void SpriteAnimation::play(float totalDuration, bool shouldLoop, bool playReversed) {
        stop();
        frameDuration = totalDuration / numFrames;
        this->shouldLoop = shouldLoop;
        if (playReversed) {
            this->playReversed = true;
            currentFrameIndex = numFrames - 1;
        }
        isPlaying = true;
    }

    void SpriteAnimation::update(float dt) {
        if (currentFrameDuration >= frameDuration) {
            currentFrameDuration = 0;
            if (!playReversed) {
                if (currentFrameIndex == numFrames - 1) {
                    if (shouldLoop) {
                        currentFrameIndex = 0;
                    } else {
                        stop();
                    }
                } else {
                    currentFrameIndex++;
                }
            } else {
                if (currentFrameIndex == 0) {
                    if (shouldLoop) {
                        currentFrameIndex = numFrames - 1;
                    } else {
                        stop();
                    }
                } else {
                    currentFrameIndex--;
                }
            }
        }

        currentFrameDuration += dt;
    }

    void SpriteAnimation::stop() {
        isPlaying = false;
        currentFrameIndex = 0;
        currentFrameDuration = 0;
    }

    void loadSpriteAnimationData(SpriteAnimation* animation, const Bitmap* bitmap, uint8_t numFrames) {
        *animation = {};
        animation->numFrames = numFrames;

        BitmapSheet sheet = sliceBitmap(bitmap, numFrames);
        render::object_id* textureIds = memory::AssetLoader->allocateN<render::object_id>(numFrames);
        for (int i = 0; i < numFrames; i++) {
            textureIds[i] = render::createTexture(&sheet.bitmaps[i]);
        }

        animation->textureIds = textureIds;
    }
}

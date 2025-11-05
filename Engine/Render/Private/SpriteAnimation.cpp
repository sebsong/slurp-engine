#include "SpriteAnimation.h"

namespace asset {
    void SpriteAnimation::play(bool shouldLoop, float totalDuration) {
        stop();
        frameDuration = totalDuration / numFrames;
        this->shouldLoop = shouldLoop;
        isPlaying = true;
    }

    void SpriteAnimation::update(float dt) {
        if (currentFrameDuration >= frameDuration) {
            currentFrameDuration = 0;
            currentFrameIndex++;
            if (currentFrameIndex >= numFrames) {
                if (shouldLoop) {
                    currentFrameIndex = 0;
                } else {
                    stop();
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
        render::object_id* textureIds = memory::AssetLoader->allocate<render::object_id>(numFrames);
        for (int i = 0; i < numFrames; i++) {
            textureIds[i] = slurp::Globals->RenderApi->createTexture(&sheet.bitmaps[i]);
        }

        animation->textureIds = textureIds;
    }
}

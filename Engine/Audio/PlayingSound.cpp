#include "PlayingSound.h"

#include "Asset.h"

namespace audio {
    PlayingSound::PlayingSound(
        const asset::Sound* sound,
        const std::function<void()>& onFinish,
        MIX_Track* audioTrack
    ): sound(sound),
       onFinish(onFinish),
       audioTrack(audioTrack),
       isPaused(false),
       isStopped(false) {}
}

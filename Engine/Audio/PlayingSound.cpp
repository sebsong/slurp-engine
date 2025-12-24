#include "PlayingSound.h"

#include "Asset.h"

namespace audio {
    PlayingSound::PlayingSound(
        uint32_t id,
        const asset::Sound* sound,
        const std::function<void()>& onFinish,
        MIX_Track* audioTrack
    ): id(id),
       sound(sound),
       onFinish(onFinish),
       audioTrack(audioTrack),
       isStopped(false) {}

    bool PlayingSound::operator==(const PlayingSound& other) const {
        return id == other.id;
    }
}

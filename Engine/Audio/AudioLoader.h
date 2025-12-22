#pragma once
#include "SDL3_mixer/SDL_mixer.h"

namespace audio {
    class AudioLoader {
    public:
        void init();
        MIX_Audio* loadSound(const char* soundFilePath);

    private:
        MIX_Mixer* mixer;
    };
}

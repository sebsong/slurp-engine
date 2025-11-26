#include "Engine.h"

#include "Logging.h"
#include "Platform.h"
#include "RenderApi.h"
#include "Settings.h"
#include "SlurpEngine.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "MacOS.cpp"

static bool GlobalRunning;

static platform::PlatformDll GlobalPlatformLib;
static render::RenderApi GlobalRenderApi;
static memory::MemoryArena GlobalPermanentMemory;
static memory::MemoryArena GlobalTransientMemory;
static slurp::SlurpDll GlobalSlurpLib;

void tryReloadSlurpLib(const char * libFilePath, const char * libLoadFilePath, slurp::SlurpDll & outSlurpLib) {
    
}

int main(int argc, char* argv[]) {
    if (!SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_IDENTIFIER)) {
        logging::error("Failed to set SDL app metadata.");
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        logging::error("Failed to initialize SDL.");
        return 1;
    }
    if (!SDL_CreateWindow(WINDOW_TITLE, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) {
        logging::error("Failed to create SDL window.");
        return 1;
    }
    GlobalRunning = true;

    std::string libFilePathStr = platform::getLocalFilePath(SLURP_LIB_FILE_NAME);
    const char* libFilePath = libFilePathStr.c_str();
    std::string libLoadFilePathStr = platform::getLocalFilePath(SLURP_LIB_LOAD_FILE_NAME);
    const char* libLoadFilePath = libLoadFilePathStr.c_str();

    platform::loadSlurpLib(libFilePath, GlobalSlurpLib);

    while (GlobalRunning) {
        // Handle Input
        slurp::KeyboardState keyboardState;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP: {
                    SDL_KeyboardEvent keyboardEvent = event.key;
                    SDL_Keycode sdlCode = keyboardEvent.key;

                    if (KeyboardSDLCodeToSlurpCode.contains(sdlCode) > 0) {
                        slurp::KeyboardCode slurpCode = KeyboardSDLCodeToSlurpCode.at(sdlCode);
                        slurp::DigitalInputState& inputState = keyboardState.state[slurpCode];
                        // TODO: update transition count computation if we poll multiple times per frame
                        inputState.transitionCount = keyboardEvent.repeat ? 0 : 1;
                        inputState.isDown = keyboardEvent.down;
                    } else {
                        logging::error(std::format("Keyboard code not registered: {}", SDL_GetKeyName(sdlCode)));
                    }
                    if (event.key.key == SDLK_ESCAPE) {
                        GlobalRunning = false;
                    }
                }
                default: {}
            }
        }
    }
    return 0;
}

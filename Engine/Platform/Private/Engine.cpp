#include "Engine.h"

#include "Logging.h"
#include "Settings.h"
#include "SDL3/SDL.h"

static const char* WINDOW_TITLE = "Slurp's Up!";
static const char* APP_NAME = "SlurpEngine";
static const char* APP_VERSION = "1.0.0";
static const char* APP_IDENTIFIER = "com.slurp.slurpengine";

int main() {
    if (!SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_IDENTIFIER)) {
        logging::error("Failed to set SDL app metadata.");
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        logging::error("Failed to initialize SDL.");
        return 1;
    }
    if (!SDL_CreateWindow(WINDOW_TITLE, DISPLAY_WIDTH, DISPLAY_WIDTH, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) {
        logging::error("Failed to create SDL window.");
        return 1;
    }
    while (true) {
        SDL_PumpEvents();
    }
    return 0;
}

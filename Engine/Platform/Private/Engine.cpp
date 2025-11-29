#include "Engine.h"

#include "Logging.h"
#include "Platform.h"
#include "RenderApi.h"
#include "Settings.h"
#include "SlurpEngine.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "MacOS.cpp"
#if RENDER_API == OPEN_GL
#include "OpenGL.cpp"
#endif

static bool GlobalRunning;

PLATFORM_SHUTDOWN(platform::shutdown) {
    GlobalRunning = false;
}

static bool initSDL(SDL_Window*& outWindow) {
    if (!SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_IDENTIFIER)) {
        logging::error("Failed to set SDL app metadata.");
        return false;
    }
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        logging::error("Failed to initialize SDL.");
        return false;
    }
    SDL_WindowFlags windowFLags{};
#if RENDER_API == OPEN_GL
    windowFLags |= SDL_WINDOW_OPENGL;
#endif

#if FULLSCREEN
    windowFLags |= SDL_WINDOW_FULLSCREEN;
#else
    windowFLags |= SDL_WINDOW_RESIZABLE;
#endif
    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE, DISPLAY_WIDTH, DISPLAY_HEIGHT, windowFLags);
    outWindow = window;

#if HIDE_CURSOR
    SDL_HideCursor();
#endif

#if RENDER_API == OPEN_GL
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        logging::error("Failed to load glad GL loader");
        return false;
    }
#endif

    return true;
}

static void initOpenGL(SDL_Window* window) {}

static void allocateMemoryArenas(memory::MemoryArena& outPermanentMemory, memory::MemoryArena& outTransientMemory) {
    size_t permanentMemorySizeBytes = PERMANENT_ARENA_SIZE;
    size_t transientMemorySizeBytes = TRANSIENT_ARENA_SIZE;
    size_t size = permanentMemorySizeBytes + transientMemorySizeBytes;
    types::byte* memory = platform::allocateMemory(size);
    memory::MemoryArena fullMemory("Full Memory", {memory, size});
    outPermanentMemory = fullMemory.allocateSubArena("Permanent Memory", permanentMemorySizeBytes);
    outTransientMemory = fullMemory.allocateSubArena("Transient Memory", transientMemorySizeBytes);
}

static platform::PlatformDll loadPlatformLib() {
    platform::PlatformDll platformLib = {};
    // platformDll.vibrateGamepad = platform::vibrateGamepad;
    platformLib.shutdown = platform::shutdown;
    // #if DEBUG
    //     platformDll.DEBUG_readFile = platform::DEBUG_readFile;
    //     platformDll.DEBUG_writeFile = platform::DEBUG_writeFile;
    //     platformDll.DEBUG_freeMemory = platform::DEBUG_freeMemory;
    //     platformDll.DEBUG_togglePause = platform::DEBUG_togglePause;
    //     platformDll.DEBUG_beginRecording = platform::DEBUG_beginRecording;
    //     platformDll.DEBUG_endRecording = platform::DEBUG_endRecording;
    //     platformDll.DEBUG_beginPlayback = platform::DEBUG_beginPlayback;
    // #endif
    return platformLib;
}

static render::RenderApi loadRenderApi() {
    render::RenderApi renderApi = {};
    renderApi.setBackgroundColor = open_gl::setBackgroundColor;
    renderApi.createTexture = open_gl::createTexture;
    renderApi.createShaderProgram = open_gl::createShaderProgram;
    renderApi.bindShaderUniformFloat = open_gl::bindShaderUniformFloat;
    renderApi.bindShaderUniformBool = open_gl::bindShaderUniformBool;
    renderApi.genVertexArrayBuffer = open_gl::genVertexArrayBuffer;
    renderApi.genElementArrayBuffer = open_gl::genElementArrayBuffer;
    renderApi.drawVertexArray = open_gl::drawVertexArray;
    renderApi.drawElementArray = open_gl::drawElementArray;
    renderApi.drawPoint = open_gl::drawPoint;
    renderApi.drawLine = open_gl::drawLine;
    renderApi.deleteResources = open_gl::deleteResources;
    return renderApi;
}

int main(int argc, char* argv[]) {
    SDL_Window* window;
    memory::MemoryArena permanentMemory;
    memory::MemoryArena transientMemory;
    platform::PlatformDll platformLib;
    render::RenderApi renderApi;
    slurp::SlurpDll slurpLib;

    if (!initSDL(window)) {
        logging::error("Failed to initialize SDL.");
        return 1;
    }

    allocateMemoryArenas(permanentMemory, transientMemory);
    platformLib = loadPlatformLib();
    renderApi = loadRenderApi();

    std::string libFilePathStr = platform::getLocalFilePath(SLURP_LIB_FILE_NAME);
    const char* libFilePath = libFilePathStr.c_str();
    std::string libLoadFilePathStr = platform::getLocalFilePath(SLURP_LIB_LOAD_FILE_NAME);
    const char* libLoadFilePath = libLoadFilePathStr.c_str();
    slurpLib = platform::loadSlurpLib(libFilePath);
    slurpLib.init(permanentMemory, transientMemory, platformLib, renderApi, false);

    GlobalRunning = true;
    while (GlobalRunning) {
        // Handle Input
        slurp::KeyboardState keyboardState{};
        slurp::MouseState mouseState{};
        slurp::GamepadState gamepadStates[MAX_NUM_GAMEPADS]{};

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
                }
                default: {}
            }
        }
        slurpLib.handleInput(mouseState, keyboardState, gamepadStates);
    }
    return 0;
}

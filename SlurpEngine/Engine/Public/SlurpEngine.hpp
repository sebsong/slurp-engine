#pragma once

#include <Platform.hpp>
#include <DynamicDeclaration.hpp>
#include <Input.hpp>
#include <Audio.hpp>
#include <Update.hpp>
#include <Render.hpp>
#include <Vector.hpp>

#define MAX_NUM_CONTROLLERS 4

namespace slurp
{
    struct GameState
    {
        Tilemap tilemap;
        bool isInitialized;
        ColorPalette colorPalette;
        Entity player;
        Vector2<int> mousePosition;
    };

#if DEBUG
    struct RecordingState
    {
        bool isRecording;
        bool isPlayingBack;
    };
#endif

#define SLURP_INIT(fnName) void fnName(const platform::PlatformDll platformDll, platform::GameMemory* gameMemory)
#define SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT(fnName) void fnName(slurp::MouseState mouseState, slurp::KeyboardState keyboardState, float dt)
#define SLURP_HANDLE_GAMEPAD_INPUT(fnName) void fnName(slurp::GamepadState controllerStates[MAX_NUM_CONTROLLERS], float dt)
#define SLURP_LOAD_AUDIO(fnName) void fnName(slurp::AudioBuffer buffer)
#define SLURP_UPDATE_AND_RENDER(fnName) void fnName(slurp::GraphicsBuffer buffer, float dt)

    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_INIT, init)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_MOUSE_AND_KEYBOARD_INPUT, handleMouseAndKeyboardInput)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_HANDLE_GAMEPAD_INPUT, handleGamepadInput)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_LOAD_AUDIO, loadAudio)
    SLURP_DECLARE_DYNAMIC_DLL_VOID(SLURP_UPDATE_AND_RENDER, updateAndRender)

    struct SlurpDll
    {
        dyn_init* init = stub_init;
        dyn_handleMouseAndKeyboardInput* handleMouseAndKeyboardInput = stub_handleMouseAndKeyboardInput;
        dyn_handleGamepadInput* handleGamepadInput = stub_handleGamepadInput;
        dyn_loadAudio* loadAudio = stub_loadAudio;
        dyn_updateAndRender* updateAndRender = stub_updateAndRender;
    };
}

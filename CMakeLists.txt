cmake_minimum_required(VERSION 3.26)
project(SlurpEngine)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

function(enable target)
    set(${target} 1 PARENT_SCOPE)
    add_compile_definitions(${target})
endfunction()

function(define target value)
    set(${target} ${value} PARENT_SCOPE)
    add_compile_definitions(${target}=${value})
endfunction()

function(define_string target value)
    set(${target} ${value} PARENT_SCOPE)
    add_compile_definitions(${target}="${value}")
endfunction()

# ENGINE
include_directories(Engine/Asset)
include_directories(Engine/Audio)
include_directories(Engine/Core)
include_directories(Engine/Entity)
include_directories(Engine/Input)
include_directories(Engine/Math)
include_directories(Engine/Memory)
include_directories(Engine/Platform)
include_directories(Engine/Render)
include_directories(Engine/Update)
include_directories(Engine/OpenGL)

# GAME
include_directories(Game/Core)
include_directories(Game/Entities)

# EXTERNAL
include_directories(External/SDL-prerelease-3.3.6/include)
include_directories(External/SDL_mixer-commit-82a64d7/include)
include_directories(External/glad/include)
include_directories(External/box2d-3.1.1/include)

set(SDL_SHARED OFF CACHE BOOL "SDL3 shared lib" FORCE)
set(SDL_STATIC ON CACHE BOOL "SDL3 static lib" FORCE)
add_subdirectory(External/SDL-prerelease-3.3.6 EXCLUDE_FROM_ALL)

set(BUILD_SHARED_LIBS FALSE CACHE BOOL "SDL3_Mixer shared lib" FORCE)
set(SDLMIXER_AIFF FALSE CACHE BOOL "SDL3_Mixer AIFF format" FORCE)
set(SDLMIXER_WAVE TRUE CACHE BOOL "SDL3_Mixer WAVE format" FORCE)
set(SDLMIXER_VOC FALSE CACHE BOOL "SDL3_Mixer VOC format" FORCE)
set(SDLMIXER_AU FALSE CACHE BOOL "SDL3_Mixer AU format" FORCE)
set(SDLMIXER_FLAC_LIBFLAC FALSE CACHE BOOL "FLAC audio using libFLAC" FORCE)
set(SDLMIXER_FLAC_DRFLAC FALSE CACHE BOOL "FLAC audio using drflac" FORCE)
set(SDLMIXER_GME FALSE CACHE BOOL "Support loading GME audio via game-music-emu" FORCE)
set(SDLMIXER_MOD_XMP FALSE CACHE BOOL "Support loading MOD audio via libxmp" FORCE)
set(SDLMIXER_MP3_DRMP3 FALSE CACHE BOOL "Support loading MP3 audio via dr_mp3" FORCE)
set(SDLMIXER_MP3_MPG123 FALSE CACHE BOOL "Support loading MP3 audio via libmpg123" FORCE)
set(SDLMIXER_MIDI_TIMIDITY FALSE CACHE BOOL "Support timidity MIDI output" FORCE)
set(SDLMIXER_OPUS FALSE CACHE BOOL "Opus audio via libopus" FORCE)
set(SDLMIXER_VORBIS_STB FALSE CACHE BOOL "Ogg Vorbis audio via stb_vorbis" FORCE)
set(SDLMIXER_VORBIS_VORBISFILE FALSE CACHE BOOL "Ogg Vorbis audio via libvorbisfile" FORCE)
set(SDLMIXER_VORBIS_TREMOR FALSE CACHE BOOL "Ogg Vorbis audio via libvorbisidec ('tremor')" FORCE)
set(SDLMIXER_WAVPACK FALSE CACHE BOOL "WavPack audio" FORCE)
add_subdirectory(External/SDL_mixer-commit-82a64d7 EXCLUDE_FROM_ALL)

set(BOX2D_SAMPLES FALSE CACHE BOOL "Build the Box2D samples" FORCE)
set(BOX2D_BENCHMARKS FALSE CACHE BOOL "Build the Box2D benchmarks" FORCE)
set(BOX2D_DOCS FALSE CACHE BOOL "Build the Box2D documentation" FORCE)
set(BOX2D_PROFILE FALSE CACHE BOOL "Enable profiling with Tracy" FORCE)
set(BOX2D_VALIDATE FALSE CACHE BOOL "Enable heavy validation" FORCE)
set(BOX2D_UNIT_TESTS FALSE CACHE BOOL "Build the Box2D unit tests" FORCE)
add_subdirectory(External/box2d-3.1.1 EXCLUDE_FROM_ALL)

if (WIN32)
    message(DEBUG "Building for Windows.")
    add_compile_definitions(PLATFORM_WINDOWS)
    define_string(LIB_FILE_EXT ".dll")
elseif (APPLE)
    message(DEBUG "Building for Mac.")
    add_compile_definitions(PLATFORM_MAC)
    define_string(LIB_FILE_EXT ".dylib")
elseif (UNIX)
    message(DEBUG "Building for Linux.")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable(DEBUG)
else ()
    enable(RELEASE)
endif ()

define_string(WINDOW_TITLE "Slurp's Up!")
define_string(APP_NAME "SlurpEngine")
define_string(APP_VERSION "1.0.0")
define_string(APP_VERSION_SHORT "1.0")
define_string(APP_IDENTIFIER "com.slurp.slurpengine")

define_string(RECORDING_FILE_NAME "${APP_NAME}.rec")

define_string(ENGINE_EXE_NAME "engine")

if (RELEASE)
    enable(UNITY_BUILD)
endif ()

define(OPEN_GL 1)
define(RENDER_API ${OPEN_GL})

# COMPILER AND LINKER FLAGS
set(COMPILER_FLAGS -fexceptions -finline-functions -std=c++20)
set(WARNING_FLAGS -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wuninitialized -Wno-cast-function-type)
list(APPEND COMPILER_FLAGS ${WARNING_FLAGS})
set(DEBUG_COMPILER_FLAGS -O0 -g)
set(RELEASE_COMPILER_FLAGS -O2)

set(LINKER_FLAGS -ffunction-sections -fdata-sections)
set(DEBUG_LINKER_FLAGS)
set(RELEASE_LINKER_FLAGS)

if (WIN32)
    list(APPEND DEBUG_COMPILER_FLAGS -Wl,-Map=Engine.map)
    list(APPEND LINKER_FLAGS -Wl,--gc-sections)
    list(APPEND RELEASE_LINKER_FLAGS -static)
elseif (APPLE OR UNIX)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND COMPILER_FLAGS ${DEBUG_COMPILER_FLAGS})
    list(APPEND LINKER_FLAGS ${DEBUG_LINKER_FLAGS})
else ()
    list(APPEND COMPILER_FLAGS ${RELEASE_COMPILER_FLAGS})
    list(APPEND LINKER_FLAGS ${RELEASE_LINKER_FLAGS})
endif ()

# ENGINE EXE
set(ENGINE_SOURCE_FILES
        Engine/Platform/Engine.cpp
)
if (NOT UNITY_BUILD)
    list(APPEND ENGINE_SOURCE_FILES
            Engine/Core/SlurpEngine.cpp

            Engine/Asset/AssetLoader.cpp
            Engine/Asset/Bitmap.cpp
            Engine/Audio/AudioPlayer.cpp
            Engine/Audio/PlayingSound.cpp
            Engine/Core/JobRunner.cpp
            Engine/Core/Scene.cpp
            Engine/Core/SpinLock.cpp
            Engine/Core/Timer.cpp
            Engine/Entity/Entity.cpp
            Engine/Entity/EntityPipeline.cpp
            Engine/Memory/MemoryConstructs.cpp
            Engine/Render/Render.cpp
            Engine/Render/RenderInfo.cpp
            Engine/Render/SpriteInstance.cpp
            Engine/Render/SpriteAnimation.cpp
            Engine/Update/Collision.cpp
            Engine/Update/Physics.cpp
            Engine/Update/Update.cpp

            Game/Core/Game.cpp
            Game/Entities/Base.cpp
            Game/Entities/MineSite.cpp
            Game/Entities/MineSiteSpawner.cpp
            Game/Entities/MouseCursor.cpp
            Game/Entities/NumberDisplay.cpp
            Game/Entities/Obstacle.cpp
            Game/Entities/PauseMenu.cpp
            Game/Entities/ProgressBar.cpp
            Game/Entities/SpawnControls.cpp
            Game/Entities/StopwatchDisplay.cpp
            Game/Entities/Turret.cpp
            Game/Entities/UIButton.cpp
            Game/Entities/Worker.cpp
    )

    if (WIN32)
        list(APPEND ENGINE_SOURCE_FILES Engine/Platform/Win32.cpp)
    elseif (APPLE)
        list(APPEND ENGINE_SOURCE_FILES Engine/Platform/MacOS.cpp)
    endif ()

    if (RENDER_API EQUAL OPEN_GL)
        list(APPEND ENGINE_SOURCE_FILES Engine/OpenGL/OpenGL.cpp)
    endif ()
    if (DEBUG)
        list(APPEND ENGINE_SOURCE_FILES
                Engine/Core/Debug.cpp
                Engine/Core/Recording.cpp
        )
    endif ()
endif ()

set(ENGINE_LIBS
        SDL3::SDL3-static
        SDL3_mixer-static
        box2d::box2d
)

add_executable(${ENGINE_EXE_NAME} ${ENGINE_SOURCE_FILES})
target_compile_options(${ENGINE_EXE_NAME} PRIVATE ${COMPILER_FLAGS})
target_link_libraries(${ENGINE_EXE_NAME} PRIVATE ${ENGINE_LIBS})
target_link_options(${ENGINE_EXE_NAME} PRIVATE ${LINKER_FLAGS})
if (APPLE)
    set(BUNDLE_IDENTIFIER ${APP_IDENTIFIER})
    set(BUNDLE_NAME ${APP_NAME})
    set(BUNDLE_VERSION ${APP_VERSION})
    set(BUNDLE_VERSION_SHORT ${APP_VERSION_SHORT})
    set(BUNDLE_VERSION_LONG ${APP_VERSION})
    set(SDL_FILESYSTEM_BASE_DIR_TYPE resource)
    set(INFO_PLIST_FILE_PATH ${CMAKE_SOURCE_DIR}/Info.plist.in)
    set_target_properties(
            ${ENGINE_EXE_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${INFO_PLIST_FILE_PATH}
    )
endif ()

# ASSETS
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Assets/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Assets

        FILES_MATCHING
        PATTERN "*.bmp"
        PATTERN "*.hex"
        PATTERN "*.glsl"
        PATTERN "*.wav"

        PATTERN "*.aesprite" EXCLUDE # Aesprite
        PATTERN "*.flp" EXCLUDE # FL Studio
        PATTERN "*.bfxrsound" EXCLUDE # Bfxr
)

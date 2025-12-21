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
include_directories(Engine/Memory)
include_directories(Engine/Platform)
include_directories(Engine/Render)
include_directories(Engine/Update)
include_directories(Engine/OpenGL)

# GAME
include_directories(Game/Core)
include_directories(Game/Entities)

# EXTERNAL
include_directories(External/SDL-release-3.2.28/include)
include_directories(External/SDL_mixer-commit-82a64d7/include)
include_directories(External/glad/include)
include_directories(External/box2d-3.1.1/include)

set(SDL_SHARED OFF CACHE BOOL "SDL3 shared lib" FORCE)
set(SDL_STATIC ON CACHE BOOL "SDL3 static lib" FORCE)
add_subdirectory(External/SDL-release-3.2.28 EXCLUDE_FROM_ALL)

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

define_string(SLURP_LIB_FILE_NAME "${APP_NAME}${LIB_FILE_EXT}")
define_string(SLURP_LIB_MARKER_FILE_NAME "${SLURP_LIB_FILE_NAME}.marker")
define_string(SLURP_LIB_LOAD_FILE_NAME "${APP_NAME}Load${LIB_FILE_EXT}")
define_string(RECORDING_FILE_NAME "${APP_NAME}.rec")

define_string(ENGINE_EXE_NAME "engine")

if (RELEASE)
    enable(UNITY_BUILD)
endif ()

define(OPEN_GL 1)
define(RENDER_API ${OPEN_GL})

# COMPILER AND LINKER FLAGS
set(compiler_flags -fexceptions -finline-functions -std=c++20)
set(warning_flags -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wuninitialized -Wno-cast-function-type)
list(APPEND compiler_flags ${warning_flags})
set(debug_compiler_flags -O0 -g)
set(release_compiler_flags -O2)

set(linker_flags -ffunction-sections -fdata-sections)
set(debug_linker_flags)
set(release_linker_flags)

if (WIN32)
    list(APPEND debug_compiler_flags -Wl,-Map=Engine.map)
    list(APPEND linker_flags -Wl,--gc-sections)
    list(APPEND release_linker_flags -static)
elseif (APPLE OR UNIX)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND compiler_flags ${debug_compiler_flags})
    list(APPEND linker_flags ${debug_linker_flags})
else ()
    list(APPEND compiler_flags ${release_compiler_flags})
    list(APPEND linker_flags ${release_linker_flags})
endif ()

set(SLURP_SOURCE_FILES
        Engine/Platform/SlurpEngine.cpp
)
if (NOT UNITY_BUILD)
    list(APPEND SLURP_SOURCE_FILES
            Engine/Asset/Asset.cpp
            Engine/Asset/Bitmap.cpp
            Engine/Asset/Wave.cpp
            Engine/Audio/AudioPlayer.cpp
            Engine/Audio/PlayingSound.cpp
            Engine/Core/JobRunner.cpp
            Engine/Core/SpinLock.cpp
            Engine/Core/Timer.cpp
            Engine/Entity/Entity.cpp
            Engine/Entity/EntityPipeline.cpp
            Engine/Memory/MemoryConstructs.cpp
            Engine/Render/Render.cpp
            Engine/Render/RenderInfo.cpp
            Engine/Render/Sprite.cpp
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
    if (DEBUG)
        list(APPEND SLURP_SOURCE_FILES
                Engine/Core/Debug.cpp
                Engine/Core/Recording.cpp
        )
    endif ()
endif ()

# SLURP ENGINE LIBRARY
if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
elseif (APPLE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ENGINE_EXE_NAME}.app/Contents/Resources)
endif ()
add_library(slurp_engine SHARED ${SLURP_SOURCE_FILES})
set_target_properties(slurp_engine PROPERTIES
        OUTPUT_NAME ${APP_NAME}
        PREFIX ""
        SUFFIX ${LIB_FILE_EXT}
)
target_compile_options(slurp_engine PRIVATE ${compiler_flags})
set(slurp_libs box2d::box2d)
target_link_libraries(slurp_engine PRIVATE ${slurp_libs})
target_link_options(slurp_engine PRIVATE ${linker_flags})
add_custom_command(TARGET slurp_engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
add_custom_command(TARGET slurp_engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${SLURP_LIB_MARKER_FILE_NAME}
)

set(ENGINE_SOURCE_FILES
        Engine/Platform/Engine.cpp
)
if (NOT UNITY_BUILD)
    list(APPEND ENGINE_SOURCE_FILES
            Engine/Memory/MemoryConstructs.cpp
            Engine/Core/SpinLock.cpp
    )

    if (WIN32)
        list(APPEND ENGINE_SOURCE_FILES Engine/Platform/Win32.cpp)
    elseif (APPLE)
        list(APPEND ENGINE_SOURCE_FILES Engine/Platform/MacOS.cpp)
    endif ()

    if (RENDER_API EQUAL OPEN_GL)
        list(APPEND ENGINE_SOURCE_FILES Engine/OpenGL/OpenGL.cpp)
    endif ()
endif ()


# ENGINE EXE
add_executable(${ENGINE_EXE_NAME} ${ENGINE_SOURCE_FILES})
target_compile_options(${ENGINE_EXE_NAME} PRIVATE ${compiler_flags})
set(engine_libs SDL3::SDL3-static)
target_link_libraries(${ENGINE_EXE_NAME} PRIVATE ${engine_libs})
target_link_options(${ENGINE_EXE_NAME} PRIVATE ${linker_flags})
if (APPLE)
    set(BUNDLE_IDENTIFIER ${APP_IDENTIFIER})
    set(BUNDLE_NAME ${APP_NAME})
    set(BUNDLE_VERSION ${APP_VERSION})
    set(BUNDLE_VERSION_SHORT ${APP_VERSION_SHORT})
    set(BUNDLE_VERSION_LONG ${APP_VERSION})
    set(SDL_FILESYSTEM_BASE_DIR_TYPE resource)
    set(INFO_PLIST_FILE_PATH ../../Info.plist.in)
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

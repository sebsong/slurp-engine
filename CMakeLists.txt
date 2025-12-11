cmake_minimum_required(VERSION 3.26)
project(SlurpEngine)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

function(include_engine_module target)
    include_directories(Engine/${target}/Public)
    include_directories(Engine/${target}/Private)
endfunction()

function(include_game_module target)
    include_directories(Game/${target}/Public)
    include_directories(Game/${target}/Private)
endfunction()

# ENGINE
include_engine_module(Asset)
include_engine_module(Audio)
include_engine_module(Core)
include_engine_module(Entity)
include_engine_module(Input)
include_engine_module(Memory)
include_engine_module(Platform)
include_engine_module(Render)
include_engine_module(Update)
include_engine_module(OpenGL)

# GAME
include_game_module(Core)
include_game_module(Entities)

# EXTERNAL
include_directories(External/SDL-release-3.2.28/include)
include_directories(External/glad/include)
include_directories(External/box2d-3.1.1/include)

set(SDL_SHARED OFF CACHE BOOL "Disable SDL3 shared lib" FORCE)
set(SDL_STATIC ON CACHE BOOL "Enable SDL3 static lib" FORCE)
add_subdirectory(External/SDL-release-3.2.28 EXCLUDE_FROM_ALL)
add_subdirectory(External/box2d-3.1.1 EXCLUDE_FROM_ALL)

add_subdirectory(Engine/Platform/Private)

# ASSETS
# Install the filtered Assets folder next to the executable
#install(DIRECTORY ${CMAKE_SOURCE_DIR}/Assets/
#        DESTINATION bin/Assets  # Or 'Assets' if exe is in root; adjust to match your exe install
#        FILES_MATCHING
#        PATTERN "*.bmp"  # Sprites and animations
#        PATTERN "*.wav"  # Sounds/music
#        # Add more patterns for other asset types if needed (e.g., PATTERN "*.ogg" for additional audio)
#        EXCLUDE
#        PATTERN "*.psd"   # Photoshop files
#        PATTERN "*.blend" # Blender files
#        PATTERN "*.fla"   # Adobe Animate
#        PATTERN "*.ai"    # Illustrator
#        PATTERN "*.svn"   # Version control dirs
#        PATTERN "*.git"
#        # Add more exclusions for any other editing/temp files in your Assets folder
#)


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
include_directories(SDL-release-3.2.28/include)
include_directories(glad/include)
include_directories(box2d-3.1.1/include)

#ExternalProject_Add(ExternalLibs
#        PREFIX ${CMAKE_BINARY_DIR}/External  # Isolated build dir
#        SOURCE_DIR ${CMAKE_SOURCE_DIR}/External  # Path to external CMakeLists.txt
#        CMAKE_ARGS
#        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}  # Match build type
#        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}  # Install to main project's prefix (e.g., bin/lib/include)
#        -DCMAKE_CXX_STANDARD=20  # Match standards
#        -DSDL_SHARED=OFF
#        -DSDL_STATIC=ON
#        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>  # Force install after build
#        BUILD_ALWAYS OFF  # Skip rebuild if unchanged
#        # Optional: For better dependency tracking
#        BUILD_BYPRODUCTS ${CMAKE_INSTALL_PREFIX}/lib/libSDL3.a
#        ${CMAKE_INSTALL_PREFIX}/lib/libbox2d.a
#)


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


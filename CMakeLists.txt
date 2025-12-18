cmake_minimum_required(VERSION 3.26)
project(SlurpEngine)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

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
include_directories(External/glad/include)
include_directories(External/box2d-3.1.1/include)

set(SDL_SHARED OFF CACHE BOOL "Disable SDL3 shared lib" FORCE)
set(SDL_STATIC ON CACHE BOOL "Enable SDL3 static lib" FORCE)
add_subdirectory(External/SDL-release-3.2.28 EXCLUDE_FROM_ALL)
add_subdirectory(External/box2d-3.1.1 EXCLUDE_FROM_ALL)

add_subdirectory(Engine/Platform)

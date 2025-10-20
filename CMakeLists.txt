cmake_minimum_required(VERSION 3.26)
project(SlurpEngine)
set(CMAKE_CXX_STANDARD 20)

include_directories(Engine/Asset/Public)
include_directories(Engine/Asset/Private)

include_directories(Engine/Audio/Public)
include_directories(Engine/Audio/Private)

include_directories(Engine/Core/Public)
include_directories(Engine/Core/Private)

include_directories(Engine/Input/Public)
include_directories(Engine/Input/Private)

include_directories(Engine/Memory/Public)
include_directories(Engine/Memory/Private)

include_directories(Engine/Platform/Public)
include_directories(Engine/Platform/Private)

include_directories(Engine/Render/Public)
include_directories(Engine/Render/Private)

include_directories(Engine/Update/Public)
include_directories(Engine/Update/Private)

include_directories(Game/Core/Public)
include_directories(Game/Core/Private)

include_directories(Game/Entities/Public)
include_directories(Game/Entities/Private)

include_directories(Engine/OpenGL/Public)
include_directories(Engine/OpenGL/Private)

if(WIN32)
    include_directories(ExternalLibs/glfw-3.4.bin.WIN64/include)
    link_directories(ExternalLibs/glfw-3.4.bin.WIN64/lib-mingw-w64)
elseif(APPLE)
    # Use system GLFW on Mac (installed via Homebrew)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
endif()

include_directories(ExternalLibs/glad/include)

add_subdirectory(Engine/Platform/Private)
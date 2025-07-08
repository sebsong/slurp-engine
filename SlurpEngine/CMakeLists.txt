cmake_minimum_required(VERSION 3.26)
project(SlurpEngine)
set(CMAKE_CXX_STANDARD 20)

include_directories(Engine/Audio/Public)
include_directories(Engine/Audio/Private)

include_directories(Engine/Core/Public)
include_directories(Engine/Core/Private)

include_directories(Engine/Input/Public)
include_directories(Engine/Input/Private)

include_directories(Engine/Platform/Public)
include_directories(Engine/Platform/Private)

include_directories(Engine/Render/Public)
include_directories(Engine/Render/Private)

include_directories(Engine/Update/Public)
include_directories(Engine/Update/Private)

add_subdirectory(Engine/Platform/Private)
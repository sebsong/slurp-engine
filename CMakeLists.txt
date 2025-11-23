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

include_game_module(Core)
include_game_module(Entities)

include_directories(ExternalLibs/glfw-3.4.bin.WIN64/include)
link_directories(ExternalLibs/glfw-3.4.bin.WIN64/lib-mingw-w64)
include_directories(ExternalLibs/glad/include)

add_subdirectory(Engine/Platform/Private)
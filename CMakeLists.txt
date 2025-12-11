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

add_subdirectory(Engine/Platform/Private)

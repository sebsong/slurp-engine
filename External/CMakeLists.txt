cmake_minimum_required(VERSION 3.26)
project(External)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(SDL_SHARED OFF CACHE BOOL "Disable SDL3 shared lib" FORCE)
set(SDL_STATIC ON CACHE BOOL "Enable SDL3 static lib" FORCE)
add_subdirectory(SDL-release-3.2.28)

add_subdirectory(box2d-3.1.1)

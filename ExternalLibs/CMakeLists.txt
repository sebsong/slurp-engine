include_directories(SDL-release-3.2.28/include)
include_directories(glad/include)
include_directories(box2d-3.1.1/include)
target_include_directories(engine PRIVATE glad/include)

set(SDL_SHARED OFF CACHE BOOL "Disable SDL3 shared lib" FORCE)
set(SDL_STATIC ON CACHE BOOL "Enable SDL3 static lib" FORCE)
add_subdirectory(SDL-release-3.2.28)

add_subdirectory(box2d-3.1.1)

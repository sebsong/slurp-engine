# PREPROCESSOR MACROS
if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(APPLE)
    add_compile_definitions(PLATFORM_MACOS)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

# PLATFORM-SPECIFIC CONFIGURATION
if(WIN32)
    # Windows-specific libraries
    set(platform_libs user32.lib gdi32.lib Winmm.lib Shlwapi.lib libksuser.a libglfw3.a)
    set(platform_source WinEngine.cpp ../../Core/Private/FileWatcherWin.cpp)
    set(engine_suffix ".dll")
    set(executable_name win_engine)

    if (MSVC)
        set(compiler_flags -nologo -FC -EHsc -Oi -std:c++20)
        set(warning_flags -WX -W4 -wd4100 -wd4189 -wd4505)
        set(debug_flags -Od -Z7 -FmWinEngine.map -MDd)
        set(release_flags /O2 -MD)
        set(linker_flags -opt:ref)
    else ()
        set(compiler_flags -fexceptions -finline-functions -std=c++20)
        set(warning_flags -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-cast-function-type)
        set(debug_flags -O0 -g -Wl,-Map=WinEngine.map)
        set(release_flags -O2)
        set(linker_flags -ffunction-sections -fdata-sections -Wl,--gc-sections)
    endif ()

elseif(APPLE)
    # macOS-specific libraries
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)

    set(platform_libs
        ${GLFW3_LIBRARIES}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
        "-framework CoreServices"
    )
    link_directories(${SDL2_LIBRARY_DIRS})
    set(platform_source MacEngine.cpp ../../Core/Private/FileWatcherMac.cpp)
    set(engine_suffix ".dylib")
    set(executable_name mac_engine)

    set(compiler_flags -fexceptions -finline-functions -std=c++20)
    set(warning_flags -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-null-dereference)
    set(debug_flags -O0 -g)
    set(release_flags -O2)
    set(linker_flags -ffunction-sections -fdata-sections)

    include_directories(${SDL2_INCLUDE_DIRS})
endif()

set(slurp_libs)
list(APPEND compiler_flags ${warning_flags})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND compiler_flags ${debug_flags})
else ()
    list(APPEND compiler_flags ${release_flags})
endif ()

# SLURP ENGINE LIBRARY
add_library(slurp_engine SHARED SlurpEngine.cpp)
set_target_properties(slurp_engine PROPERTIES
        OUTPUT_NAME "SlurpEngine"
        PREFIX "lib"
        SUFFIX ${engine_suffix}
)
target_compile_options(slurp_engine PRIVATE ${compiler_flags})
target_link_libraries(slurp_engine PRIVATE ${slurp_libs})
if(NOT MSVC)
    target_link_options(slurp_engine PRIVATE ${linker_flags})
endif()

# PLATFORM ENGINE EXECUTABLE
if(WIN32)
    add_executable(${executable_name} WIN32 ${platform_source})
else()
    add_executable(${executable_name} ${platform_source})
endif()

target_compile_options(${executable_name} PRIVATE ${compiler_flags})
target_link_libraries(${executable_name} PRIVATE ${platform_libs} ${SDL2_LIBRARIES})
if(NOT MSVC)
    target_link_options(${executable_name} PRIVATE ${linker_flags})
endif()

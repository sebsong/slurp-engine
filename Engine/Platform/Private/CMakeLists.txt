# PREPROCESSOR MACROS
add_compile_definitions(PLATFORM_WINDOWS)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()
#add_compile_definitions(ASSETS_DIR="C:/Users/sebys/Workspace/slurp-engine/SlurpEngine/Assets/")

# COMPILER AND LINKER FLAGS
set(libs user32.lib gdi32.lib Winmm.lib Shlwapi.lib)
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
        PREFIX ""
        SUFFIX ".dll"
)
target_compile_options(slurp_engine PRIVATE ${compiler_flags})
target_link_options(slurp_engine PRIVATE ${linker_flags})

# WIN ENGINE EXE
add_executable(win_engine WIN32 WinEngine.cpp)
target_link_libraries(win_engine PRIVATE ${libs})
target_compile_options(win_engine PRIVATE ${compiler_flags})
target_link_options(win_engine PRIVATE ${linker_flags})


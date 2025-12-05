function(define target value)
    set(${target} ${value} PARENT_SCOPE)
    add_compile_definitions(${target}="${value}")
endfunction()

if (WIN32)
    message("Building for Windows.")
    add_compile_definitions(PLATFORM_WINDOWS)
    define(LIB_FILE_EXT ".dll")
elseif (APPLE)
    message("Building for Mac.")
    add_compile_definitions(PLATFORM_MAC)
    define(LIB_FILE_EXT ".dylib")
elseif (UNIX)
    message("Building for Linux.")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

define(WINDOW_TITLE "Slurp's Up!")
define(APP_NAME "SlurpEngine")
define(APP_VERSION "1.0.0")
define(APP_VERSION_SHORT "1.0")
define(APP_IDENTIFIER "com.slurp.slurpengine")

define(SLURP_LIB_FILE_NAME "${APP_NAME}${LIB_FILE_EXT}")
define(SLURP_LIB_MARKER_FILE_NAME "${SLURP_LIB_FILE_NAME}.marker")
define(SLURP_LIB_LOAD_FILE_NAME "${APP_NAME}Load${LIB_FILE_EXT}")
define(RECORDING_FILE_NAME "${APP_NAME}.rec")

define(ENGINE_EXE_NAME "engine")

# COMPILER AND LINKER FLAGS
if (WIN32)
    set(slurp_libs)
    if (MSVC)
        set(compiler_flags -nologo -FC -EHsc -Oi -std:c++20)
        set(warning_flags -WX -W4 -wd4100 -wd4189 -wd4505)
        set(debug_flags -Od -Z7 -FmEngine.map -MDd)
        set(release_flags /O2 -MD)

        set(linker_flags -opt:ref)
    else ()
        set(compiler_flags -fexceptions -finline-functions -std=c++20)
        set(warning_flags -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-cast-function-type)
        set(debug_flags -O0 -g -Wl,-Map=Engine.map)
        set(release_flags -O2)

        set(linker_flags -ffunction-sections -fdata-sections -Wl,--gc-sections)
    endif ()
elseif (APPLE OR UNIX)
    set(compiler_flags -fexceptions -finline-functions -std=c++20)
    set(warning_flags -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-cast-function-type)
    set(debug_flags -O0 -g)
    set(release_flags -O2)

    set(linker_flags -ffunction-sections -fdata-sections -Wl)
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
        OUTPUT_NAME ${APP_NAME}
        PREFIX ""
        SUFFIX ${LIB_FILE_EXT}
)
target_compile_options(slurp_engine PRIVATE ${compiler_flags})
target_link_libraries(slurp_engine PRIVATE ${slurp_libs})
target_link_options(slurp_engine PRIVATE ${linker_flags})
if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
elseif (APPLE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${ENGINE_EXE_NAME}.app/Contents/Resources/)
endif ()
add_custom_command(TARGET slurp_engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${SLURP_LIB_MARKER_FILE_NAME}
)

# ENGINE EXE
set(engine_libs SDL3::SDL3-static)
add_executable(${ENGINE_EXE_NAME} Engine.cpp)
target_compile_options(${ENGINE_EXE_NAME} PRIVATE ${compiler_flags})
target_link_libraries(${ENGINE_EXE_NAME} PRIVATE ${engine_libs})
target_link_options(${ENGINE_EXE_NAME} PRIVATE ${linker_flags})

if (APPLE)
    set(BUNDLE_IDENTIFIER ${APP_IDENTIFIER})
    set(BUNDLE_NAME ${APP_NAME})
    set(BUNDLE_VERSION ${APP_VERSION})
    set(BUNDLE_VERSION_SHORT ${APP_VERSION_SHORT})
    set(BUNDLE_VERSION_LONG ${APP_VERSION})
    set(SDL_FILESYSTEM_BASE_DIR_TYPE resource)
    set(INFO_PLIST_FILE_PATH ${PROJECT_SOURCE_DIR}/Info.plist.in)
    set_target_properties(
            ${ENGINE_EXE_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${INFO_PLIST_FILE_PATH}
    )
endif ()

function(define target value)
    set(${target} ${value} PARENT_SCOPE)
    add_compile_definitions(${target}="${value}")
endfunction()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

if (WIN32)
    define(LIB_FILE_EXT ".dll")
elseif (APPLE)
    define(LIB_FILE_EXT ".dylib")
elseif (UNIX)
endif ()
define(WINDOW_TITLE "Slurp's Up!")
define(APP_NAME "SlurpEngine")
define(APP_VERSION "1.0.0")
define(APP_IDENTIFIER "com.slurp.slurpengine")

define(SLURP_LIB_FILE_NAME "${APP_NAME}${LIB_FILE_EXT}")
define(SLURP_LIB_MARKER_FILE_NAME "${SLURP_LIB_FILE_NAME}.marker")
define(SLURP_LIB_LOAD_FILE_NAME "${APP_NAME}Load${LIB_FILE_EXT}")
define(RECORDING_FILE_NAME "${APP_NAME}.rec")

if (WIN32)
    message("Building for Windows")
    # PREPROCESSOR MACROS
    add_compile_definitions(PLATFORM_WINDOWS)
    # COMPILER AND LINKER FLAGS
    set(win_libs SDL3::SDL3-static)
    set(slurp_libs)
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
            OUTPUT_NAME ${APP_NAME}
            PREFIX ""
            SUFFIX ${LIB_FILE_EXT}
    )
    target_compile_options(slurp_engine PRIVATE ${compiler_flags})
    target_link_libraries(slurp_engine PRIVATE ${slurp_libs})
    target_link_options(slurp_engine PRIVATE ${linker_flags})
    add_custom_command(TARGET slurp_engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E touch ${SLURP_LIB_MARKER_FILE_NAME}
    )

    # WIN ENGINE EXE
    add_executable(engine WIN32 Engine.cpp)
    target_compile_options(engine PRIVATE ${compiler_flags})
    target_link_libraries(engine PRIVATE ${win_libs})
    target_link_options(engine PRIVATE ${linker_flags})

elseif (APPLE)
    message("Building for Mac")
    add_compile_definitions(PLATFORM_MAC)

    # COMPILER AND LINKER FLAGS
    set(mac_libs SDL3::SDL3-static)
    set(compiler_flags -fexceptions -finline-functions -std=c++20)
    set(warning_flags -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-cast-function-type)
    set(debug_flags -O0 -g)
    set(release_flags -O2)

    set(linker_flags -ffunction-sections -fdata-sections -Wl)

    list(APPEND compiler_flags ${warning_flags})
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND compiler_flags ${debug_flags})
    else ()
        list(APPEND compiler_flags ${release_flags})
    endif ()

    # SLURP ENGINE LIBRARY
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/engine.app/Contents/MacOS/)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    add_library(slurp_engine SHARED SlurpEngine.cpp)
    set_target_properties(slurp_engine PROPERTIES
            OUTPUT_NAME ${APP_NAME}
            PREFIX ""
            SUFFIX ${LIB_FILE_EXT}
    )
    target_compile_options(slurp_engine PRIVATE ${compiler_flags})
    target_link_libraries(slurp_engine PRIVATE ${slurp_libs})
    target_link_options(slurp_engine PRIVATE ${linker_flags})
    add_custom_command(TARGET slurp_engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/${SLURP_LIB_MARKER_FILE_NAME}
    )

    # MAC ENGINE EXE
    add_executable(engine Engine.cpp)
    target_compile_options(engine PRIVATE ${compiler_flags})
    target_link_libraries(engine PRIVATE ${mac_libs})
    target_link_options(engine PRIVATE ${linker_flags})
    set_target_properties(engine PROPERTIES MACOSX_BUNDLE TRUE MACOSX_BUNDLE_GUI_IDENTIFIER APP_IDENTIFIER)
elseif (UNIX)
    message("Linux not supported (yet?)")
endif ()


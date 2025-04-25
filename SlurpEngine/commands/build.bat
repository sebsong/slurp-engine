@echo off

SETLOCAL

set includes=-I %engine_dir%\Public\ -I %engine_dir%\Private\
set warning_flags=-WX -W4 -wd4100 -wd4189 -wd4505
set macros=-D PLATFORM_WINDOWS
set compiler_flags=-nologo -FC -EHsc -MT -Oi

set debug_flags=-Od -Z7 -FmWinEngine.map 
set release_flags=-O2

if "%~1" == "" (
    set debug=1
) else (
    set debug=%1
)
if %debug% == 1 (
    set macros=%macros% -D DEBUG
    set compiler_flags=%debug_flags% %compiler_flags%
) else (
    set compiler_flags=%release_flags% %compiler_flags%
)

set linker_flags=/link -opt:ref
set libs=user32.lib gdi32.lib

if not exist %slurp_dir%\build mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl %includes% %warning_flags% %macros% %compiler_flags% %engine_dir%\Private\WinEngine.cpp %linker_flags% %libs%
popd

ENDLOCAL

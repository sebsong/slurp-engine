@echo off

SETLOCAL

set includes=-I %engine_dir%\Public\ -I %engine_dir%\Private\
set warnings=-WX -W4 -wd4100 -wd4189 -wd4505
set macros=-D PLATFORM_WINDOWS
set linker=/link -opt:ref
set libs=user32.lib gdi32.lib

if "%~1" == "" (
    set debug=1
) else (
    set debug=%1
)

if %debug% == 1 (
    set macros=%macros% -D DEBUG
)

if not exist %slurp_dir%\build mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl -nologo -FC -FmWinEngine.map -Z7 -EHsc -MT -Oi %includes% %warnings% %macros% %engine_dir%\Private\WinEngine.cpp %linker% %libs%
popd

ENDLOCAL

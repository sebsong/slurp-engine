@echo off

SETLOCAL

set libs=user32.lib gdi32.lib
set includes=/I %engine_dir%\Public\ /I %engine_dir%\Private\

if "%~1" == "" (
    set debug=1
) else (
    set debug=%1
)

set macros=/D PLATFORM_WINDOWS
if %debug% == 1 (
    set macros=%macros% /D DEBUG
)

if not exist %slurp_dir%\build mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl /std:c++20 -FC -Zi /EHsc %includes% %engine_dir%\Private\WinEngine.cpp %libs% %macros%
popd

ENDLOCAL

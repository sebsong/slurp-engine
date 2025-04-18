@echo off

SETLOCAL

set libs=user32.lib gdi32.lib
set includes=/I %engine_dir%\Public\ /I %engine_dir%\Private\

mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl -FC -Zi   %includes% %engine_dir%\Private\WinEngine.cpp %libs%
popd

ENDLOCAL

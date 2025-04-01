@echo off

SETLOCAL

set libs=user32.lib gdi32.lib

mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl -Zi ..\Engine\WinEngine.cpp %libs%
popd

ENDLOCAL

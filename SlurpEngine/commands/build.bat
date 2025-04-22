@echo off

SETLOCAL

set libs=user32.lib gdi32.lib
set includes=/I %engine_dir%\Public\ /I %engine_dir%\Private\

if not exist %slurp_dir%\build mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl /std:c++20 -FC -Zi /EHsc %includes% %engine_dir%\Private\WinEngine.cpp %libs%
popd

ENDLOCAL

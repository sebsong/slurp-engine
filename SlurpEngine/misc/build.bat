@echo off

mkdir %slurp_dir%\build
pushd %slurp_dir%\build
cl -Zi ..\Engine\Engine.cpp user32.lib
popd

@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=w:\slurp-engine\SlurpEngine\commands;%path%

set slurp_dir="w:\slurp-engine\SlurpEngine"
set engine_dir="%slurp_dir%\Engine"

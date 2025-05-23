@echo off

SETLOCAL

set source_dir=%engine_dir%\Private

set includes=-I %engine_dir%\Public\ -I %engine_dir%\Private\
set warning_flags=-WX -W4 -wd4100 -wd4189 -wd4505
set macros=-D PLATFORM_WINDOWS
set compiler_flags=-nologo -FC -EHsc -Oi -std:c++14

set debug_flags=-Od -Z7 -FmWinEngine.map -MDd
set release_flags=-O2 -MD

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

set slurp_compilation_files=%source_dir%\SlurpEngine.cpp

set timestamp=%time::=_%
set timestamp=%timestamp:.=_%
set timestamp=%timestamp: =%
set slurp_linker_flags=-incremental:no -PDB:slurp_%timestamp%.pdb
set win_linker_flags=-opt:ref
set libs=user32.lib gdi32.lib Winmm.lib Shlwapi.lib

if not exist %slurp_dir%\build mkdir %slurp_dir%\build
pushd %slurp_dir%\build
del *.pdb
set build_start=%time%

cl %includes% %warning_flags% %macros% %compiler_flags% -LD %slurp_compilation_files% -link %slurp_linker_flags%
cl %includes% %warning_flags% %macros% %compiler_flags% %source_dir%\WinEngine.cpp -link %win_linker_flags% %libs%

set build_end=%time%
echo Build Start: %build_start%
echo Build End:   %build_end%

popd

ENDLOCAL

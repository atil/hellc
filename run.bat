@echo off

if not exist bin (
    mkdir bin
)

pushd bin
if "%1" == "-d" (
    cl -Zi /nologo /I..\include\ ..\src\main.c ..\lib\glew32.lib ..\lib\glfw3dll.lib opengl32.lib
    devenv main.exe
) else if "%1" == "-pdb" (
    cl -Zi /nologo /I..\include\ ..\src\main.c ..\lib\glew32.lib ..\lib\glfw3dll.lib opengl32.lib
) else (
    cl /nologo /I..\include\ ..\src\main.c ..\lib\glew32.lib ..\lib\glfw3dll.lib opengl32.lib
    REM This is the stupidest == 0 check ever
    REM https://stackoverflow.com/a/32751809/4894526
    if errorlevel 0 if not errorlevel 1 main.exe
)
popd

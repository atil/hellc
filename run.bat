@echo off

if not exist bin (
    mkdir bin
)

pushd bin
if "%1" == "-vs" (
    call :CompileDebug
    devenv game.exe
) else if "%1" == "-d" (
    call :CompileDebug
) else (
    call :Compile
    REM This is the stupidest == 0 check ever
    REM https://stackoverflow.com/a/32751809/4894526
    if errorlevel 0 if not errorlevel 1 game.exe
)
popd
exit /b

:Compile
cl /nologo /EHsc /I..\include\ ..\src\*.cpp  ..\lib\*.lib opengl32.lib /Fe"game.exe"
exit /b

:CompileDebug
cl -Zi /nologo /EHsc /I..\include\ ..\src\*.cpp ..\lib\*.lib opengl32.lib /Fe"game.exe"

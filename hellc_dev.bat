REM This is the shell-specific stuff for this project
REM Attach this file to the cmd shortcut with "cmd.exe /k hellc_dev.bat"

@echo off
DOSKEY r="run.bat"
subst w: %HOME%\code\hellc
w:
cls
REM This needs to be the last thing we do, for some reason. Otherwise the ones after won't execute
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

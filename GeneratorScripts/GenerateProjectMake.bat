@echo off
pushd %~dp0
call ..\libs\premake5\windows\premake5.exe --file=../premake5.lua gmake2
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
popd
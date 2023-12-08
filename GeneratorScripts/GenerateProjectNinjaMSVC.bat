@echo off
pushd %~dp0
call ..\libs\premake5\windows\premake5.exe --os=windows --file=../premake5.lua ninja
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
popd

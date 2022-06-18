@echo off
pushd %~dp0
call ..\libs\premake5\windows\premake5.exe --file=../premake5.lua vs2019
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
popd
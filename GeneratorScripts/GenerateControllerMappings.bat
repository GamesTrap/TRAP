@echo off
pushd %~dp0
call ..\libs\premake5\windows\premake5.exe --file=../.modules/generatecontrollermappings/generatecontrollermappings.lua genconmappings
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
popd
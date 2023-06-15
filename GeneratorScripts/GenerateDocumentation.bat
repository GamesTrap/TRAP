@echo off
pushd %~dp0
call ..\libs\premake5\windows\premake5.exe --file=../.modules/generatedocs/generatedocs.lua gendocs
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
popd
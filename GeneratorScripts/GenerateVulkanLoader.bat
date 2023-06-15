@echo off
pushd %~dp0
call ..\libs\premake5\windows\premake5.exe --file=../.modules/generatevulkanloader/generatevulkanloader.lua generatevulkanloader
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)
popd
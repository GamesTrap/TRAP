#!/bin/bash
exec="../libs/premake5/linux/./premake5 --file=../.modules/generatevulkanloader/generatevulkanloader.lua generatevulkanloader"
$exec
if [[ "$?" -ne 0 ]]; then
    read -s -N 1 -p "Press any key to continue...";
fi


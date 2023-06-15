#!/bin/bash
exec="../libs/premake5/linux/./premake5 --file=../premake5.lua generatevulkanloader"
$exec
if [[ "$?" -ne 0 ]]; then
    read -s -N 1 -p "Press any key to continue...";
fi


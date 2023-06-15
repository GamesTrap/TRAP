#!/bin/bash
exec="../libs/premake5/linux/./premake5 --file=../.modules/generatecontrollermappings/generatecontrollermappings.lua genconmappings"
$exec
if [[ "$?" -ne 0 ]]; then
    read -s -N 1 -p "Press any key to continue...";
fi


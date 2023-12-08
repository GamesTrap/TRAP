#!/bin/bash
exec="../libs/premake5/linux/./premake5 --file=../premake5.lua ninja --cc=gcc"
$exec
if [[ "$?" -ne 0 ]]; then
    read -s -N 1 -p "Press any key to continue...";
fi
exec="../libs/premake5/linux/./premake5 --file=../premake5.lua ecc"
$exec
if [[ "$?" -ne 0 ]]; then
    read -s -N 1 -p "Press any key to continue...";
fi
exec="../libs/premake5/linux/./premake5 --file=../premake5.lua vscode --action=ninja"
$exec
if [[ "$?" -ne 0 ]]; then
    read -s -N 1 -p "Press any key to continue...";
fi

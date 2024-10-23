# Steamworks SDK

## Installation

1. Get the version 1.60 of the Steamworks SDK from https://partner.steamgames.com/.
2. Extract the contents of the steamworks_sdk_160.zip file into this folder.

TRAP will then automatically detect if the Steamworks SDK is installed properly on the next run of any GenerateProject script.  
If everything went right, feel free to use the TRAP::Utils::Steam namespace.

### Windows

The steam_api64.dll file will be automatically copied to the .exe file's folder.

### Linux

The libsteam_api.so file will be automatically copied to the executables folder.

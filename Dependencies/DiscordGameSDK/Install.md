# Discord Game SDK

## Installation

1. Get the latest version of the Discord Game SDK from https://discord.com/developers/docs/game-sdk/sdk-starter-guide.
2. Extract the contents of the discord_game_sdk.zip file into this folder.

TRAP will then automatically detect if the Discord Game SDK is installed properly on the next run of any GenerateProject script.  
To make use of the features offered by the Discord Game SDK set the preprocessor macro USE_DISCORD_GAME_SDK in Core/Base.h  
After that feel free to use the TRAP::Utils::Discord namespace.

### Windows

The discord_game_sdk.dll file will be automatically copied to the .exe file's folder.

### Linux

The discord_game_sdk.so file will ne automatically copied to /usr/local/lib during linking
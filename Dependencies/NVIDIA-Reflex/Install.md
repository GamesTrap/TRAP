# NVIDIA Reflex SDK

## Installation

1. Create an account on the [NVIDIA Developer site](https://developer.nvidia.com/).
2. Download the NVIDIA Reflex SDK from the [NVIDIA Reflex SDK site](https://developer.nvidia.com/performance-rendering-tools/reflex/get-started).
3. Extract the contents of the Nvidia_Reflex_SDK_X.X.zip file into this folder.

The newest version supported by TRAP is NVIDIA Reflex SDK 1.6.

TRAP will then automatically detect if the NVIDIA Reflex SDK is installed properly on the next run of any GenerateProject script.  
If everything went right, feel free to use the features via the TRAP::Graphics::RenderCommand::SetLatencyMode() function.  
NOTE: NVIDIA Reflex is only available on Windows 10 and newer.

### Windows

The NvLowLatencyVk.dll file will be automatically copied to the .exe file's folder.

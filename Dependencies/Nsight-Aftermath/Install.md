# Nsight-Aftermath

## Installation

1. Create an account on the [NVIDIA Developer site](https://developer.nvidia.com/).
2. Download the Nsight-Aftermath SDK from the [NVIDIA Nsight Aftermath SDK site](https://developer.nvidia.com/nsight-aftermath).
3. Extract the contents of the NVIDIA_Nsight_Aftermath_SDK_XXXX.X.X.XXXXX.zip/tgz file into this folder.

TRAP will then automatically detect if the Nsight-Aftermath SDK is installed properly on the next run of any GenerateProject script.  
If everything went right, feel free to use the macro in Base.h to enable Nsight-Aftermath in debug builds.

### Windows

The GFSDK_Aftermath_Lib.x64.dll file will be automatically copied to the .exe file's folder.

### Linux

The libGFSDK_Aftermath_Lib.x64.so file will be automatically copied to /usr/local/lib during linking.

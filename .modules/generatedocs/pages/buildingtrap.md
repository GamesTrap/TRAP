(buildingtrap)=

# Building TRAP™

## Requirements

TRAP™ Engine requires a GPU that is Vulkan 1.1 or newer compatible.

:::{tab} Windows
Please install the [VulkanSDK](https://vulkan.lunarg.com/sdk/home/).

If you want to be able to build the documentation and to update the Vulkan loader you need to install [Python 3](https://www.python.org/downloads/).
:::
:::{tab} Linux
Please install the [VulkanSDK](https://vulkan.lunarg.com/sdk/home/) and make sure that the `VULKAN_SDK` environment variable is set properly.

If you want to be able to build the documentation and to update the Vulkan loader you need to install [Python 3](https://www.python.org/downloads/).
Make sure Python 3 can be used from Terminal via the `python` command.
:::

## Cloning the repository

Clone the repository with {code}`git clone --recursive https://github.com/GamesTrap/TRAP`.

:::{note}
If the repository has already been cloned non-recursively, use {code}`git submodule update --init --recursive` to clone the necessary submodules.
:::

## Project generation

TRAP™ uses [Premake 5](https://premake.github.io/) as a build generation tool.
By default it is shipped with the TRAP™ repository.

:::{tab} Windows
   You can generate project files for Visual Studio 2022, Visual Studio 2019, Codelite and make.
:::
:::{tab} Linux
   You can generate project files for Codelite and make.
:::
Just run the appropriate generator script inside the `GeneratorScripts` folder.

## Generate documentation

:::{important}
Python 3 is required in order to build the documentation.
:::

To update the documentation run the `GenerateDocumentation` script inside the `GeneratorScripts` folder.

## Update controller mappings

To update the controller mappings database run the `GenerateControllerMappings` script inside the `GeneratorScripts` folder.

## Update Vulkan loader

:::{important}
Python 3 is required in order to update the Vulkan loader.
:::

To update the Vulkan loader run the `GenerateVulkanLoader` script inside the `GeneratorScripts` folder.

(additionallibraries)=

## Additional libraries

:::{tab} Discord Game SDK
To install the Discord Game SDK do the following:

1. Download the [Discord Game SDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)
2. Extract the contents of the downloaded SDK to the `Dependencies/DiscordGameSDK` folder.

TRAP™ will then automatically detect if the Discord Game SDK is installed properly on the next run of any of the GenerateProject scripts inside the `GeneratorScripts` folder.

If everything went right, feel free to use the TRAP::Utils::Discord namespace.
:::
:::{tab} NVIDIA® Nsight™ Aftermath SDK
To install the NVIDIA® Nsight™ Aftermath SDK do the following:

1. Create an account on the [NVIDIA Developer site](https://developer.nvidia.com/).
2. Download the [NVIDIA® Nsight™ Aftermath SDK](https://developer.nvidia.com/nsight-aftermath).
3. Extract the contents of the downloaded SDK to the `Dependencies/Nsight-Aftermath` folder.

TRAP™ will then automatically detect if the NVIDIA® Nsight™ Aftermath SDK is installed properly on the next run of any of the GenerateProject scripts inside the `GeneratorScripts` folder.

If everything went right, feel free to use the `ENABLE_NSIGHT_AFTERMATH` macro in Base.h to enable Nsight-Aftermath in debug builds.
:::

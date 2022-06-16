# Building TRAP™

(buildingtrap)=

## Requirements

TRAP™ Engine requires a GPU that is Vulkan 1.1 or newer compatible.

Please install the [VulkanSDK](https://vulkan.lunarg.com/sdk/home/) and make sure that the `VULKAN_SDK` environment variable is set properly.

If you want to be able to build the documentation and to update the Vulkan loader you need to install [Python3](https://www.python.org/downloads/).

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

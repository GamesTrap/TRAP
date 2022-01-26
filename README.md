# TRAP

![TRAP](Branding/TRAP.gif?raw=true "TRAP")

TRAPVK-Alpha is a cross-platform, high performance game engine currently in development.  
This README will be updated with more info eventually.

## Setup

### Requirements

TRAP Engine requires the [VulkanSDK](https://vulkan.lunarg.com/sdk/home)  
TRAP Engine requires a GPU that is Vulkan 1.2 or newer compatible.

First clone the repository with `git clone --recursive https://github.com/GamesTrap/TRAP`.

> Note: If the repository has already been cloned non-recursively, use `git submodule update --init` to clone the necessary submodules.

TRAP uses _Premake 5_ as a build generation tool. Follow [these](https://premake.github.io/download.html) instructions in order to install it.  
Also make sure you have [Python 3](https://www.python.org/downloads/) installed and accessible via ```python3``` as it is needed by some scripts.

Then, execute one of the generator scripts in the GeneratorScripts folder.

## Updating Controller Mappings

To update the controller mappings run the `GenerateControllerMappings` file from the GeneratorScripts folder.

## Updating Vulkan Loader

To update the Vulkan Loader run the `GenerateVulkanLoader` file from the GeneratorScripts folder.

## Documentation

To generate the documentation run the `GenerateDocumentation` file from the GeneratorScripts folder.

## 3rd Party Libraries used by TRAP Engine

- [ImGui](https://github.com/ocornut/imgui/tree/docking)
- [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
- [GLSLang](https://github.com/KhronosGroup/glslang)
- [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross)
- [YAML-Cpp](https://github.com/jbeder/yaml-cpp)
- [Entt](https://github.com/skypjack/entt)
- [GLFW](https://github.com/glfw/glfw) (Custom rewrite)
- [GLM](https://github.com/g-truc/glm) (Custom rewrite)
- [LodePNG](https://github.com/lvandeve/lodepng) (Embedded, only Inflate/decompression)
- [OpenSans](https://fonts.google.com/specimen/Open+Sans#standard-styles) (Embedded fonts)
- [Premake5](https://github.com/premake/premake-core) (Build system)
- [SFML](https://github.com/SFML/SFML) (Only Networking part)
- [Volk](https://github.com/zeux/volk) (Embedded as VulkanLoader)
- [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/)

## System Requirements

### Minimum System Requirements

- OS: Windows 7/Ubuntu 18.04.2 or newer (64-Bit versions only)
- Processor: AVX2 support, TBD
- Graphics Card: Vulkan 1.2 support, TBD
- Memory: 4 GB RAM
- Storage: 5+ GB available space
- Additional Notes: -

### Recommended System Requirements

- OS: Windows 7/Ubuntu 18.04.2 or newer (64-Bit versions only)
- Processor: AVX2 support, TBD
- Graphics Card: Vulkan 1.2 support, TBD
- Memory: 4 GB RAM
- Storage: 10+ GB available space
- Additional Notes: -

## License

Copyright (c) 2019-2022 Jan "GamesTrap" Schürkamp

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

# TRAP

![TRAP](Branding/TRAP.gif?raw=true "TRAP")

[![Build Status](https://travis-ci.com/GamesTrap/TRAPVK.svg?branch=master)](https://travis-ci.com/GamesTrap/TRAPVK)
[![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/GamesTrap/TRAPVK)](https://github.com/GamesTrap/TRAPVK)
[![GitHub repo size](https://img.shields.io/github/repo-size/GamesTrap/TRAPVK)](https://github.com/GamesTrap/TRAPVK)
[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/GamesTrap/TRAP?include_prereleases)](https://github.com/GamesTrap/TRAPVK/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/GamesTrap/TRAPVK/total)](https://github.com/GamesTrap/TRAPVK/releases)
[![GitHub issues](https://img.shields.io/github/issues/GamesTrap/TRAPVK)](https://github.com/GamesTrap/TRAPVK/issues?q=is%3Aopen+is%3Aissue)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/GamesTrap/TRAPVK)](https://github.com/GamesTrap/TRAPVK/pulls?q=is%3Aopen+is%3Apr)
[![GitHub](https://img.shields.io/github/license/GamesTrap/TRAPVK)](https://github.com/GamesTrap/TRAPVK/blob/master/LICENSE)


TRAPVK is a cross-platform, high performance game engine currently in development.  
This README will be updated with more info eventually.

## Setup

### Requirements

TRAP Engine requires the [VulkanSDK](https://vulkan.lunarg.com/sdk/home)  
TRAP Engine requires a GPU that is Vulkan 1.2 or newer compatible.

First clone the repository with `git clone --recursive https://github.com/GamesTrap/TRAP`.

> Note: If the repository has already been cloned non-recursively, use `git submodule update --init` to clone the necessary submodules.

TRAP uses _Premake 5_ as a build generation tool. Follow [these](https://premake.github.io/download.html) instructions in order to install it.

Then, follow the steps relevant to your operating system.

### Windows

Premake can generate project files for several Visual Studio versions.
For example, run `premake vs2019` to generate the `.sln` and `.vcxproj` files for Visual Studio 2019.

### Linux

Premake can generate makefile project files.
For example, run `premake gmake2` to generate the `Makefile` files.

## 3rd Party Libraries used by TRAP Engine:

- [ImGui](https://github.com/GamesTrap/imgui)
- [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
- [GLSLang](https://github.com/GamesTrap/glslang)
- [SPIRV-Cross](https://github.com/GamesTrap/SPIRV-Cross)
- [Entt](https://github.com/skypjack/entt)

## System Requirements

### Minimum System Requirements

- OS: Windows 7/Ubuntu 18.04.2 or newer (64-Bit versions only)
- Processor: AVX2 support, TBD
- Graphics Card: TBD
- Memory: 4 GB RAM
- Storage: 5+ GB available space
- Any of these RenderAPIs
  - Vulkan: Version 1.2
  - OpenGL: Version 4.6
- Additional Notes: -

### Recommended System Requirements

- OS: Windows 7/Ubuntu 18.04.2 or newer (64-Bit versions only)
- Processor: AVX2 support, TBD
- Graphics Card: TBD
- Memory: 4 GB RAM
- Storage: 10+ GB available space
- Any of these RenderAPIs
  - Vulkan: Version 1.2
  - OpenGL: Version 4.6
- Additional Notes: -

## License

Copyright (c) 2019-2020 Jan "GamesTrap" Schürkamp

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
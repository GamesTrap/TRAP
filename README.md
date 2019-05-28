# TRAP

![TRAP](Branding/TRAP.gif?raw=true "TRAP")

Status: [![Build Status](https://travis-ci.com/GamesTrap/TRAP.svg?branch=master)](https://travis-ci.com/GamesTrap/TRAP)

TRAP is a cross-platform, high performance game engine currently in development.   
This README will be updated with more info eventually.

## Setup

### Requirements

TRAP Engine requires the [VulkanSDK](https://vulkan.lunarg.com/sdk/home)  
TRAP Engine requires a GPU that is DirectX12, Vulkan 1.1, OpenGL 4.6 or newer compatible.
  
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

### Mac OS X

Premake can generate project files for XCode4.
For example, run `premake xcode4` to generate the `.xcworkspace` and `.xcodeproj` fiesl for XCode 4.

## 3rd Party Libraries used by TRAP Engine:
- [GLFW](https://github.com/GamesTrap/glfw)
- [GLAD](https://glad.dav1d.de/)
- [ImGui](https://github.com/GamesTrap/imgui)
- [VulkanSDK](https://vulkan.lunarg.com/sdk/home)

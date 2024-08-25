(trap-engine)=

# TRAP™ Engine

```{image} Branding/TRAP.gif
:align: center
:alt: Powered by TRAP™ Engine
:height: 450px
:width: 800px
```

TRAP™ is a cross-platform, high performance game engine currently in development.

:::{warning}
This documentation is still WIP and may contain errors and/or outdated code!
:::

To build TRAP™ follow the guide on this page: {ref}`buildingtrap`.

## System Requirements

:::{tab} Minimum

- OS: Windows 10/Ubuntu 18.04.6 or newer (64-Bit versions only)
- Processor: Intel or AMD CPU with AVX2 support
- Graphics Card: AMD, NVIDIA with Vulkan 1.1 support (Intel untested)
- Memory: 4 GB RAM
- Storage: 5+ GB available space
- Additional Notes: -

:::
:::{tab} Recommended

- OS: Windows 10/Ubuntu 18.04.6 or newer (64-Bit versions only)
- Processor: Intel or AMD CPU with AVX2 support
- Graphics Card: AMD, NVIDIA with Vulkan 1.1 support (Intel untested)
- Memory: 4 GB RAM
- Storage: 10+ GB available space
- Additional Notes: -

:::

## 3rd Party Libraries used by TRAP™ Engine

:::{tab} Required
The following libraries/utilities are required to build TRAP™.
They are provided in the TRAP™ repository.

- [Box2D](https://github.com/erincatto/box2d)
- [Entt](https://github.com/skypjack/entt)
- [fmt](https://github.com/fmtlib/fmt)
- [GCEM](https://github.com/kthohr/gcem)
- [GLFW](https://github.com/glfw/glfw) integrated as TRAP::INTERNAL::WindowingAPI
- [GLM](https://github.com/g-truc/glm) integrated as TRAP::Math
- [GLSLang](https://github.com/GamesTrap/glslang)
- [ImGui](https://github.com/GamesTrap/imgui) (docking branch)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [LibDecor](https://gitlab.freedesktop.org/libdecor/libdecor) - Linux Wayland client-side decorations
- [LodePNG](https://github.com/lvandeve/lodepng) integrated as TRAP::INTERNAL::PNGImage and TRAP::Utils::Decompress::Inflate()
- [ModernDialogs](https://github.com/GamesTrap/ModernDialogs)
- [PlatformFolders](https://github.com/sago007/PlatformFolders) partially integrated as part of TRAP::FileSystem
- [Premake](https://github.com/premake/premake-core) as build system
- [Premake-VSCodeGen](https://github.com/GamesTrap/Premake-VSCodeGen)
- [Premake-Ninja](https://github.com/GamesTrap/premake-ninja)
- [Quite OK Image Format](https://github.com/phoboslab/qoi)
- [SDL_GameControllerDB from gabomdq](https://github.com/gabomdq/SDL_GameControllerDB)
- [SFML](https://github.com/SFML/SFML) partially used as part of TRAP::Network
- [SPIRV-Cross](https://github.com/GamesTrap/SPIRV-Cross)
- [TinyImageFormat](https://github.com/DeanoC/tiny_imageformat) integrated as TRAP::Graphics::API::ImageFormat
- [Tracy](https://github.com/wolfpld/tracy) only client integration is used
- [Volk](https://github.com/zeux/volk) integrated as VulkanLoader
- [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [YAMLCpp](https://github.com/jbeder/yaml-cpp)
- [zlib](https://github.com/madler/zlib/)
- [libpng](https://github.com/pnggroup/libpng)
:::
:::{tab} Additional
The following libraries add additional features to TRAP™.  

The user is required to supply these. See the {ref}`additionallibraries` section of the {ref}`buildingtrap` page for instructions.

- [Discord Game SDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)
- [NVIDIA® Nsight™ Aftermath SDK](https://developer.nvidia.com/nsight-aftermath)
- [NVIDIA® Reflex SDK](https://developer.nvidia.com/performance-rendering-tools/reflex)
- [Steamworks SDK](https://partner.steamgames.com/doc/sdk)
:::


```{toctree}
:caption: 'TRAP:'
:hidden: true
:maxdepth: 2

self
pages/buildingtrap
pages/gettingstarted
pages/changelog
pages/eula

api/library_root
```

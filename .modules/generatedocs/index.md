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

- OS: Windows 7/Ubuntu 18.04.6 or newer (64-Bit versions only)
- Processor: Intel or AMD CPU with AVX2 support
- Graphics Card: AMD, NVIDIA with Vulkan 1.1 support (Intel untested)
- Memory: 4 GB RAM
- Storage: 5+ GB available space
- Additional Notes: -

:::
:::{tab} Recommended

- OS: Windows 7/Ubuntu 18.04.6 or newer (64-Bit versions only)
- Processor: Intel or AMD CPU with AVX2 support
- Graphics Card: AMD, NVIDIA with Vulkan 1.1 support (Intel untested)
- Memory: 4 GB RAM
- Storage: 10+ GB available space
- Additional Notes: -

:::

## 3rd Party Libraries used by TRAP™ Engine

:::{tab} Required
The following libraries are required to build TRAP™.
They are provided in the TRAP™ repository.

- [ImGui](https://github.com/GamesTrap/imgui)
- [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
- [GLSLang](https://github.com/GamesTrap/glslang)
- [SPIRV-Cross](https://github.com/GamesTrap/SPIRV-Cross)
- [Entt](https://github.com/skypjack/entt)
- [ModernDialogs](https://github.com/GamesTrap/ModernDialogs)
- [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [YAMLCpp](https://github.com/jbeder/yaml-cpp)
- [SDL_gamecontrollerdb.h from SDL](https://github.com/libsdl-org/SDL/blob/main/src/joystick/SDL_gamecontrollerdb.h)
- [Volk](https://github.com/zeux/volk)
:::
:::{tab} Additional
The following libraries add additional features to TRAP™.  

The user is required to supply these. See the {ref}`additionallibraries` section of the {ref}`buildingtrap` page for instructions.

- [DiscordGameSDK](https://discord.com/developers/docs/game-sdk/sdk-starter-guide)
- [NVIDIA® Nsight™ Aftermath SDK](https://developer.nvidia.com/nsight-aftermath)
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

(gettingstarted)=

# Getting Started

## Differences between normal mode and headless mode

:::{tab} Normal mode
All classes, functions, etc. are available to use.

This mode by default creates a Window to be used to render to.
The engine uses more strict RenderAPI compatibility checks and creates a Surface and Swapchain for the default Window.
:::
:::{tab} Headless mode
Some classes, functions, etc. may not be available to use.

This mode does the following by default:

- WindowingAPI only gets initialized if there is a supported Window Manager available
- Default Window only gets created if the WindowingAPI is available to use. It is hidden by default
- Relaxed RenderAPI compatibility checks are used
- Rendering uses a back buffer to render into instead of the usual Swapchain and Surface

The following APIs may not be available in headless mode and usage of these may result in crashes:

- WindowingAPI may not be available if the engine was unable to recognize a supported Window Manager
- Input is only available if WindowingAPI is available to use
- RendererAPI is not available if the host does not have any GPU, else RendererAPI is initialized as usual
- Surfaces and Swapchains may not be available depending on if the host has a GPU with surface capability
:::

:::{note}
More Info Soon.
:::

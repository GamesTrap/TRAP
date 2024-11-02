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

- Some classes/structs/namespaces from normal mode are not available to use
- Relaxed RenderAPI compatibility checks are used
- RendererAPI uses a back buffer to render into instead of the usual Swapchain and Surface

The following classes/structs/namespaces are not available in headless mode:

- ImGui (TRAP::Graphics::ImGuiLayer, TRAP::INTERNAL::ImGuiWindowing)
- ImGuizmo
- DiscordGameSDK (TRAP::Utils::Discord)
- TRAP::Graphics::EditorCamera
- TRAP::Graphics::OrthographicCameraBounds
- TRAP::Graphics::OrthographicCameraController
- TRAP::Window
- TRAP::Monitor
- TRAP::Input
- TRAP::Events::ControllerEvent (TRAP::Events::ControllerConnectEvent, TRAP::Events::ControllerDisconnectEvent)
- TRAP::Events::KeyEvent (TRAP::Events::KeyPressEvent, TRAP::Events::KeyReleaseEvent)
- TRAP::Events::KeyTypeEvent
- TRAP::Events::KeyLayoutEvent
- TRAP::Events::MonitorEvent (TRAP::Events::MonitorConnectEvent, TRAP::Events::MonitorDisconnectEvent)
- TRAP::Events::MouseMoveEvent
- TRAP::Events::MouseScrollEvent
- TRAP::Events::MouseButtonEvent (TRAP::Events::MouseButtonPressEvent, TRAP::Events::MouseButtonReleaseEvent)
- TRAP::Events::MouseEnterEvent
- TRAP::Events::MouseLeaveEvent
- TRAP::Events::WindowResizeEvent
- TRAP::Events::WindowMinimizeEvent
- TRAP::Events::WindowMaximizeEvent
- TRAP::Events::WindowRestoreEvent
- TRAP::Events::WindowCloseEvent
- TRAP::Events::WindowMoveEvent
- TRAP::Events::WindowFocusEvent
- TRAP::Events::WindowLostFocusEvent
- TRAP::Events::WindowDropEvent
- TRAP::Events::WindowContentScaleEvent
- TRAP::Events::FrameBufferResizeEvent
- TRAP::INTERNAL::WindowingAPI
- TRAP::Graphics::SwapChain (TRAP::Graphics::API::VulkanSwapChain)
- TRAP::Graphics::API::VulkanSurface
:::

:::{note}
More Info Soon.
:::

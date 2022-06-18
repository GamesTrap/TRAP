/*
The MIT License(MIT)

Copyright(c) 2014 - 2022 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_IMGUIWINDOWING_H
#define TRAP_IMGUIWINDOWING_H

#include "Core/PlatformDetection.h"
#include "Window/WindowingAPI.h"

namespace TRAP::Graphics
{
	enum class RenderAPI;
}

namespace TRAP::INTERNAL
{
	/// <summary>
	/// Interfacing class between ImGui and the TRAP::INTERNAL::WindowingAPI.
	/// </summary>
	class ImGuiWindowing
	{
	public:
		/// <summary>
		/// Initialize the ImGui TRAP::INTERNAL::WindowingAPI interface.
		/// </summary>
		/// <param name="window">Handle to the internal main window.</param>
		/// <param name="installCallbacks">Whether to set ImGui callbacks or not.</param>
		/// <param name="renderAPI">RenderAPI to be used by ImGui.</param>
		/// <returns>True on successful initialization, false otherwise.</returns>
		static bool Init(WindowingAPI::InternalWindow* window, bool installCallbacks,
		                 Graphics::RenderAPI renderAPI);
		/// <summary>
		/// Shutdown the ImGui TRAP::INTERNAL::WindowingAPI interface.
		/// </summary>
		static void Shutdown();
		/// <summary>
		/// Starts a new ImGui frame.
		/// </summary>
		static void NewFrame();
		/// <summary>
		/// Set a custom cursor to be shown in ImGui UIs.
		/// </summary>
		/// <param name="cursor">Cursor to show.</param>
		static void SetCustomCursor(Scope<WindowingAPI::InternalCursor>& cursor);

		/// <summary>
		/// Callback to notify ImGui that a monitor was connected or disconnected.
		/// </summary>
		/// <param name="unused1">Monitor whose status changed.</param>
		/// <param name="unused2">Whether monitor got connected or disconnected.</param>
		static void MonitorCallback(const WindowingAPI::InternalMonitor* unused1, bool unused2);
	private:
		static WindowingAPI::InternalWindow* s_window; //Main Window
		static double s_time;
		static std::array<bool, 5> s_mouseJustPressed;
		static std::array<Scope<WindowingAPI::InternalCursor>, ImGuiMouseCursor_COUNT> s_mouseCursors;
		static bool s_installedCallbacks;
		static bool s_wantUpdateMonitors;
		static Scope<WindowingAPI::InternalCursor> s_customCursor;
		static TRAP::Graphics::RenderAPI s_renderAPI;

		//Chain WindowingAPI callbacks for main viewport: Our callbacks will call the user's
		//                                                previously installed callbacks, if any.
		static WindowingAPI::MouseButtonFunc s_prevUserCallbackMouseButton;
		static WindowingAPI::ScrollFunc s_prevUserCallbackScroll;
		static WindowingAPI::KeyFunc s_prevUserCallbackKey;
		static WindowingAPI::CharFunc s_prevUserCallbackChar;

		/// <summary>
		/// Data struct for an ImGuiViewport.
		/// </summary>
		struct ImGuiViewportDataTRAP
		{
			Scope<WindowingAPI::InternalWindow> Window = nullptr;
			WindowingAPI::InternalWindow* WindowPtr = nullptr;
			bool WindowOwned = false;
			int32_t IgnoreWindowSizeEventFrame = -1;

			/// <summary>
			/// Destructor
			/// </summary>
			~ImGuiViewportDataTRAP()
			{
				IM_ASSERT(Window == nullptr);
			}
		};

		/// <summary>
		/// Initialize the platform interface.
		/// </summary>
		static void InitPlatformInterface();
		/// <summary>
		/// Update the mouse button states and the position.
		/// </summary>
		static void UpdateMousePosAndButtons();
		/// <summary>
		/// Update mouse cursor mode and shape.
		/// </summary>
		static void UpdateMouseCursor();
		/// <summary>
		/// Update gamepad states.
		/// </summary>
		static void UpdateGamepads();
		/// <summary>
		/// Update all ImGui monitor states.
		/// </summary>
		static void UpdateMonitors();

		/// <summary>
		/// Retrieve data from clipboard as string.
		/// </summary>
		/// <param name="userData">Unused user data.</param>
		/// <returns>Clipboard string.</returns>
		static const char* GetClipboardText(void* userData);
		/// <summary>
		/// Set clipboard text.
		/// </summary>
		/// <param name="userData">Unused user data.</param>
		/// <param name="text">Text to set.</param>
		static void SetClipboardText(void* userData, const char* text);
		/// <summary>
		/// Mouse button callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="mouseButton">Affected mouse button.</param>
		/// <param name="pressed">Is button pressed or not.</param>
		static void MouseButtonCallback(const WindowingAPI::InternalWindow* window, Input::MouseButton mouseButton,
		                                bool pressed);
		/// <summary>
		/// Scroll callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="xOffset">Horizontal mouse wheel offset.</param>
		/// <param name="yOffset">Vertical mouse wheel offset.</param>
		static void ScrollCallback(const WindowingAPI::InternalWindow* window, double xOffset, double yOffset);
		/// <summary>
		/// Key callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="key">Keyboard key.</param>
		/// <param name="pressed">Is key pressed or not.</param>
		static void KeyCallback(const WindowingAPI::InternalWindow* window, Input::Key key, bool pressed);
		/// <summary>
		/// Char callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="codePoint">UTF-32 code point.</param>
		static void CharCallback(const WindowingAPI::InternalWindow* window, uint32_t codePoint);
		/// <summary>
		/// Window close callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		static void WindowCloseCallback(const WindowingAPI::InternalWindow* window);
		/// <summary>
		/// Window position callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="x">X position.</param>
		/// <param name="y">Y position.</param>
		static void WindowPosCallback(const WindowingAPI::InternalWindow* window, int32_t x, int32_t y);
		/// <summary>
		/// Window size callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="width">Width.</param>
		/// <param name="height">Height.</param>
		static void WindowSizeCallback(const WindowingAPI::InternalWindow* window, int32_t width, int32_t height);
		/// <summary>
		/// Create a new window to be handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		static void CreateWindow(ImGuiViewport* viewport);
		/// <summary>
		/// Destroy a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		static void DestroyWindow(ImGuiViewport* viewport);
		/// <summary>
		/// Show/Display a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		static void ShowWindow(ImGuiViewport* viewport);
		/// <summary>
		/// Retrieve the position of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <returns>Current window position.</returns>
		static ImVec2 GetWindowPos(ImGuiViewport* viewport);
		/// <summary>
		/// Set the position of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <param name="pos">New window position.</param>
		static void SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
		/// <summary>
		/// Retrieve the size of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <returns>Current window size.</returns>
		static ImVec2 GetWindowSize(ImGuiViewport* viewport);
		/// <summary>
		/// Set the size of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <param name="size">New window size.</param>
		static void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
		/// <summary>
		/// Set the title of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <param name="title">New window title.</param>
		static void SetWindowTitle(ImGuiViewport* viewport, const char* title);
		/// <summary>
		/// Focus a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		static void SetWindowFocus(ImGuiViewport* viewport);
		/// <summary>
		/// Retrieve focus state of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <returns>True if window has focus, false otherwise.</returns>
		static bool GetWindowFocus(ImGuiViewport* viewport);
		/// <summary>
		/// Retrieve minimization state of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <returns>True if window is minimized, false otherwise.</returns>
		static bool GetWindowMinimized(ImGuiViewport* viewport);
		/// <summary>
		/// Set transparency/alpha value of a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <param name="alpha">New alpha value.</param>
		static void SetWindowAlpha(ImGuiViewport* viewport, float alpha);
		/// <summary>
		/// Render window which is handled by ImGui.
		/// Note: This function does not actually render anything when using RenderAPI::Vulkan.
		/// </summary>
		/// <param name="unused"></param>
		static void RenderWindow(ImGuiViewport* viewport, void* unused);
		/// <summary>
		/// Create a Vulkan surface for a window which is handled by ImGui.
		/// </summary>
		/// <param name="viewport">Viewport data from ImGui.</param>
		/// <param name="vkInstance">VkInstance to create surface with.</param>
		/// <param name="vkAllocator">VkAllocationCallbacks to be used for surface creation.</param>
		/// <param name="outVkSurface">Out pointer to surface handle.</param>
		/// <returns>VkResult of the surface creation.</returns>
		static int32_t CreateVkSurface(ImGuiViewport* viewport, ImU64 vkInstance, const void* vkAllocator,
		                               ImU64* outVkSurface);
	};
}

#endif /*TRAP_IMGUIWINDOWING_H*/
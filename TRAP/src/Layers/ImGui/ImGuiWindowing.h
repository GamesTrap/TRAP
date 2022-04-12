/*The MIT License(MIT)

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
		/// </summary>
		/// <param name="window"></param>
		/// <param name="mouseButton"></param>
		/// <param name="pressed"></param>
		static void MouseButtonCallback(const WindowingAPI::InternalWindow* window, Input::MouseButton mouseButton,
		                                bool pressed);
		static void ScrollCallback(const WindowingAPI::InternalWindow* window, double xOffset, double yOffset);
		static void KeyCallback(const WindowingAPI::InternalWindow* window, Input::Key key, bool pressed);
		static void CharCallback(const WindowingAPI::InternalWindow* window, uint32_t codePoint);
		static void WindowCloseCallback(const WindowingAPI::InternalWindow* window);
		static void WindowPosCallback(const WindowingAPI::InternalWindow* window, int32_t x, int32_t y);
		static void WindowSizeCallback(const WindowingAPI::InternalWindow* window, int32_t width, int32_t height);
		static void CreateWindow(ImGuiViewport* viewport);
		static void DestroyWindow(ImGuiViewport* viewport);
		static void ShowWindow(ImGuiViewport* viewport);
		static ImVec2 GetWindowPos(ImGuiViewport* viewport);
		static void SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
		static ImVec2 GetWindowSize(ImGuiViewport* viewport);
		static void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
		static void SetWindowTitle(ImGuiViewport* viewport, const char* title);
		static void SetWindowFocus(ImGuiViewport* viewport);
		static bool GetWindowFocus(ImGuiViewport* viewport);
		static bool GetWindowMinimized(ImGuiViewport* viewport);
		static void SetWindowAlpha(ImGuiViewport* viewport, float alpha);
		static void RenderWindow(ImGuiViewport* viewport, void* unused);
		static void SwapBuffers(ImGuiViewport* viewport, void* unused);
#ifdef TRAP_PLATFORM_WINDOWS
		static void SetIMEInputPos(ImGuiViewport* viewport, ImVec2 pos);
#endif
		static int32_t CreateVkSurface(ImGuiViewport* viewport, ImU64 vkInstance, const void* vkAllocator,
		                               ImU64* outVkSurface);
	};
}

#endif /*TRAP_IMGUIWINDOWING_H*/
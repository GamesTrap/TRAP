/*
The MIT License(MIT)

Copyright(c) 2014 - 2023 Omar Cornut

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

#ifndef TRAP_HEADLESS_MODE

#include <imgui.h>

#include "Core/PlatformDetection.h"
#include "Graphics/API/RendererAPI.h"

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
		[[nodiscard]] static bool Init(WindowingAPI::InternalWindow* window, bool installCallbacks,
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
		/// Set a custom cursor for ImGui.
		/// Note: This will make ImGui the owner of the cursor!
		/// </summary>
		/// <param name="cursor">Custom cursor to use.</param>
		static void SetCustomCursor(WindowingAPI::InternalCursor* cursor);
		/// <summary>
		/// Set to true to enable chaining installed callbacks for all windows
		/// (including secondary viewports created y backends or by user).
		///
		/// Note: This is false by default meaning we only chain callbacks for the main viewport.
		///       We cannot set this to true by default because user callbacks code may not be testing
		///       the window parameter of their callback.
		///       If you set this to true your user callback code will need to make sure you are testing
		///       the window parameter.
		/// </summary>
		/// <param name="chainForAllWindows">Whether to enable callback chaining or not.</param>
		static void SetCallbacksChainForAllWindows(bool chainForAllWindows);

		/// <summary>
		/// Install ImGui callbacks.
		/// </summary>
		/// <param name="window">Window to set callbacks for.</param>
		static void InstallCallbacks(WindowingAPI::InternalWindow* window);
		/// <summary>
		/// Restore old callbacks.
		/// </summary>
		/// <param name="window">Window to restore callbacks for.</param>
		static void RestoreCallbacks(WindowingAPI::InternalWindow* window);

		/// <summary>
		/// Check if the given window should chain callbacks.
		/// </summary>
		/// <param name="window">Window to check for chaining.</param>
		/// <returns>True or false.</returns>
		static bool ShouldChainCallback(const WindowingAPI::InternalWindow* window);

	private:
		inline constinit static std::string s_clipboardText{};

		/// <summary>
		/// Data struct for ImGui user data.
		/// </summary>
		struct ImGuiTRAPData
		{
			WindowingAPI::InternalWindow* Window{};
			Graphics::RenderAPI ClientAPI{};
			double Time{};
			const WindowingAPI::InternalWindow* MouseWindow{};
			std::array<WindowingAPI::InternalCursor*, ImGuiMouseCursor_COUNT> MouseCursors{};
			ImVec2 LastValidMousePos = ImVec2(0.0f, 0.0f);
			std::vector<const WindowingAPI::InternalWindow*> KeyOwnerWindows = std::vector<const WindowingAPI::InternalWindow*>(ToUnderlying(TRAP::Input::Key::Menu), nullptr);
			bool InstalledCallbacks{};
			bool CallbacksChainForAllWindows{};
			bool WantUpdateMonitors{};
 			WindowingAPI::InternalCursor* CustomCursor = nullptr;

			//Chain WindowingAPI callbacks; our callbacks will call the user's previously installed callbacks, if any.
			WindowingAPI::WindowFocusFunc PrevUserCallbackWindowFocus{};
			WindowingAPI::CursorPositionFunc PrevUserCallbackCursorPos{};
			WindowingAPI::CursorEnterFunc PrevUserCallbackCursorEnter{};
			WindowingAPI::MouseButtonFunc PrevUserCallbackMouseButton{};
			WindowingAPI::ScrollFunc PrevUserCallbackScroll{};
			WindowingAPI::KeyFunc PrevUserCallbackKey{};
			WindowingAPI::CharFunc PrevUserCallbackChar{};
			WindowingAPI::MonitorFunc PrevUserCallbackMonitor{};

			ImGuiTRAPData() = default;
		};

		/// <summary>
		/// Data struct for an ImGuiViewport.
		/// </summary>
		struct ImGuiViewportDataTRAP
		{
			WindowingAPI::InternalWindow* Window{};
			bool WindowOwned{};
			int32_t IgnoreWindowPosEventFrame = -1;
			int32_t IgnoreWindowSizeEventFrame = -1;

			/// <summary>
			/// Constructor.
			/// </summary>
			ImGuiViewportDataTRAP() noexcept = default;

			/// <summary>
			/// Destructor
			/// </summary>
			~ImGuiViewportDataTRAP()
			{
				IM_ASSERT(Window == nullptr);
			}

			ImGuiViewportDataTRAP(const ImGuiViewportDataTRAP& other) = default;
			ImGuiViewportDataTRAP(ImGuiViewportDataTRAP&& other) = default;

			ImGuiViewportDataTRAP& operator=(const ImGuiViewportDataTRAP& other) = default;
			ImGuiViewportDataTRAP& operator=(ImGuiViewportDataTRAP&& other) = default;
		};

		/// <summary>
		/// Window Focus callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="focused">Is window focused or not.</param>
		static void WindowFocusCallback(const WindowingAPI::InternalWindow& window, bool focused);
		/// <summary>
		/// Cursor enter callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="entered">Cursor entered or leaved window.</param>
		static void CursorEnterCallback(const WindowingAPI::InternalWindow& window, bool entered);
		/// <summary>
		/// Cursor position callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="xPos">Cursor x position of the window.</param>
		/// <param name="yPos">Cursor y position of the window.</param>
		static void CursorPosCallback(const WindowingAPI::InternalWindow& window, double xPos, double yPos);
		/// <summary>
		/// Mouse button callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="mouseButton">Affected mouse button.</param>
		/// <param name="state">Mouse button state (pressed or released).</param>
		static void MouseButtonCallback(const WindowingAPI::InternalWindow& window, Input::MouseButton mouseButton,
		                                Input::KeyState state);
		/// <summary>
		/// Scroll callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="xOffset">Horizontal mouse wheel offset.</param>
		/// <param name="yOffset">Vertical mouse wheel offset.</param>
		static void ScrollCallback(const WindowingAPI::InternalWindow& window, double xOffset, double yOffset);
		/// <summary>
		/// Key callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="key">Keyboard key.</param>
		/// <param name="state">Key state (pressed, repeated or released).</param>
		static void KeyCallback(const WindowingAPI::InternalWindow& window, Input::Key key, Input::KeyState state);
		/// <summary>
		/// Char callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="codePoint">UTF-32 code point.</param>
		static void CharCallback(const WindowingAPI::InternalWindow& window, uint32_t codePoint);
		/// <summary>
		/// Monitor callback.
		/// </summary>
		/// <param name="monitor">Affected monitor.</param>
		/// <param name="connected">Monitor connected or disconnected.</param>
		static void MonitorCallback(const WindowingAPI::InternalMonitor& monitor, bool connected);

		/// <summary>
		/// Get the backend data provided by user.
		/// </summary>
		/// <returns>Pointer to backend data.</returns>
		[[nodiscard]] static ImGuiTRAPData* GetBackendData();

		/// <summary>
		/// Get clipboard text.
		/// </summary>
		/// <param name="userData">Unused.</param>
		/// <returns>Clipboard content.</returns>
		[[nodiscard]] static const char* GetClipboardText(void* userData);
		/// <summary>
		/// Set the clipboard text.
		/// </summary>
		/// <param name="userData">Unused.</param>
		/// <param name="text">Text to set on clipboard.</param>
		static void SetClipboardText(void* userData, const char* text);

		/// <summary>
		/// Convert a TRAP::Input::Key into an ImGuiKey.
		/// </summary>
		/// <param name="key">Key to convert.</param>
		/// <returns>Converted key.</returns>
		[[nodiscard]] static ImGuiKey KeyToImGuiKey(Input::Key key) noexcept;

		/// <summary>
		/// Update modifier keys.
		/// </summary>
		/// <param name="window">Window to check modifier keys on.</param>
		static void UpdateKeyModifiers(const WindowingAPI::InternalWindow* window);

		/// <summary>
		/// Translate untranslated keys from the WindowingAPI
		/// back to translated keys to be used by ImGui:
		/// </summary>
		/// <param name="key">Key to translate.</param>
		/// <returns>Translated key.</returns>
		[[nodiscard]] static Input::Key TranslateUntranslateKey(Input::Key key);

		/// <summary>
		/// Update the ImGui mouse data.
		/// </summary>
		static void UpdateMouseData();
		/// <summary>
		/// Update the ImGui mouse cursor.
		/// </summary>
		static void UpdateMouseCursor();
		/// <summary>
		/// Update the ImGui gamepad data.
		/// </summary>
		static void UpdateGamepads();
		/// <summary>
		/// Update the ImGui monitor data.
		/// </summary>
		static void UpdateMonitors();

		//-------------------------------------------------------------------------------------------------------------------//
		//MULTI-VIEWPORT | PLATFORM INTERFACE SUPPORT
		//This is an _advanced_ and _optional_ feature,
		//allowing the back-end to create and handle multiple viewports simultaneously.
		//If you are new to dear imgui or creating a new binding for dear imgui,
		//it is recommended that you completely ignore this section first...
		//-------------------------------------------------------------------------------------------------------------------//

		/// <summary>
		/// Window close callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		static void WindowCloseCallback(const WindowingAPI::InternalWindow& window);
		/// <summary>
		/// Window position callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="xPos">New x position.</param>
		/// <param name="yPos">New y position.</param>
		static void WindowPosCallback(const WindowingAPI::InternalWindow& window, int32_t xPos, int32_t yPos);
		/// <summary>
		/// Window size callback.
		/// </summary>
		/// <param name="window">Affected window.</param>
		/// <param name="width">New width.</param>
		/// <param name="height">New height.</param>
		static void WindowSizeCallback(const WindowingAPI::InternalWindow& window, int32_t width, int32_t height);

		/// <summary>
		/// Create a new ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to create window for.</param>
		static void CreateWindow(ImGuiViewport* viewport);
		/// <summary>
		/// Destroy an ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to destroy window on.</param>
		static void DestroyWindow(ImGuiViewport* viewport);
		/// <summary>
		/// Show an ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to show window for.</param>
		static void ShowWindow(ImGuiViewport* viewport);
		/// <summary>
		/// Retrieve the ImGui Window position.
		/// </summary>
		/// <param name="viewport">Viewport to retrieve position for.</param>
		/// <returns>Window position.</returns>
		[[nodiscard]] static ImVec2 GetWindowPos(ImGuiViewport* viewport);
		/// <summary>
		/// Set the position for the ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to set window position on.</param>
		/// <param name="pos">Position to set.</param>
		static void SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
		/// <summary>
		/// Retrieve the ImGui Window size.
		/// </summary>
		/// <param name="viewport">Viewport to retrieve size for.</param>
		/// <returns>Window size.</returns>
		[[nodiscard]] static ImVec2 GetWindowSize(ImGuiViewport* viewport);
		/// <summary>
		/// Set the size for the ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to set window size on.</param>
		/// <param name="size">Size to set.</param>
		static void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
		/// <summary>
		/// Set the title for the ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to set window title on.</param>
		/// <param name="title">Title to set.</param>
		static void SetWindowTitle(ImGuiViewport* viewport, const char* title);
		/// <summary>
		/// Set focus for the ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to set window focus on.</param>
		static void SetWindowFocus(ImGuiViewport* viewport);
		/// <summary>
		/// Retrieve the ImGui Window focus state.
		/// </summary>
		/// <param name="viewport">Viewport to retrieve focus state for.</param>
		/// <returns>Focus state.</returns>
		[[nodiscard]] static bool GetWindowFocus(ImGuiViewport* viewport);
		/// <summary>
		/// Retrieve the ImGui Window minimization state.
		/// </summary>
		/// <param name="viewport">Viewport to retrieve minimization state for.</param>
		/// <returns>Minimization state.</returns>
		[[nodiscard]] static bool GetWindowMinimized(ImGuiViewport* viewport);
		/// <summary>
		/// Set the alpha value for the ImGui Window.
		/// </summary>
		/// <param name="viewport">Viewport to set alpha value on.</param>
		/// <param name="alpha">Alpha value.</param>
		static void SetWindowAlpha(ImGuiViewport* viewport, float alpha);

		//--------------------------------------------------------------------------------------------------------
		// Vulkan support (the Vulkan renderer needs to call a platform-side support function to create the surface)
		//--------------------------------------------------------------------------------------------------------

		/// <summary>
		/// Create a Vulkan surface for ImGui.
		/// </summary>
		/// <param name="viewport">Viewport to create surface for.</param>
		/// <param name="vkInstance">Vulkan instance.</param>
		/// <param name="vkAllocator">Vulkan allocator.</param>
		/// <param name="outVkSurface">Output for Vulkan surface.</param>
		/// <returns>Vulkan error code.</returns>
		[[nodiscard]] static int32_t CreateVkSurface(ImGuiViewport* viewport, ImU64 vkInstance,
		                                             const void* vkAllocator, ImU64* outVkSurface);

		/// <summary>
		/// Initialize the platform interface.
		/// </summary>
		static void InitPlatformInterface();
		/// <summary>
		/// Shutdown the platform interface.
		/// </summary>
		static void ShutdownPlatformInterface();
	};
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_IMGUIWINDOWING_H*/
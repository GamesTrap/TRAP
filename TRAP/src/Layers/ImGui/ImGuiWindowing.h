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
		inline static std::string s_clipboardText{};

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
			std::vector<const WindowingAPI::InternalWindow*> KeyOwnerWindows = std::vector<const WindowingAPI::InternalWindow*>(std::to_underlying(TRAP::Input::Key::Menu), nullptr);
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
			constexpr ImGuiViewportDataTRAP() noexcept = default;

			/// <summary>
			/// Destructor
			/// </summary>
			~ImGuiViewportDataTRAP()
			{
				IM_ASSERT(Window == nullptr);
			}

			constexpr ImGuiViewportDataTRAP(const ImGuiViewportDataTRAP& other) = default;
			constexpr ImGuiViewportDataTRAP(ImGuiViewportDataTRAP&& other) = default;

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
		[[nodiscard]] static constexpr ImGuiKey KeyToImGuiKey(Input::Key key) noexcept;

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

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr ImGuiKey TRAP::INTERNAL::ImGuiWindowing::KeyToImGuiKey(const TRAP::Input::Key key) noexcept
{
	switch(key)
	{
	case TRAP::Input::Key::Tab: return ImGuiKey_Tab;
	case TRAP::Input::Key::Left: return ImGuiKey_LeftArrow;
	case TRAP::Input::Key::Right: return ImGuiKey_RightArrow;
	case TRAP::Input::Key::Up: return ImGuiKey_UpArrow;
	case TRAP::Input::Key::Down: return ImGuiKey_DownArrow;
	case TRAP::Input::Key::Page_Up: return ImGuiKey_PageUp;
	case TRAP::Input::Key::Page_Down: return ImGuiKey_PageDown;
	case TRAP::Input::Key::Home: return ImGuiKey_Home;
	case TRAP::Input::Key::End: return ImGuiKey_End;
	case TRAP::Input::Key::Insert: return ImGuiKey_Insert;
	case TRAP::Input::Key::Delete: return ImGuiKey_Delete;
	case TRAP::Input::Key::Backspace: return ImGuiKey_Backspace;
	case TRAP::Input::Key::Space: return ImGuiKey_Space;
	case TRAP::Input::Key::Enter: return ImGuiKey_Enter;
	case TRAP::Input::Key::Escape: return ImGuiKey_Escape;
	case TRAP::Input::Key::Apostrophe: return ImGuiKey_Apostrophe;
	case TRAP::Input::Key::Comma: return ImGuiKey_Comma;
	case TRAP::Input::Key::Minus: return ImGuiKey_Minus;
	case TRAP::Input::Key::Period: return ImGuiKey_Period;
	case TRAP::Input::Key::Slash: return ImGuiKey_Slash;
	case TRAP::Input::Key::Semicolon: return ImGuiKey_Semicolon;
	case TRAP::Input::Key::Equal: return ImGuiKey_Equal;
	case TRAP::Input::Key::Left_Bracket: return ImGuiKey_LeftBracket;
	case TRAP::Input::Key::Backslash: return ImGuiKey_Backslash;
	case TRAP::Input::Key::Right_Bracket: return ImGuiKey_RightBracket;
	case TRAP::Input::Key::Grave_Accent: return ImGuiKey_GraveAccent;
	case TRAP::Input::Key::Caps_Lock: return ImGuiKey_CapsLock;
	case TRAP::Input::Key::Scroll_Lock: return ImGuiKey_ScrollLock;
	case TRAP::Input::Key::Num_Lock: return ImGuiKey_NumLock;
	case TRAP::Input::Key::Print_Screen: return ImGuiKey_PrintScreen;
	case TRAP::Input::Key::Pause: return ImGuiKey_Pause;
	case TRAP::Input::Key::KP_0: return ImGuiKey_Keypad0;
	case TRAP::Input::Key::KP_1: return ImGuiKey_Keypad1;
	case TRAP::Input::Key::KP_2: return ImGuiKey_Keypad2;
	case TRAP::Input::Key::KP_3: return ImGuiKey_Keypad3;
	case TRAP::Input::Key::KP_4: return ImGuiKey_Keypad4;
	case TRAP::Input::Key::KP_5: return ImGuiKey_Keypad5;
	case TRAP::Input::Key::KP_6: return ImGuiKey_Keypad6;
	case TRAP::Input::Key::KP_7: return ImGuiKey_Keypad7;
	case TRAP::Input::Key::KP_8: return ImGuiKey_Keypad8;
	case TRAP::Input::Key::KP_9: return ImGuiKey_Keypad9;
	case TRAP::Input::Key::KP_Decimal: return ImGuiKey_KeypadDecimal;
	case TRAP::Input::Key::KP_Divide: return ImGuiKey_KeypadDivide;
	case TRAP::Input::Key::KP_Multiply: return ImGuiKey_KeypadMultiply;
	case TRAP::Input::Key::KP_Subtract: return ImGuiKey_KeypadSubtract;
	case TRAP::Input::Key::KP_Add: return ImGuiKey_KeypadAdd;
	case TRAP::Input::Key::KP_Enter: return ImGuiKey_KeypadEnter;
	case TRAP::Input::Key::KP_Equal: return ImGuiKey_KeypadEqual;
	case TRAP::Input::Key::Left_Shift: return ImGuiKey_LeftShift;
	case TRAP::Input::Key::Left_Control: return ImGuiKey_LeftCtrl;
	case TRAP::Input::Key::Left_ALT: return ImGuiKey_LeftAlt;
	case TRAP::Input::Key::Left_Super: return ImGuiKey_LeftSuper;
	case TRAP::Input::Key::Right_Shift: return ImGuiKey_RightShift;
	case TRAP::Input::Key::Right_Control: return ImGuiKey_RightCtrl;
	case TRAP::Input::Key::Right_ALT: return ImGuiKey_RightAlt;
	case TRAP::Input::Key::Right_Super: return ImGuiKey_RightSuper;
	case TRAP::Input::Key::Menu: return ImGuiKey_Menu;
	case TRAP::Input::Key::Zero: return ImGuiKey_0;
	case TRAP::Input::Key::One: return ImGuiKey_1;
	case TRAP::Input::Key::Two: return ImGuiKey_2;
	case TRAP::Input::Key::Three: return ImGuiKey_3;
	case TRAP::Input::Key::Four: return ImGuiKey_4;
	case TRAP::Input::Key::Five: return ImGuiKey_5;
	case TRAP::Input::Key::Six: return ImGuiKey_6;
	case TRAP::Input::Key::Seven: return ImGuiKey_7;
	case TRAP::Input::Key::Eight: return ImGuiKey_8;
	case TRAP::Input::Key::Nine: return ImGuiKey_9;
	case TRAP::Input::Key::A: return ImGuiKey_A;
	case TRAP::Input::Key::B: return ImGuiKey_B;
	case TRAP::Input::Key::C: return ImGuiKey_C;
	case TRAP::Input::Key::D: return ImGuiKey_D;
	case TRAP::Input::Key::E: return ImGuiKey_E;
	case TRAP::Input::Key::F: return ImGuiKey_F;
	case TRAP::Input::Key::G: return ImGuiKey_G;
	case TRAP::Input::Key::H: return ImGuiKey_H;
	case TRAP::Input::Key::I: return ImGuiKey_I;
	case TRAP::Input::Key::J: return ImGuiKey_J;
	case TRAP::Input::Key::K: return ImGuiKey_K;
	case TRAP::Input::Key::L: return ImGuiKey_L;
	case TRAP::Input::Key::M: return ImGuiKey_M;
	case TRAP::Input::Key::N: return ImGuiKey_N;
	case TRAP::Input::Key::O: return ImGuiKey_O;
	case TRAP::Input::Key::P: return ImGuiKey_P;
	case TRAP::Input::Key::Q: return ImGuiKey_Q;
	case TRAP::Input::Key::R: return ImGuiKey_R;
	case TRAP::Input::Key::S: return ImGuiKey_S;
	case TRAP::Input::Key::T: return ImGuiKey_T;
	case TRAP::Input::Key::U: return ImGuiKey_U;
	case TRAP::Input::Key::V: return ImGuiKey_V;
	case TRAP::Input::Key::W: return ImGuiKey_W;
	case TRAP::Input::Key::X: return ImGuiKey_X;
	case TRAP::Input::Key::Y: return ImGuiKey_Y;
	case TRAP::Input::Key::Z: return ImGuiKey_Z;
	case TRAP::Input::Key::F1: return ImGuiKey_F1;
	case TRAP::Input::Key::F2: return ImGuiKey_F2;
	case TRAP::Input::Key::F3: return ImGuiKey_F3;
	case TRAP::Input::Key::F4: return ImGuiKey_F4;
	case TRAP::Input::Key::F5: return ImGuiKey_F5;
	case TRAP::Input::Key::F6: return ImGuiKey_F6;
	case TRAP::Input::Key::F7: return ImGuiKey_F7;
	case TRAP::Input::Key::F8: return ImGuiKey_F8;
	case TRAP::Input::Key::F9: return ImGuiKey_F9;
	case TRAP::Input::Key::F10: return ImGuiKey_F10;
	case TRAP::Input::Key::F11: return ImGuiKey_F11;
	case TRAP::Input::Key::F12: return ImGuiKey_F12;

	default:
		return ImGuiKey_None;
	}
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_IMGUIWINDOWING_H*/
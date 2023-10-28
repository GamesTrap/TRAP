/*The MIT License(MIT)

Copyright(c) 2014 - 2023 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
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

#include "TRAPPCH.h"
#include "ImGuiWindowing.h"

#ifndef TRAP_HEADLESS_MODE

#include "Application.h"
#include "Maths/Math.h"

namespace
{
	/// <summary>
	/// Data struct for an ImGuiViewport.
	/// </summary>
	struct ImGuiViewportDataTRAP
	{
		TRAP::INTERNAL::WindowingAPI::InternalWindow* Window{};
		bool WindowOwned{};
		std::optional<i32> IgnoreWindowPosEventFrame = std::nullopt;
		std::optional<i32> IgnoreWindowSizeEventFrame = std::nullopt;

		/// <summary>
		/// Constructor.
		/// </summary>
		constexpr ImGuiViewportDataTRAP() noexcept = default;

		/// <summary>
		/// Destructor
		/// </summary>
		~ImGuiViewportDataTRAP()
		{
			TRAP_ASSERT(Window == nullptr, "ImGuiViewportDataTRAP: Window is not nullptr!");
		}

		constexpr ImGuiViewportDataTRAP(const ImGuiViewportDataTRAP& other) = default;
		constexpr ImGuiViewportDataTRAP(ImGuiViewportDataTRAP&& other) = default;

		ImGuiViewportDataTRAP& operator=(const ImGuiViewportDataTRAP& other) = default;
		ImGuiViewportDataTRAP& operator=(ImGuiViewportDataTRAP&& other) = default;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Data struct for ImGui user data.
	/// </summary>
	struct ImGuiTRAPData
	{
		TRAP::INTERNAL::WindowingAPI::InternalWindow* Window{};
		TRAP::Graphics::RenderAPI ClientAPI{};
		f64 Time{};
		const TRAP::INTERNAL::WindowingAPI::InternalWindow* MouseWindow{};
		std::array<TRAP::INTERNAL::WindowingAPI::InternalCursor*, ImGuiMouseCursor_COUNT> MouseCursors{};
		ImVec2 LastValidMousePos = ImVec2(0.0f, 0.0f);
		std::vector<const TRAP::INTERNAL::WindowingAPI::InternalWindow*> KeyOwnerWindows = std::vector<const TRAP::INTERNAL::WindowingAPI::InternalWindow*>(std::to_underlying(TRAP::Input::Key::Menu), nullptr);
		bool InstalledCallbacks{};
		bool CallbacksChainForAllWindows{};
		bool WantUpdateMonitors{};
		TRAP::INTERNAL::WindowingAPI::InternalCursor* CustomCursor = nullptr;

		//Chain WindowingAPI callbacks; our callbacks will call the user's previously installed callbacks, if any.
		TRAP::INTERNAL::WindowingAPI::WindowFocusFunc PrevUserCallbackWindowFocus{};
		TRAP::INTERNAL::WindowingAPI::CursorPositionFunc PrevUserCallbackCursorPos{};
		TRAP::INTERNAL::WindowingAPI::CursorEnterFunc PrevUserCallbackCursorEnter{};
		TRAP::INTERNAL::WindowingAPI::MouseButtonFunc PrevUserCallbackMouseButton{};
		TRAP::INTERNAL::WindowingAPI::ScrollFunc PrevUserCallbackScroll{};
		TRAP::INTERNAL::WindowingAPI::KeyFunc PrevUserCallbackKey{};
		TRAP::INTERNAL::WindowingAPI::CharFunc PrevUserCallbackChar{};
		TRAP::INTERNAL::WindowingAPI::MonitorFunc PrevUserCallbackMonitor{};

		constexpr ImGuiTRAPData() = default;
	};

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Get the backend data provided by user.
	///
	/// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui context
	/// It is STRONGLY preferred that you use docking branch with multi-viewports
	/// (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
	/// </summary>
	/// <returns>Pointer to backend data.</returns>
	[[nodiscard]] ImGuiTRAPData* GetBackendData()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		const ImGuiContext* context = ImGui::GetCurrentContext();
		const ImGuiIO& io = ImGui::GetIO();

        TRAP_ASSERT(context != nullptr, "ImGuiWindowing::GetBackendData(): ImGui context is nullptr!");
        TRAP_ASSERT(io.BackendPlatformUserData != nullptr, "ImGuiWindowing::GetBackendData(): BackendPlatformUserData is nullptr!");

		return context != nullptr ? static_cast<ImGuiTRAPData*>(io.BackendPlatformUserData) : nullptr;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Update modifier keys.
	/// </summary>
	/// <param name="window">Window to check modifier keys on.</param>
	void UpdateKeyModifiers(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(ImGuiMod_Ctrl, TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Left_Control) != TRAP::Input::KeyState::Released ||
									  TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Right_Control) != TRAP::Input::KeyState::Released);
		io.AddKeyEvent(ImGuiMod_Shift, TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Left_Shift) != TRAP::Input::KeyState::Released ||
									   TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Right_Shift) != TRAP::Input::KeyState::Released);
		io.AddKeyEvent(ImGuiMod_Alt, TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Left_ALT) != TRAP::Input::KeyState::Released ||
									 TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Right_ALT) != TRAP::Input::KeyState::Released);
		io.AddKeyEvent(ImGuiMod_Super, TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Left_Super) != TRAP::Input::KeyState::Released ||
									   TRAP::INTERNAL::WindowingAPI::GetKey(window, TRAP::Input::Key::Right_Super) != TRAP::Input::KeyState::Released);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Translate untranslated keys from the WindowingAPI
	/// back to translated keys to be used by ImGui:
	/// </summary>
	/// <param name="key">Key to translate.</param>
	/// <returns>Translated key.</returns>
	[[nodiscard]] TRAP::Input::Key TranslateUntranslateKey(TRAP::Input::Key key)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		if(key >= TRAP::Input::Key::KP_0 && key <= TRAP::Input::Key::KP_Equal)
			return key;
		const std::string keyName = TRAP::Input::GetKeyName(key);
		if(!keyName.empty() && keyName[0] != 0 && keyName[1] == 0)
		{
			static constexpr std::array<char, 11> charNames{'`', '-', '=', '[', ']', '\\', ',', ';', '\'', '.', '/'};
			static constexpr std::array<TRAP::Input::Key, 11> charKeys
			{
				TRAP::Input::Key::Grave_Accent, TRAP::Input::Key::Minus,
				TRAP::Input::Key::Equal, TRAP::Input::Key::Left_Bracket,
				TRAP::Input::Key::Right_Bracket, TRAP::Input::Key::Backslash,
				TRAP::Input::Key::Comma, TRAP::Input::Key::Semicolon,
				TRAP::Input::Key::Apostrophe, TRAP::Input::Key::Period,
				TRAP::Input::Key::Slash
			};
			const std::array<char, 11>::const_iterator it = std::ranges::find(charNames, keyName[0]);
			if(keyName[0] >= '0' && keyName[0] <= '9')
				key = static_cast<TRAP::Input::Key>(std::to_underlying(TRAP::Input::Key::Zero) + (keyName[0] - '0'));
			else if(keyName[0] >= 'A' && keyName[0] <= 'Z')
				key = static_cast<TRAP::Input::Key>(std::to_underlying(TRAP::Input::Key::A) + (keyName[0] - 'A'));
			else if(keyName[0] >= 'a' && keyName[0] <= 'z')
				key = static_cast<TRAP::Input::Key>(std::to_underlying(TRAP::Input::Key::A) + (keyName[0] - 'a'));
			else if(it != charNames.cend())
				key = charKeys[NumericCast<usize>(it - charNames.cbegin())];
		}

		return key;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a TRAP::Input::Key into an ImGuiKey.
	/// </summary>
	/// <param name="key">Key to convert.</param>
	/// <returns>Converted key.</returns>
	[[nodiscard]] constexpr ImGuiKey KeyToImGuiKey(const TRAP::Input::Key key) noexcept
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
		case TRAP::Input::Key::F13: return ImGuiKey_F13;
		case TRAP::Input::Key::F14: return ImGuiKey_F14;
		case TRAP::Input::Key::F15: return ImGuiKey_F15;
		case TRAP::Input::Key::F16: return ImGuiKey_F16;
		case TRAP::Input::Key::F17: return ImGuiKey_F17;
		case TRAP::Input::Key::F18: return ImGuiKey_F18;
		case TRAP::Input::Key::F19: return ImGuiKey_F19;
		case TRAP::Input::Key::F20: return ImGuiKey_F20;
		case TRAP::Input::Key::F21: return ImGuiKey_F21;
		case TRAP::Input::Key::F22: return ImGuiKey_F22;
		case TRAP::Input::Key::F23: return ImGuiKey_F23;
		case TRAP::Input::Key::F24: return ImGuiKey_F24;

		default:
			return ImGuiKey_None;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Window Focus callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="focused">Is window focused or not.</param>
	void WindowFocusCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window, const bool focused)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		const ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackWindowFocus != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackWindowFocus(window, focused);

		ImGuiIO& io = ImGui::GetIO();
		io.AddFocusEvent(focused);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Cursor enter callback.
	///
	/// Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
	/// so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="entered">Cursor entered or leaved window.</param>
	void CursorEnterCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window, const bool entered)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackCursorEnter != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackCursorEnter(window, entered);

		ImGuiIO& io = ImGui::GetIO();
		if(entered)
		{
			bd->MouseWindow = &window;
			io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
		}
		else if(!entered && bd->MouseWindow == &window)
		{
			bd->LastValidMousePos = io.MousePos;
			bd->MouseWindow = nullptr;
			io.AddMousePosEvent(-std::numeric_limits<f32>::max(), -std::numeric_limits<f32>::max());
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Cursor position callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="xPos">Cursor x position of the window.</param>
	/// <param name="yPos">Cursor y position of the window.</param>
	void CursorPosCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window, f64 xPos, f64 yPos)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackCursorPos != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackCursorPos(window, xPos, yPos);

		ImGuiIO& io = ImGui::GetIO();
		if((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
		{
			i32 windowX = 0, windowY = 0;
			TRAP::INTERNAL::WindowingAPI::GetWindowPos(window, windowX, windowY);
			xPos += windowX;
			yPos += windowY;
		}
		bd->LastValidMousePos = ImVec2(NumericCast<f32>(xPos), NumericCast<f32>(yPos));
		io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Mouse button callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="mouseButton">Affected mouse button.</param>
	/// <param name="state">Mouse button state (pressed or released).</param>
	void MouseButtonCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window,
	                         const TRAP::Input::MouseButton mouseButton, const TRAP::Input::KeyState state)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		const ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackMouseButton != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackMouseButton(window, mouseButton, state);

		TRAP_ASSERT(bd->Window != nullptr, "ImGuiWindowing::MouseButtonCallback(): bd->Window is nullptr!");
		UpdateKeyModifiers(*bd->Window);

		ImGuiIO& io = ImGui::GetIO();
		if(std::to_underlying(mouseButton) < ImGuiMouseButton_COUNT)
			io.AddMouseButtonEvent(NumericCast<i32>(std::to_underlying(mouseButton)), (state == TRAP::Input::KeyState::Pressed));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Scroll callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="xOffset">Horizontal mouse wheel offset.</param>
	/// <param name="yOffset">Vertical mouse wheel offset.</param>
	void ScrollCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window, const f64 xOffset,
	                    const f64 yOffset)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		const ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackScroll != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackScroll(window, xOffset, yOffset);

		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(NumericCast<f32>(xOffset), NumericCast<f32>(yOffset));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Key callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="key">Keyboard key.</param>
	/// <param name="state">Key state (pressed, repeated or released).</param>
	void KeyCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window, TRAP::Input::Key key,
	                 const TRAP::Input::KeyState state)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackKey != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackKey(window, key, state);

		if(state != TRAP::Input::KeyState::Pressed && state != TRAP::Input::KeyState::Released)
			return;

		TRAP_ASSERT(bd->Window != nullptr, "ImGuiWindowing::KeyCallback(): bd->Window is nullptr!");
		UpdateKeyModifiers(*bd->Window);

		if(std::to_underlying(key) >= 0 && std::to_underlying(key) < NumericCast<i32>(bd->KeyOwnerWindows.size()))
			bd->KeyOwnerWindows[NumericCast<usize>(std::to_underlying(key))] = (state == TRAP::Input::KeyState::Pressed) ? &window : nullptr;

		key = TranslateUntranslateKey(key);

		ImGuiIO& io = ImGui::GetIO();
		const ImGuiKey imguiKey = KeyToImGuiKey(key);
		io.AddKeyEvent(imguiKey, (state == TRAP::Input::KeyState::Pressed));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Char callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="codePoint">UTF-32 code point.</param>
	void CharCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window, const u32 codePoint)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		const ImGuiTRAPData* const bd = GetBackendData();
		if (bd->PrevUserCallbackChar != nullptr && TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(&window))
			bd->PrevUserCallbackChar(window, codePoint);

		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(codePoint);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Monitor callback.
	/// </summary>
	/// <param name="monitor">Affected monitor.</param>
	/// <param name="connected">Monitor connected or disconnected.</param>
	void MonitorCallback([[maybe_unused]] const TRAP::INTERNAL::WindowingAPI::InternalMonitor& monitor,
	                     [[maybe_unused]] const bool connected)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

		ImGuiTRAPData* const bd = GetBackendData();
		bd->WantUpdateMonitors = true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	std::string ClipboardText{};

	/// <summary>
	/// Get clipboard text.
	/// </summary>
	/// <param name="userData">Unused.</param>
	/// <returns>Clipboard content.</returns>
	[[nodiscard]] const char* GetClipboardText([[maybe_unused]] void* const userData)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ClipboardText = TRAP::INTERNAL::WindowingAPI::GetClipboardString();
		return ClipboardText.c_str();
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Set the clipboard text.
	/// </summary>
	/// <param name="userData">Unused.</param>
	/// <param name="text">Text to set on clipboard.</param>
	void SetClipboardText([[maybe_unused]] void* const userData, const char* const text)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP::INTERNAL::WindowingAPI::SetClipboardString(text);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Update the ImGui mouse data.
	/// </summary>
	void UpdateMouseData()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiTRAPData* const bd = GetBackendData();
		ImGuiIO& io = ImGui::GetIO();
		const ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

		ImGuiID mouseViewportID = 0;
		const ImVec2 mousePosPrev = io.MousePos;
		for(const ImGuiViewport* const viewport : platformIO.Viewports)
		{
			TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::UpdateMouseData(): viewport is nullptr!");
			if(viewport == nullptr)
				continue;

			TRAP::INTERNAL::WindowingAPI::InternalWindow* const window = static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(viewport->PlatformHandle);
			TRAP_ASSERT(window != nullptr, "ImGuiWindowing::UpdateMouseData(): window is nullptr!");
			if(window == nullptr)
				continue;

			const bool isWindowFocused = TRAP::INTERNAL::WindowingAPI::GetWindowHint(*window, TRAP::INTERNAL::WindowingAPI::Hint::Focused);

			if(isWindowFocused)
			{
				//(Optional) Set OS mouse position from Dear ImGui if requested
				//(rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by used)
				//When multi-viewports are enabled, all Dear ImGui positions are same as OS positions.
				if(io.WantSetMousePos)
					TRAP::INTERNAL::WindowingAPI::SetCursorPos(*window, NumericCast<f64>(mousePosPrev.x - viewport->Pos.x),
														       NumericCast<f64>(mousePosPrev.y - viewport->Pos.y));

				//(Optional) Fallback to provide mouse position when focused (CursorPosCallback already provides this when hovered or captured)
				if(bd->MouseWindow == nullptr)
				{
					f64 mouseX = 0.0, mouseY = 0.0;
					TRAP::INTERNAL::WindowingAPI::GetCursorPos(*window, mouseX, mouseY);
					if((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
					{
						//Single-viewport mode: mouse position in client window coordinates
						//(io.MousePos is (0, 0) when the mouse is on the upper-left corner of the app window)
						//Multi-viewport mode: mouse position in OS absolute coordinates
						//(io.MousePos is (0, 0) when the mouse is on the upper-left of the primary monitor)
						i32 windowX = 0, windowY = 0;
						TRAP::INTERNAL::WindowingAPI::GetWindowPos(*window, windowX, windowY);
						mouseX += windowX;
						mouseY += windowY;
					}

					bd->LastValidMousePos = ImVec2(NumericCast<f32>(mouseX), NumericCast<f32>(mouseY));
					io.AddMousePosEvent(bd->LastValidMousePos.x, bd->LastValidMousePos.y);
				}
			}

			//(Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport
			//the OS mouse cursor is hovering.
			//If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear ImGui will ignore this field
			//and infer the information using its flawed heuristic.
			const bool windowNoInput = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;
			TRAP::INTERNAL::WindowingAPI::SetWindowHint(*window, TRAP::INTERNAL::WindowingAPI::Hint::MousePassthrough, windowNoInput);
			if(TRAP::INTERNAL::WindowingAPI::GetWindowHint(*window, TRAP::INTERNAL::WindowingAPI::Hint::Hovered) && !windowNoInput)
				mouseViewportID = viewport->ID;
		}

		if((io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport) != 0)
			io.AddMouseViewportEvent(mouseViewportID);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Update the ImGui mouse cursor.
	/// </summary>
	void UpdateMouseCursor()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		const ImGuiIO& io = ImGui::GetIO();
		const ImGuiTRAPData* const bd = GetBackendData();
		if (((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) != 0) ||
			TRAP::INTERNAL::WindowingAPI::GetCursorMode(*bd->Window) == TRAP::INTERNAL::WindowingAPI::CursorMode::Disabled)
			return;

		const ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
		const ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		for(const ImGuiViewport* viewport : platformIO.Viewports)
		{
			TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::UpdateMouseCursor(): viewport is nullptr!");
			if(viewport == nullptr)
				continue;

			TRAP::INTERNAL::WindowingAPI::InternalWindow* const windowPtr = static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>
			(
				viewport->PlatformHandle
			);
			TRAP_ASSERT(windowPtr != nullptr, "ImGuiWindowing::UpdateMouseCursor(): windowPtr is nullptr!");
			if(windowPtr == nullptr)
				continue;

			if(imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
			{
				//Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
				TRAP::INTERNAL::WindowingAPI::SetCursorMode(*windowPtr, TRAP::INTERNAL::WindowingAPI::CursorMode::Hidden);
			}
			else
			{
				if(imguiCursor != ImGuiMouseCursor_Arrow)
				{
					//Show OS mouse cursor
					TRAP::INTERNAL::WindowingAPI::SetCursor(*windowPtr, bd->MouseCursors[NumericCast<usize>(imguiCursor)] != nullptr ?
														    bd->MouseCursors[NumericCast<usize>(imguiCursor)] :
														    std::get<ImGuiMouseCursor_Arrow>(bd->MouseCursors));
					TRAP::INTERNAL::WindowingAPI::SetCursorMode(*windowPtr, TRAP::INTERNAL::WindowingAPI::CursorMode::Normal);
				}
				else
				{
					if(bd->CustomCursor != nullptr)
						TRAP::INTERNAL::WindowingAPI::SetCursor(*windowPtr, bd->CustomCursor);
					else
						TRAP::INTERNAL::WindowingAPI::SetCursor(*windowPtr, std::get<ImGuiMouseCursor_Arrow>(bd->MouseCursors));
				}
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Update the ImGui gamepad data.
	/// </summary>
	void UpdateGamepads()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiIO& io = ImGui::GetIO();
		if((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
			return;

		io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
		if(!TRAP::Input::IsControllerConnected(TRAP::Input::Controller::One))
			return;

		#define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED) { io.AddKeyEvent(KEY_NO, TRAP::Input::IsControllerButtonPressed(TRAP::Input::Controller::One, BUTTON_NO)); }
		#define MAP_DPAD(KEY_NO, DPAD_NO, _UNUSED) { io.AddKeyEvent(KEY_NO, static_cast<bool>(TRAP::Input::GetControllerDPad(TRAP::Input::Controller::One, 0) & (DPAD_NO))); }
		#define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1) { f32 v = TRAP::Input::GetControllerAxis(TRAP::Input::Controller::One, AXIS_NO); v = (v - (V0)) / ((V1) - (V0)); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, TRAP::Math::Clamp(v, 0.0f, 1.0f)); }

		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		MAP_BUTTON(ImGuiKey_GamepadStart, TRAP::Input::ControllerButton::Start, 7);
		MAP_BUTTON(ImGuiKey_GamepadBack, TRAP::Input::ControllerButton::Back, 6);
		MAP_BUTTON(ImGuiKey_GamepadFaceDown, TRAP::Input::ControllerButton::A, 0); //XBox A, PS Cross
		MAP_BUTTON(ImGuiKey_GamepadFaceRight, TRAP::Input::ControllerButton::B, 1); //XBox B, PS Circle
		MAP_BUTTON(ImGuiKey_GamepadFaceLeft, TRAP::Input::ControllerButton::X, 2); //XBox X, PS Square
		MAP_BUTTON(ImGuiKey_GamepadFaceUp, TRAP::Input::ControllerButton::Y, 3); //XBox Y, PS Triangle
		MAP_DPAD(ImGuiKey_GamepadDpadLeft, TRAP::Input::ControllerDPad::Left, 13);
		MAP_DPAD(ImGuiKey_GamepadDpadRight, TRAP::Input::ControllerDPad::Right, 11);
		MAP_DPAD(ImGuiKey_GamepadDpadUp, TRAP::Input::ControllerDPad::Up, 10);
		MAP_DPAD(ImGuiKey_GamepadDpadDown, TRAP::Input::ControllerDPad::Down, 12);
		MAP_BUTTON(ImGuiKey_GamepadL1, TRAP::Input::ControllerButton::Left_Bumper, 4);
		MAP_BUTTON(ImGuiKey_GamepadR1, TRAP::Input::ControllerButton::Right_Bumper, 5);
		MAP_ANALOG(ImGuiKey_GamepadL2, TRAP::Input::ControllerAxis::Left_Trigger, 4, -0.75f, +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadR2, TRAP::Input::ControllerAxis::Right_Trigger, 5, -0.75f, +1.0f);
		MAP_BUTTON(ImGuiKey_GamepadL3, TRAP::Input::ControllerButton::Left_Thumb, 8);
		MAP_BUTTON(ImGuiKey_GamepadR3, TRAP::Input::ControllerButton::Right_Thumb, 9);
		MAP_ANALOG(ImGuiKey_GamepadLStickLeft, TRAP::Input::ControllerAxis::Left_X, 0, -0.25f, -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadLStickRight, TRAP::Input::ControllerAxis::Left_X, 0, +0.25f, +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadLStickUp, TRAP::Input::ControllerAxis::Left_Y, 1, -0.25f, -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadLStickDown, TRAP::Input::ControllerAxis::Left_Y, 1, +0.25f, +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickLeft, TRAP::Input::ControllerAxis::Right_X, 2, -0.25f, -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickRight, TRAP::Input::ControllerAxis::Right_X, 2, +0.25f, +1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickUp, TRAP::Input::ControllerAxis::Right_Y, 3, -0.25f, -1.0f);
		MAP_ANALOG(ImGuiKey_GamepadRStickDown, TRAP::Input::ControllerAxis::Right_Y, 3, +0.25f, +1.0f);

		#undef MAP_BUTTON
		#undef MAP_DPAD
		#undef MAP_ANALOG
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Update the ImGui monitor data.
	/// </summary>
	void UpdateMonitors()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGuiTRAPData* const bd = GetBackendData();
		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		const auto& monitors = TRAP::INTERNAL::WindowingAPI::GetMonitors();
		platformIO.Monitors.resize(0);
		bd->WantUpdateMonitors = false;
		for (TRAP::INTERNAL::WindowingAPI::InternalMonitor* n : monitors)
		{
			if(n == nullptr)
				continue;

			ImGuiPlatformMonitor monitor;
			i32 x = 0, y = 0;
			TRAP::INTERNAL::WindowingAPI::GetMonitorPos(*n, x, y);
			const std::optional<TRAP::INTERNAL::WindowingAPI::InternalVideoMode> videoMode = TRAP::INTERNAL::WindowingAPI::GetVideoMode(*n);
			if(!videoMode)
				continue; // Failed to get Video mode
			monitor.MainPos = monitor.WorkPos = ImVec2(NumericCast<f32>(x), NumericCast<f32>(y));
			monitor.MainSize = monitor.WorkSize = ImVec2(NumericCast<f32>(videoMode->Width), NumericCast<f32>(videoMode->Height));
			i32 width = 0, height = 0;
			TRAP::INTERNAL::WindowingAPI::GetMonitorWorkArea(*n, x, y, width, height);
			if(width > 0 && height > 0)
			{
				monitor.WorkPos = ImVec2(NumericCast<f32>(x), NumericCast<f32>(y));
				monitor.WorkSize = ImVec2(NumericCast<f32>(width), NumericCast<f32>(height));
			}

			//Warning: The validity of monitor DPI information on Windows depends on the application
			//DPI awareness settings,
			//which generally needs to be set in the manifest or at runtime.
			f32 xScale = 0.0f, yScale = 0.0f;
			TRAP::INTERNAL::WindowingAPI::GetMonitorContentScale(*n, xScale, yScale);
			monitor.DpiScale = xScale;

			platformIO.Monitors.push_back(monitor);
		}
	}

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
	void WindowCloseCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
		ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
		(
			const_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(&window)
		);

		if (viewport != nullptr)
			viewport->PlatformRequestClose = true;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Window position callback.
	///
	/// WindowingAPI may dispatch window pos/size events after calling SetWindowPos()/SetWindowSize().
	/// However: depending on the platform the callback may be invoked at different time:
	///  - on Windows it appears to be called within the SetWindowPos()/SetWindowSize() call
	///  - on Linux it is queued and invoked during PollEvents()
	/// Because the event doesn't always fire on SetWindowXXX() we use a frame counter tag to only
	/// ignore recent SetWindowXXX() calls.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="xPos">New x position.</param>
	/// <param name="yPos">New y position.</param>
	void WindowPosCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window,
	                       [[maybe_unused]] const i32 xPos, [[maybe_unused]] const i32 yPos)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
		ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
		(
			const_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(&window)
		);

		if (viewport != nullptr)
		{
			if(const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData); vd)
			{
				const bool ignoreEvent = vd->IgnoreWindowPosEventFrame.has_value() && (ImGui::GetFrameCount() <= vd->IgnoreWindowPosEventFrame.value() + 1);
				// vd->IgnoreWindowPosEventFrame = -1;
				if(ignoreEvent)
					return;
			}

			viewport->PlatformRequestMove = true;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Window size callback.
	/// </summary>
	/// <param name="window">Affected window.</param>
	/// <param name="width">New width.</param>
	/// <param name="height">New height.</param>
	void WindowSizeCallback(const TRAP::INTERNAL::WindowingAPI::InternalWindow& window,
	                        [[maybe_unused]] const i32 width, [[maybe_unused]] const i32 height)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		//const_cast is safe because window only gets compared inside ImGui::FindViewportByPlatformHandle()
		ImGuiViewport* const viewport = ImGui::FindViewportByPlatformHandle
		(
			const_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(&window)
		);

		if(viewport != nullptr)
		{
			if(const ImGuiViewportDataTRAP* const data = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData); data)
			{
				const bool ignoreEvent = data->IgnoreWindowSizeEventFrame.has_value() && (ImGui::GetFrameCount() <= data->IgnoreWindowSizeEventFrame.value() + 1);
				// data->IgnoreWindowSizeEventFrame = -1;
				if (ignoreEvent)
					return;
			}
			viewport->PlatformRequestResize = true;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Create a new ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to create window for.</param>
	void CreateWindow(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::CreateWindow(): viewport is nullptr!");

		ImGuiViewportDataTRAP* const vd = IM_NEW(ImGuiViewportDataTRAP)();
		viewport->PlatformUserData = vd;

		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Visible, false);
		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Focused, false);
		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::FocusOnShow, false);
		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Decorated, (viewport->Flags & ImGuiViewportFlags_NoDecoration) == 0);
		TRAP::INTERNAL::WindowingAPI::WindowHint(TRAP::INTERNAL::WindowingAPI::Hint::Floating, (viewport->Flags & ImGuiViewportFlags_TopMost) != 0);
		vd->Window = TRAP::INTERNAL::WindowingAPI::CreateWindow(NumericCast<u32>(viewport->Size.x),
												                NumericCast<u32>(viewport->Size.y), "No Title Yet", nullptr);
		vd->WindowOwned = true;
		viewport->PlatformHandle = static_cast<void*>(vd->Window);
	#ifdef TRAP_PLATFORM_WINDOWS
		viewport->PlatformHandleRaw = TRAP::INTERNAL::WindowingAPI::GetWin32Window(*vd->Window);
	#endif /*TRAP_PLATFORM_WINDOWS*/
		TRAP::INTERNAL::WindowingAPI::SetWindowPos(*vd->Window, NumericCast<i32>(viewport->Pos.x),
								                   NumericCast<i32>(viewport->Pos.y));

		//Install callbacks for secondary viewports
		TRAP::INTERNAL::WindowingAPI::SetWindowFocusCallback(*vd->Window, WindowFocusCallback);
		TRAP::INTERNAL::WindowingAPI::SetCursorEnterCallback(*vd->Window, CursorEnterCallback);
		TRAP::INTERNAL::WindowingAPI::SetCursorPosCallback(*vd->Window, CursorPosCallback);
		TRAP::INTERNAL::WindowingAPI::SetMouseButtonCallback(*vd->Window, MouseButtonCallback);
		TRAP::INTERNAL::WindowingAPI::SetScrollCallback(*vd->Window, ScrollCallback);
		TRAP::INTERNAL::WindowingAPI::SetKeyCallback(*vd->Window, KeyCallback);
		TRAP::INTERNAL::WindowingAPI::SetCharCallback(*vd->Window, CharCallback);
		TRAP::INTERNAL::WindowingAPI::SetWindowCloseCallback(*vd->Window, WindowCloseCallback);
		TRAP::INTERNAL::WindowingAPI::SetWindowPosCallback(*vd->Window, WindowPosCallback);
		TRAP::INTERNAL::WindowingAPI::SetWindowSizeCallback(*vd->Window, WindowSizeCallback);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Destroy an ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to destroy window on.</param>
	void DestroyWindow(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::DestroyWindow(): viewport is nullptr!");

		if(ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData); vd)
		{
			if(vd->WindowOwned)
			{
				//Release any keys that were pressed in the window being destroyed and are still held down,
				//because we will not receive any release events after window is destroyed.
				const ImGuiTRAPData* const bd = GetBackendData();
				for(usize i = 0; i < bd->KeyOwnerWindows.size(); ++i)
				{
					if(bd->KeyOwnerWindows[i] == vd->Window)
						KeyCallback(*vd->Window, static_cast<TRAP::Input::Key>(i), TRAP::Input::KeyState::Released); //Later params are only used for main viewport, on which this function is never called.
				}

				TRAP::INTERNAL::WindowingAPI::DestroyWindow(vd->Window);
			}
			vd->Window = nullptr;
			IM_DELETE(vd);
		}
		viewport->PlatformUserData = nullptr;
		viewport->PlatformHandle = nullptr;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Show an ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to show window for.</param>
	void ShowWindow(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::ShowWindow(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::ShowWindow(): vd is nullptr!");

		if((viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) != 0)
			TRAP::INTERNAL::WindowingAPI::HideWindowFromTaskbar(*static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(viewport->PlatformHandle));

		TRAP::INTERNAL::WindowingAPI::ShowWindow(*vd->Window);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Retrieve the ImGui Window position.
	/// </summary>
	/// <param name="viewport">Viewport to retrieve position for.</param>
	/// <returns>Window position.</returns>
	[[nodiscard]] ImVec2 GetWindowPos(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::GetWindowPos(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::GetWindowPos(): vd is nullptr!");

		i32 x = 0, y = 0;
		TRAP::INTERNAL::WindowingAPI::GetWindowPos(*vd->Window, x, y);

		return ImVec2(NumericCast<f32>(x), NumericCast<f32>(y));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Set the position for the ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to set window position on.</param>
	/// <param name="pos">Position to set.</param>
	void SetWindowPos(ImGuiViewport* const viewport, ImVec2 pos)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::SetWindowPos(): viewport is nullptr!");

		ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::SetWindowPos(): vd is nullptr!");

		vd->IgnoreWindowPosEventFrame = ImGui::GetFrameCount();
		TRAP::INTERNAL::WindowingAPI::SetWindowPos(*vd->Window, NumericCast<i32>(pos.x), NumericCast<i32>(pos.y));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Retrieve the ImGui Window size.
	/// </summary>
	/// <param name="viewport">Viewport to retrieve size for.</param>
	/// <returns>Window size.</returns>
	[[nodiscard]] ImVec2 GetWindowSize(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::GetWindowSize(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::GetWindowSize(): vd is nullptr!");

		i32 width = 0, height = 0;
		TRAP::INTERNAL::WindowingAPI::GetWindowSize(*vd->Window, width, height);

		return ImVec2(NumericCast<f32>(width), NumericCast<f32>(height));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Set the size for the ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to set window size on.</param>
	/// <param name="size">Size to set.</param>
	void SetWindowSize(ImGuiViewport* const viewport, ImVec2 size)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::SetWindowSize(): viewport is nullptr!");

		ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::SetWindowSize(): vd is nullptr!");

		vd->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();

		TRAP::INTERNAL::WindowingAPI::SetWindowSize(*vd->Window, NumericCast<i32>(size.x), NumericCast<i32>(size.y));
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Set the title for the ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to set window title on.</param>
	/// <param name="title">Title to set.</param>
	void SetWindowTitle(ImGuiViewport* const viewport, const char* title)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::SetWindowTitle(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::SetWindowTitle(): vd is nullptr!");

		TRAP::INTERNAL::WindowingAPI::SetWindowTitle(*vd->Window, title);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Set focus for the ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to set window focus on.</param>
	void SetWindowFocus(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::SetWindowFocus(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::SetWindowFocus(): vd is nullptr!");

		TRAP::INTERNAL::WindowingAPI::FocusWindow(*vd->Window);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Retrieve the ImGui Window focus state.
	/// </summary>
	/// <param name="viewport">Viewport to retrieve focus state for.</param>
	/// <returns>Focus state.</returns>
	[[nodiscard]] bool GetWindowFocus(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::GetWindowFocus(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::GetWindowFocus(): vd is nullptr!");

		return TRAP::INTERNAL::WindowingAPI::GetWindowHint(*vd->Window, TRAP::INTERNAL::WindowingAPI::Hint::Focused);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Retrieve the ImGui Window minimization state.
	/// </summary>
	/// <param name="viewport">Viewport to retrieve minimization state for.</param>
	/// <returns>Minimization state.</returns>
	[[nodiscard]] bool GetWindowMinimized(ImGuiViewport* const viewport)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::GetWindowMinimized(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::GetWindowMinimized(): vd is nullptr!");

		return TRAP::INTERNAL::WindowingAPI::GetWindowHint(*vd->Window, TRAP::INTERNAL::WindowingAPI::Hint::Minimized);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Set the alpha value for the ImGui Window.
	/// </summary>
	/// <param name="viewport">Viewport to set alpha value on.</param>
	/// <param name="alpha">Alpha value.</param>
	void SetWindowAlpha(ImGuiViewport* const viewport, f32 alpha)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::SetWindowAlpha(): viewport is nullptr!");

		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::SetWindowAlpha(): vd is nullptr!");

		TRAP::INTERNAL::WindowingAPI::SetWindowOpacity(*vd->Window, alpha);
	}

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
	[[nodiscard]] i32 CreateVkSurface(ImGuiViewport* const viewport, const ImU64 vkInstance,
	                                      const void* const vkAllocator, ImU64* const outVkSurface)
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		TRAP_ASSERT(viewport != nullptr, "ImGuiWindowing::CreateVkSurface(): viewport is nullptr!");

		[[maybe_unused]] const ImGuiTRAPData* const bd = GetBackendData();
		const ImGuiViewportDataTRAP* const vd = static_cast<ImGuiViewportDataTRAP*>(viewport->PlatformUserData);
		TRAP_ASSERT(vd != nullptr, "ImGuiWindowing::CreateVkSurface(): vd is nullptr!");
		TRAP_ASSERT(bd->ClientAPI == TRAP::Graphics::RenderAPI::Vulkan, "ImGuiWindowing::CreateVkSurface(): RenderAPI is not Vulkan!");
		const VkResult err = TRAP::INTERNAL::WindowingAPI::CreateWindowSurface(std::bit_cast<const VkInstance>(vkInstance),
															                   *vd->Window,
															                   static_cast<const VkAllocationCallbacks*>(vkAllocator),
															                   *reinterpret_cast<VkSurfaceKHR*>(outVkSurface));

		return static_cast<i32>(err);
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Initialize the platform interface.
	/// </summary>
	void InitPlatformInterface()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		//Register platform interface (will be coupled with a renderer interface)
		const ImGuiTRAPData* const bd = GetBackendData();
		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		platformIO.Platform_CreateWindow = CreateWindow;
		platformIO.Platform_DestroyWindow = DestroyWindow;
		platformIO.Platform_ShowWindow = ShowWindow;
		platformIO.Platform_SetWindowPos = SetWindowPos;
		platformIO.Platform_GetWindowPos = GetWindowPos;
		platformIO.Platform_SetWindowSize = SetWindowSize;
		platformIO.Platform_GetWindowSize = GetWindowSize;
		platformIO.Platform_SetWindowFocus = SetWindowFocus;
		platformIO.Platform_GetWindowFocus = GetWindowFocus;
		platformIO.Platform_GetWindowMinimized = GetWindowMinimized;
		platformIO.Platform_SetWindowTitle = SetWindowTitle;
		platformIO.Platform_RenderWindow = nullptr; //Unused because we don't use OpenGL.
		platformIO.Platform_SwapBuffers = nullptr; //Unused because we don't use OpenGL.
		platformIO.Platform_SetWindowAlpha = SetWindowAlpha;
		platformIO.Platform_CreateVkSurface = CreateVkSurface;

		//Register main window handle (which is owned by the main application, not by us)
		//This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use
		//same logic for main and secondary viewports.
		ImGuiViewport* const main_viewport = ImGui::GetMainViewport();
		TRAP_ASSERT(main_viewport != nullptr, "ImGuiWindowing::InitPlatformInterface(): main_viewport is nullptr!");
		ImGuiViewportDataTRAP* const vd = IM_NEW(ImGuiViewportDataTRAP)();
		vd->Window = bd->Window;
		vd->WindowOwned = false;
		main_viewport->PlatformUserData = vd;
		main_viewport->PlatformHandle = bd->Window;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Shutdown the platform interface.
	/// </summary>
	void ShutdownPlatformInterface()
	{
		ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

		ImGui::DestroyPlatformWindows();
	}
}

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::INTERNAL::ImGuiWindowing::Init(WindowingAPI::InternalWindow* const window, const bool installCallbacks,
                                                        const TRAP::Graphics::RenderAPI renderAPI)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	if(renderAPI == Graphics::RenderAPI::NONE)
		return true; //Do nothing when not using any RenderAPI

	ImGuiIO& io = ImGui::GetIO();
	TRAP_ASSERT(io.BackendPlatformUserData == nullptr, "ImGuiWindowing::Init(): Platform backend is already initialized!");

	//Setup back-end capabilities flags
	ImGuiTRAPData* const bd = IM_NEW(ImGuiTRAPData)();
	io.BackendPlatformUserData = bd;
	io.BackendPlatformName = "TRAP";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; //We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; //We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; //We can create multi-viewports on the Platform side (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; //We can set io.MouseHoveredViewport correctly (optional, not easy)

	bd->Window = window;
	bd->Time = 0.0;
	bd->WantUpdateMonitors = true;

	io.SetClipboardTextFn = SetClipboardText;
	io.GetClipboardTextFn = GetClipboardText;
	io.ClipboardUserData = bd->Window;

	//Create mouse cursors
	std::get<ImGuiMouseCursor_Arrow>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Arrow);
	std::get<ImGuiMouseCursor_TextInput>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor(WindowingAPI::CursorType::Input);
	std::get<ImGuiMouseCursor_ResizeNS>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeVertical
	);
	std::get<ImGuiMouseCursor_ResizeEW>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeHorizontal
	);
	std::get<ImGuiMouseCursor_Hand>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::PointingHand
	);
	std::get<ImGuiMouseCursor_ResizeAll>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeAll
	);
	std::get<ImGuiMouseCursor_ResizeNESW>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeDiagonalTopLeftBottomRight
	);
	std::get<ImGuiMouseCursor_ResizeNWSE>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::ResizeDiagonalTopRightBottomLeft
	);
	std::get<ImGuiMouseCursor_NotAllowed>(bd->MouseCursors) = WindowingAPI::CreateStandardCursor
	(
		WindowingAPI::CursorType::NotAllowed
	);

	//Chain WindowingAPI callback: our callback will call the user's previously installed callbacks, if any.
	if(installCallbacks)
		InstallCallbacks(window);

	UpdateMonitors();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	//Set platform dependent data in viewport
	ImGuiViewport* const mainViewport = ImGui::GetMainViewport();
	mainViewport->PlatformHandle = bd->Window;
#ifdef TRAP_PLATFORM_WINDOWS
	mainViewport->PlatformHandleRaw = WindowingAPI::GetWin32Window(*bd->Window);
#endif /*TRAP_PLATFORM_WINDOWS*/
	if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
		InitPlatformInterface();

	bd->ClientAPI = renderAPI;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	TRAP_ASSERT(bd != nullptr, "ImGuiWindowing::Shutdown(): No platform backend to shutdown, or already shutdown?!");
	ImGuiIO& io = ImGui::GetIO();

	ShutdownPlatformInterface();

	if (bd->InstalledCallbacks)
		RestoreCallbacks(bd->Window);

	for (ImGuiMouseCursor cursorN = 0; cursorN < ImGuiMouseCursor_COUNT; cursorN++)
	{
		WindowingAPI::DestroyCursor(bd->MouseCursors[NumericCast<usize>(cursorN)]);
		bd->MouseCursors[NumericCast<usize>(cursorN)] = nullptr;
	}

	io.BackendPlatformName = nullptr;
	io.BackendPlatformUserData = nullptr;
	IM_DELETE(bd);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::NewFrame()
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiTRAPData* const bd = GetBackendData();
	TRAP_ASSERT(bd != nullptr, "ImGuiWindowing::NewFrame(): Did you call ImGuiWindowing::Init()?");

	//Setup display size (every frame to accommodate for window resizing)
	i32 width = 0, height = 0;
	i32 displayWidth = 0, displayHeight = 0;
	WindowingAPI::GetWindowSize(*bd->Window, width, height);
	WindowingAPI::GetFrameBufferSize(*bd->Window, displayWidth, displayHeight);
	io.DisplaySize = ImVec2(NumericCast<f32>(width), NumericCast<f32>(height));
	if (width > 0 && height > 0)
		io.DisplayFramebufferScale = ImVec2(NumericCast<f32>(displayWidth) / io.DisplaySize.x,
		                                    NumericCast<f32>(displayHeight) / io.DisplaySize.y);
	if (bd->WantUpdateMonitors)
		UpdateMonitors();

	//Setup time step
	f64 currentTime = static_cast<f64>(Application::GetTime());
	if(currentTime <= bd->Time)
		currentTime = bd->Time + 0.00001f;
	io.DeltaTime = bd->Time > 0.0 ? NumericCast<f32>(currentTime - bd->Time) : (1.0f / 60.0f);
	bd->Time = currentTime;

	UpdateMouseData();
	UpdateMouseCursor();

	//Update game controller (if enabled and available)
	UpdateGamepads();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetCustomCursor(WindowingAPI::InternalCursor* const cursor)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	bd->CustomCursor = cursor;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::SetCallbacksChainForAllWindows(const bool chainForAllWindows)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	bd->CallbacksChainForAllWindows = chainForAllWindows;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::InstallCallbacks(WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	TRAP_ASSERT(bd->InstalledCallbacks == false, "ImGuiWindowing::InstallCallbacks(): Callbacks are already initialized!");
	TRAP_ASSERT(bd->Window == window, "ImGuiWindowing::InstallCallbacks(): window and bd->Window aren't the same!");

	bd->PrevUserCallbackWindowFocus = WindowingAPI::GetWindowFocusCallback(*window);
	WindowingAPI::SetWindowFocusCallback(*window, WindowFocusCallback);

	bd->PrevUserCallbackCursorEnter = WindowingAPI::GetCursorEnterCallback(*window);
	WindowingAPI::SetCursorEnterCallback(*window, CursorEnterCallback);

	bd->PrevUserCallbackCursorPos = WindowingAPI::GetCursorPosCallback(*window);
	WindowingAPI::SetCursorPosCallback(*window, CursorPosCallback);

	bd->PrevUserCallbackMouseButton = WindowingAPI::GetMouseButtonCallback(*window);
	WindowingAPI::SetMouseButtonCallback(*window, MouseButtonCallback);

	bd->PrevUserCallbackScroll = WindowingAPI::GetScrollCallback(*window);
	WindowingAPI::SetScrollCallback(*window, ScrollCallback);

	bd->PrevUserCallbackKey = WindowingAPI::GetKeyCallback(*window);
	WindowingAPI::SetKeyCallback(*window, KeyCallback);

	bd->PrevUserCallbackChar = WindowingAPI::GetCharCallback(*window);
	WindowingAPI::SetCharCallback(*window, CharCallback);

	bd->PrevUserCallbackMonitor = WindowingAPI::GetMonitorCallback();
	WindowingAPI::SetMonitorCallback(MonitorCallback);

	bd->InstalledCallbacks = true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::INTERNAL::ImGuiWindowing::RestoreCallbacks(WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	TRAP_ASSERT(bd->InstalledCallbacks == true, "ImGuiWindowing::RestoreCallbacks(): Callbacks are not installed!");
	TRAP_ASSERT(bd->Window == window, "ImGuiWindowing::RestoreCallbacks(): window and bd->Window are not the same!");

	WindowingAPI::SetWindowFocusCallback(*window, bd->PrevUserCallbackWindowFocus);
	WindowingAPI::SetCursorEnterCallback(*window, bd->PrevUserCallbackCursorEnter);
	WindowingAPI::SetCursorPosCallback(*window, bd->PrevUserCallbackCursorPos);
	WindowingAPI::SetMouseButtonCallback(*window, bd->PrevUserCallbackMouseButton);
	WindowingAPI::SetScrollCallback(*window, bd->PrevUserCallbackScroll);
	WindowingAPI::SetKeyCallback(*window, bd->PrevUserCallbackKey);
	WindowingAPI::SetCharCallback(*window, bd->PrevUserCallbackChar);
	WindowingAPI::SetMonitorCallback(bd->PrevUserCallbackMonitor);
	bd->InstalledCallbacks = false;
	bd->PrevUserCallbackWindowFocus = nullptr;
	bd->PrevUserCallbackCursorEnter = nullptr;
	bd->PrevUserCallbackCursorPos = nullptr;
	bd->PrevUserCallbackMouseButton = nullptr;
	bd->PrevUserCallbackScroll = nullptr;
	bd->PrevUserCallbackKey = nullptr;
	bd->PrevUserCallbackChar = nullptr;
	bd->PrevUserCallbackMonitor = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::INTERNAL::ImGuiWindowing::ShouldChainCallback(const WindowingAPI::InternalWindow* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Brown, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Layers);

	ImGuiTRAPData* const bd = GetBackendData();
	return bd->CallbacksChainForAllWindows ? true : (window == bd->Window);
}

#endif /*TRAP_HEADLESS_MODE*/
/*
Copyright(c) 2002 - 2006 Marcus Geelnard

Copyright(c) 2006 - 2019 Camilla Loewy

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter itand redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would
be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.

Modified by : Jan "GamesTrap" Schuerkamp
The above license only applies to some of the Controller specific parts of this file!
*/

#include "TRAPPCH.h"
#include "Input.h"

#ifndef TRAP_HEADLESS_MODE

#ifdef TRAP_PLATFORM_WINDOWS
#define TRAP_BUILD_WIN32_MAPPINGS
#elif defined(TRAP_PLATFORM_LINUX)
#define TRAP_BUILD_LINUX_MAPPINGS
#endif

#include "Application.h"
#include "ControllerMappings.h"
#include "Utils/String/String.h"
#include "Window/WindowingAPI.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::EventCallbackFn TRAP::Input::s_eventCallback{};
std::array<TRAP::Input::ControllerInternal, 16> TRAP::Input::s_controllerInternal{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	TP_DEBUG(Log::InputPrefix, "Initializing");

	InitControllerMappings();

	if(!InitController())
		TP_ERROR(Log::InputControllerPrefix, "Failed to initialize controller support!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	TP_DEBUG(Log::InputPrefix, "Shutting down input");

	ShutdownController();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsKeyPressed(const Key key)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (key == Key::Unknown)
	{
		TP_WARN(Log::InputPrefix, "Invalid key provided!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                  (Application::GetWindow()->GetInternalWindow()), key);

	return state != TRAP::Input::KeyState::Released;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsKeyPressed(const Key key, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (key == Key::Unknown)
	{
		TP_WARN(Log::InputPrefix, "Invalid key provided!");
		return false;
	}
	if(window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to IsKeyPressed!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                  (window->GetInternalWindow()), key);

	return state != TRAP::Input::KeyState::Released;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	const auto state = INTERNAL::WindowingAPI::GetMouseButton(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                          (Application::GetWindow()->GetInternalWindow()),
															  button);

	return state != TRAP::Input::KeyState::Released;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsMouseButtonPressed(const MouseButton button, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to IsMouseButtonPressed!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetMouseButton(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                          (window->GetInternalWindow()), button);

	return state != TRAP::Input::KeyState::Released;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsRawMouseInputSupported()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	return INTERNAL::WindowingAPI::RawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsControllerGamepad(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return false;

	if (!PollController(controller, PollMode::Presence))
		return false;

	return s_controllerInternal[std::to_underlying(controller)].mapping != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	f64 xPos = 0.0, yPos = 0.0;
	INTERNAL::WindowingAPI::GetCursorPos(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), xPos, yPos);

#ifdef TRAP_PLATFORM_WINDOWS
	const TRAP::Math::Vec2i windowPos = Application::GetWindow()->GetPosition();
	xPos += windowPos.x();
	yPos += windowPos.y();
#endif /*TRAP_PLATFORM_WINDOWS*/

	return {NumericCast<f32>(xPos), NumericCast<f32>(yPos)};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Input::GetMousePosition(const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if(window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to GetMousePosition!");
		return TRAP::Math::Vec2{};
	}

	f64 xPos = 0.0, yPos = 0.0;
	INTERNAL::WindowingAPI::GetCursorPos(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), xPos, yPos);

#ifdef TRAP_PLATFORM_WINDOWS
	const TRAP::Math::Vec2i windowPos = window->GetPosition();
	xPos += windowPos.x();
	yPos += windowPos.y();
#endif /*TRAP_PLATFORM_WINDOWS*/

	return TRAP::Math::Vec2{ NumericCast<f32>(xPos), NumericCast<f32>(yPos) };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Input::GetMouseX()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return GetMousePosition().x();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Input::GetMouseY()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return GetMousePosition().y();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Input::GetMouseX(const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return GetMousePosition(window).x();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Input::GetMouseY(const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return GetMousePosition(window).y();
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	/// @brief Get a string representation for a non printable key (space key for example).
	/// @param key Key to get.
	/// @return String representation.
	[[nodiscard]] constexpr std::string NonPrintableKeyToString(const TRAP::Input::Key key) noexcept
	{
		switch(key)
		{
		case TRAP::Input::Key::Unknown:
			return "Unknown";

		case TRAP::Input::Key::Space:
			return "Space";

		case TRAP::Input::Key::Grave_Accent:
			return "Grave Accent";

		case TRAP::Input::Key::Escape:
			return "Escape";

		case TRAP::Input::Key::Enter:
			return "Enter";

		case TRAP::Input::Key::Tab:
			return "Tab";

		case TRAP::Input::Key::Backspace:
			return "Backspace";

		case TRAP::Input::Key::Insert:
			return "Insert";

		case TRAP::Input::Key::Delete:
			return "Delete";

		case TRAP::Input::Key::Right:
			return "Right";

		case TRAP::Input::Key::Left:
			return "Left";

		case TRAP::Input::Key::Down:
			return "Down";

		case TRAP::Input::Key::Up:
			return "Up";

		case TRAP::Input::Key::Page_Up:
			return "Page Up";

		case TRAP::Input::Key::Page_Down:
			return "Page Down";

		case TRAP::Input::Key::Home:
			return "Home";

		case TRAP::Input::Key::End:
			return "End";

		case TRAP::Input::Key::Caps_Lock:
			return "Caps Lock";

		case TRAP::Input::Key::Scroll_Lock:
			return "Scroll Lock";

		case TRAP::Input::Key::Num_Lock:
			return "Num Lock";

		case TRAP::Input::Key::Print_Screen:
			return "Print Screen";

		case TRAP::Input::Key::Pause:
			return "Pause";

		case TRAP::Input::Key::F1:
			return "F1";

		case TRAP::Input::Key::F2:
			return "F2";

		case TRAP::Input::Key::F3:
			return "F3";

		case TRAP::Input::Key::F4:
			return "F4";

		case TRAP::Input::Key::F5:
			return "F5";

		case TRAP::Input::Key::F6:
			return "F6";

		case TRAP::Input::Key::F7:
			return "F7";

		case TRAP::Input::Key::F8:
			return "F8";

		case TRAP::Input::Key::F9:
			return "F9";

		case TRAP::Input::Key::F10:
			return "F10";

		case TRAP::Input::Key::F11:
			return "F11";

		case TRAP::Input::Key::F12:
			return "F12";

		case TRAP::Input::Key::F13:
			return "F13";

		case TRAP::Input::Key::F14:
			return "F14";

		case TRAP::Input::Key::F15:
			return "F15";

		case TRAP::Input::Key::F16:
			return "F16";

		case TRAP::Input::Key::F17:
			return "F17";

		case TRAP::Input::Key::F18:
			return "F18";

		case TRAP::Input::Key::F19:
			return "F19";

		case TRAP::Input::Key::F20:
			return "F20";

		case TRAP::Input::Key::F21:
			return "F21";

		case TRAP::Input::Key::F22:
			return "F22";

		case TRAP::Input::Key::F23:
			return "F23";

		case TRAP::Input::Key::F24:
			return "F24";

		case TRAP::Input::Key::F25:
			return "F25";

		case TRAP::Input::Key::KP_Enter:
			return "Numpad Enter";

		case TRAP::Input::Key::Left_Shift:
			return "Left Shift";

		case TRAP::Input::Key::Left_Control:
			return "Left Control";

		case TRAP::Input::Key::Left_ALT:
			return "Left ALT";

		case TRAP::Input::Key::Left_Super:
			return "Left Windows/Super";

		case TRAP::Input::Key::Right_Shift:
			return "Right Shift";

		case TRAP::Input::Key::Right_Control:
			return "Right Control";

		case TRAP::Input::Key::Right_ALT:
			return "Right ALT";

		case TRAP::Input::Key::Right_Super:
			return "Right Windows/Super";

		case TRAP::Input::Key::Menu:
			return "Menu";

		default:
			return "";
		}
	}
}

[[nodiscard]] std::string TRAP::Input::GetKeyName(const Key key)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	return INTERNAL::WindowingAPI::GetKeyName(key, 0).value_or(NonPrintableKeyToString(key));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Input::GetControllerAxis(const Controller controller, const ControllerAxis axis)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return 0.0f;

	return GetMappedControllerAxis(controller, axis);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPad(const Controller controller, const u32 dpad)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return ControllerDPad::Centered;

	return GetMappedControllerDPad(controller, dpad);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsControllerButtonPressed(const Controller controller, const ControllerButton button)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return false;

	return GetMappedControllerButton(controller, button);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Input::GetControllerName(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return "";

	if (!PollController(controller, PollMode::Presence))
		return "";

	if(s_controllerInternal[std::to_underlying(controller)].mapping == nullptr)
		return s_controllerInternal[std::to_underlying(controller)].Name;

	return s_controllerInternal[std::to_underlying(controller)].mapping->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Input::GetControllerGUID(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return "";

	if (!PollController(controller, PollMode::Presence))
		return "";

	return s_controllerInternal[std::to_underlying(controller)].guid;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<f32> TRAP::Input::GetAllControllerAxes(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Axes))
		return {};

	return s_controllerInternal[std::to_underlying(controller)].Axes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<bool> TRAP::Input::GetAllControllerButtons(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Buttons))
		return {};

	return s_controllerInternal[std::to_underlying(controller)].Buttons;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPads(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Buttons))
		return {};

	return s_controllerInternal[std::to_underlying(controller)].DPads;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(const Controller controller, const f32 leftMotor, const f32 rightMotor)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return;

	if (!PollController(controller, PollMode::Presence))
		return;

	if (leftMotor != s_controllerInternal[std::to_underlying(controller)].LeftMotor ||
		rightMotor != s_controllerInternal[std::to_underlying(controller)].RightMotor)
	{
		s_controllerInternal[std::to_underlying(controller)].LeftMotor = leftMotor;
		s_controllerInternal[std::to_underlying(controller)].RightMotor = rightMotor;
		SetControllerVibrationInternal(controller, leftMotor, rightMotor);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(const Controller controller, const Math::Vec2& intensity)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!s_controllerInternal[std::to_underlying(controller)].Connected)
		return;

	if (!PollController(controller, PollMode::Presence))
		return;

	if (intensity.x() != s_controllerInternal[std::to_underlying(controller)].LeftMotor ||
		intensity.y() != s_controllerInternal[std::to_underlying(controller)].RightMotor)
	{
		s_controllerInternal[std::to_underlying(controller)].LeftMotor = intensity.x();
		s_controllerInternal[std::to_underlying(controller)].RightMotor = intensity.y();
		SetControllerVibrationInternal(controller, intensity.x(), intensity.y());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::ControllerBatteryStatus TRAP::Input::GetControllerBatteryStatus(Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if(!s_controllerInternal[std::to_underlying(controller)].Connected)
		return ControllerBatteryStatus::Wired;

	if(!PollController(controller, PollMode::Presence))
		return ControllerBatteryStatus::Wired;

	return GetControllerBatteryStatusInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const f32 x, const f32 y)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const f32 x, const f32 y, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to SetMousePosition!");
		return;
	}

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const Math::Vec2& position)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), position.x(), position.y());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const Math::Vec2& position, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to SetMousePosition!");
		return;
	}

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), position.x(), position.y());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetClipboard(const std::string& str)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	INTERNAL::WindowingAPI::SetClipboardString(str);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Input::GetClipboard()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetEventCallback(const EventCallbackFn &callback) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	s_eventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerMappings(const std::string_view map)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	Mapping mapping{};

	if(ParseMapping(mapping, map))
	{
		Mapping* const previous = FindMapping(mapping.guid);
		if (previous != nullptr)
			*previous = mapping;
		else
			s_mappings.push_back(mapping);
	}

	for(u32 cID = 0; cID <= std::to_underlying(Controller::Sixteen); cID++)
	{
		ControllerInternal* const con = &s_controllerInternal[cID];
		if(s_controllerInternal[cID].Connected)
			con->mapping = FindValidMapping(con);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::EventCallbackFn TRAP::Input::GetEventCallback() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return s_eventCallback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerInternal* TRAP::Input::AddInternalController(std::string name,
                                                                    std::string guid,
																	const u32 axisCount,
																	const u32 buttonCount,
																	const u32 dpadCount)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	u32 cID = 0;
	for(cID = 0; cID <= std::to_underlying(Controller::Sixteen); cID++)
	{
		if (!s_controllerInternal[cID].Connected)
			break;
	}

	if (cID > std::to_underlying(Controller::Sixteen))
		return nullptr;

	ControllerInternal* const con = &s_controllerInternal[cID];
	con->Connected = true;
	con->Name = std::move(name);
	con->guid = std::move(guid);
	con->Axes.resize(axisCount);
	con->Buttons.resize(buttonCount + dpadCount * 4);
	con->DPads.resize(dpadCount);
	con->ButtonCount = NumericCast<i32>(buttonCount);
	con->mapping = FindValidMapping(con);

	TP_INFO(Log::InputControllerPrefix, "Controller: ", (con->mapping != nullptr ? con->mapping->Name : con->Name),
	        " (", cID, ") connected!");

	return con;
}

//-------------------------------------------------------------------------------------------------------------------//

//Parse an SDL_GameControllerDB line and adds it to the mapping list
bool TRAP::Input::ParseMapping(Mapping& mapping, const std::string_view str)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	struct Fields
	{
		std::string Name;
		MapElement* Element = nullptr;
	};
	const std::array<Fields, 22> fields =
	{
		{
			{"platform", nullptr},
			{"a", &mapping.Buttons[std::to_underlying(ControllerButton::A)]},
			{"b", &mapping.Buttons[std::to_underlying(ControllerButton::B)]},
			{"x", &mapping.Buttons[std::to_underlying(ControllerButton::X)]},
			{"y", &mapping.Buttons[std::to_underlying(ControllerButton::Y)]},
			{"back", &mapping.Buttons[std::to_underlying(ControllerButton::Back)]},
			{"start", &mapping.Buttons[std::to_underlying(ControllerButton::Start)]},
			{"guide", &mapping.Buttons[std::to_underlying(ControllerButton::Guide)]},
			{"leftshoulder", &mapping.Buttons[std::to_underlying(ControllerButton::Left_Bumper)]},
			{"rightshoulder", &mapping.Buttons[std::to_underlying(ControllerButton::Right_Bumper)]},
			{"leftstick", &mapping.Buttons[std::to_underlying(ControllerButton::Left_Thumb)]},
			{"rightstick", &mapping.Buttons[std::to_underlying(ControllerButton::Right_Thumb)]},
			{"dpup", &mapping.Buttons[std::to_underlying(ControllerButton::DPad_Up)]},
			{"dpright", &mapping.Buttons[std::to_underlying(ControllerButton::DPad_Right)]},
			{"dpdown", &mapping.Buttons[std::to_underlying(ControllerButton::DPad_Down)]},
			{"dpleft", &mapping.Buttons[std::to_underlying(ControllerButton::DPad_Left)]},
			{"lefttrigger", &mapping.Axes[std::to_underlying(ControllerAxis::Left_Trigger)]},
			{"righttrigger", &mapping.Axes[std::to_underlying(ControllerAxis::Right_Trigger)]},
			{"leftx", &mapping.Axes[std::to_underlying(ControllerAxis::Left_X)]},
			{"lefty", &mapping.Axes[std::to_underlying(ControllerAxis::Left_Y)]},
			{"rightx", &mapping.Axes[std::to_underlying(ControllerAxis::Right_X)]},
			{"righty", &mapping.Axes[std::to_underlying(ControllerAxis::Right_Y)]}
		}
	};

	const std::vector<std::string_view> splittedString = Utils::String::SplitStringView(str, ',');

	if(splittedString.empty())
	{
		TP_ERROR(Log::InputControllerPrefix, "Map is empty!");
		return false;
	}
	// if (splittedString.size() > 25)
	// {
	// 	TP_ERROR(Log::InputControllerPrefix, "Map has too many elements! There must be less than 24 elements!");
	// 	return false;
	// }

	if(splittedString[0].size() != 32)
	{
		TP_ERROR(Log::InputControllerPrefix, "Invalid GUID size ", splittedString[0].size(),
		         "! Must be 32 bytes long");
		return false;
	}

	if (splittedString[0].empty())
	{
		TP_ERROR(Log::InputControllerPrefix, "Mapping GUID can't be empty!");
		return false;
	}
	mapping.guid = Utils::String::ToLower(std::string(splittedString[0]));

	if(splittedString[1].empty())
	{
		TP_ERROR(Log::InputControllerPrefix, "Mapping name can't be empty!");
		return false;
	}
	mapping.Name = splittedString[1];

	for (u8 i = 2; i < splittedString.size();) //Start after Mapping Name
	{
		const std::vector<std::string_view> splittedField = Utils::String::SplitStringView(splittedString[i], ':');
		if (splittedField.empty())
		{
			TP_ERROR(Log::InputControllerPrefix, "Field can't be empty! Mapping: ", splittedString[1]);
			return false;
		}
		if (splittedField.size() < 2)
		{
			TP_ERROR(Log::InputControllerPrefix, "Too few elements inside field: ", NumericCast<u32>(i),
			         "! Mapping: ", splittedString[1]);
			return false;
		}
		if (splittedField.size() > 2)
		{
			TP_ERROR(Log::InputControllerPrefix, "Too many elements inside field: ", NumericCast<u32>(i),
			         "! Mapping: ", splittedString[1]);
			return false;
		}

		for (const auto& c : splittedField[0])
		{
			if(c == '+' || c == '-') //TODO
			{
				TP_WARN(Log::InputControllerPrefix, "Controller mapping output modifiers are WIP! Mapping: ",
				        splittedString[1]);
				return false;
			}
			if (!Utils::String::IsAlphaNumeric(c))
			{
				TP_ERROR(Log::InputControllerPrefix, "Invalid char inside field: ", NumericCast<u32>(i),
				         "! Mapping: ", splittedString[1]);
				return false;
			}
		}

		bool found = false;
		u8 j = 0;
		for (j = 0; std::cmp_less(j, fields.size()); j++)
		{
			if (fields[j].Name == splittedField[0])
			{
				found = true;
				break;
			}
		}

		if(found && (fields[j].Element != nullptr))
		{
			MapElement* const e = fields[j].Element;
			i8 minimum = -1;
			i8 maximum = 1;
			u32 charOffset = 0;

			//Input modifiers
			if (splittedField[1][0] == '+')
			{
				minimum = 0;
				charOffset++;
			}
			else if (splittedField[1][0] == '-')
			{
				maximum = 0;
				charOffset++;
			}

			if (splittedField[1][charOffset] == 'a')
				e->Type = 1; //Axis
			else if (splittedField[1][charOffset] == 'b')
				e->Type = 2; //Button
			else if (splittedField[1][charOffset] == 'h')
				e->Type = 3; //DPad
			else
				break;

			charOffset++;

			if (e->Type == 3) //DPad
			{
				const u64 hat = std::stoul(splittedField[1].data() + charOffset);
				const u64 bit = std::stoul(splittedField[1].data() + charOffset);
				e->Index = NumericCast<u8>((hat << 4u) | bit);
			}
			else
				e->Index = NumericCast<u8>(std::stoul(splittedField[1].data() + charOffset));

			if (e->Type == 1) //Axis
			{
				e->AxisScale = NumericCast<i8>(2 / (maximum - minimum));
				e->AxisOffset = NumericCast<i8>(-(maximum + minimum));

				//Invert axis input modifier
				if (splittedField[1][charOffset] == '~')
				{
					e->AxisScale = NumericCast<i8>(-e->AxisScale);
					e->AxisOffset = NumericCast<i8>(-e->AxisOffset);
				}
			}
		}

		i++;
	}

	UpdateControllerGUID(mapping.guid);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::InitControllerMappings()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	s_mappings.resize(Embed::ControllerMappings.size());

	for(usize i = 0; i < Embed::ControllerMappings.size(); ++i)
		ParseMapping(s_mappings[i], Embed::ControllerMappings[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::GetMappedControllerButton(Controller controller, ControllerButton button)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (!PollController(controller, PollMode::Buttons))
		return false;

	const ControllerInternal* const con = &s_controllerInternal[std::to_underlying(controller)];

	if (con->mapping == nullptr)
		return false;

	const MapElement* const e = &con->mapping->Buttons[std::to_underlying(button)];
	if (e->Index < con->ButtonCount)
	{
		if (e->Type == 2) //Button
			return con->Buttons[e->Index];
	}
	if (e->Type == 1) //Axis
	{
		const f32 value = con->Axes[e->Index] * NumericCast<f32>(e->AxisScale) +
		                    NumericCast<f32>(e->AxisOffset);
		if (e->AxisOffset < 0 || (e->AxisOffset == 0 && e->AxisScale > 0))
		{
			if (value >= 0.0f)
				return true;
		}
		else
		{
			if (value <= 0.0f)
				return false;
		}
	}
	else if (e->Type == 3) //DPad
	{
		if (con->Buttons[e->Index >> 4u])
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] f32 TRAP::Input::GetMappedControllerAxis(const Controller controller, const ControllerAxis axis)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if(!PollController(controller, PollMode::Axes))
		return 0.0f;

	const ControllerInternal* const con = &s_controllerInternal[std::to_underlying(controller)];

	if(con->mapping == nullptr)
		return 0.0f;

	const MapElement* const e = &con->mapping->Axes[std::to_underlying(axis)];
	if(e->Type == 1) //Axis
	{
		const f32 value = con->Axes[e->Index] * NumericCast<f32>(e->AxisScale) +
		                    NumericCast<f32>(e->AxisOffset);
		return Math::Clamp(value, -1.0f, 1.0f);
	}
	if(e->Type == 2) //Button
		return con->Buttons[e->Index] ? 1.0f : -1.0f;
	if(e->Type == 3) //DPad
	{
		if(con->Buttons[e->Index >>  4u])
			return 1.0f;

		return -1.0f;
	}

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::ControllerDPad TRAP::Input::GetMappedControllerDPad(const Controller controller, const u32 dpad)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if(!PollController(controller, PollMode::All))
		return ControllerDPad::Centered;

	const ControllerInternal* const con = &s_controllerInternal[std::to_underlying(controller)];

	if (con->mapping == nullptr)
		return ControllerDPad::Centered;

	const MapElement* const e = &con->mapping->Buttons[11 + (dpad * 4)];
	if(e->Type == 3)
		return con->DPads[e->Index >> 4u];

	return ControllerDPad::Centered;
}

#endif /*TRAP_HEADLESS_MODE*/
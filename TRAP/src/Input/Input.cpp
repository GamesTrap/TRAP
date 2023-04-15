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
std::vector<TRAP::Input::Mapping> TRAP::Input::s_mappings{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	TP_DEBUG(Log::InputPrefix, "Initializing");

	InitControllerMappings();

	if(!InitController())
		TP_ERROR(Log::InputControllerPrefix, "Failed to initialize controller support!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	TP_DEBUG(Log::InputPrefix, "Shutting down input");

	ShutdownController();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsKeyPressed(const Key key)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (key == Key::Unknown)
	{
		TP_WARN(Log::InputPrefix, "Invalid key provided!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                  (Application::GetWindow()->GetInternalWindow()), key);

	return static_cast<bool>(state);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsKeyPressed(const Key key, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

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

	return static_cast<bool>(state);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	const auto state = INTERNAL::WindowingAPI::GetMouseButton(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                          (Application::GetWindow()->GetInternalWindow()),
															  button);

	return static_cast<bool>(state);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsMouseButtonPressed(const MouseButton button, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to IsMouseButtonPressed!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetMouseButton(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                          (window->GetInternalWindow()), button);

	return static_cast<bool>(state);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsRawMouseInputSupported()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	return INTERNAL::WindowingAPI::RawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsControllerConnected(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	return s_controllerInternal[static_cast<uint32_t>(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsControllerGamepad(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return false;

	if (!PollController(controller, PollMode::Presence))
		return false;

	return s_controllerInternal[static_cast<uint32_t>(controller)].mapping != nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	double xPos = 0.0, yPos = 0.0;
	INTERNAL::WindowingAPI::GetCursorPos(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), xPos, yPos);

#ifdef TRAP_PLATFORM_WINDOWS
	const TRAP::Math::Vec2i windowPos = Application::GetWindow()->GetPosition();
	xPos += windowPos.x;
	yPos += windowPos.y;
#endif /*TRAP_PLATFORM_WINDOWS*/

	return {static_cast<float>(xPos), static_cast<float>(yPos)};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Math::Vec2 TRAP::Input::GetMousePosition(const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to GetMousePosition!");
		return TRAP::Math::Vec2{};
	}

	double xPos = 0.0, yPos = 0.0;
	INTERNAL::WindowingAPI::GetCursorPos(*static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), xPos, yPos);

#ifdef TRAP_PLATFORM_WINDOWS
	const TRAP::Math::Vec2i windowPos = window->GetPosition();
	xPos += windowPos.x;
	yPos += windowPos.y;
#endif /*TRAP_PLATFORM_WINDOWS*/

	return TRAP::Math::Vec2{ static_cast<float>(xPos), static_cast<float>(yPos) };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Input::GetMouseX()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetMousePosition().x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Input::GetMouseY()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetMousePosition().y;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Input::GetMouseX(const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetMousePosition(window).x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Input::GetMouseY(const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return GetMousePosition(window).y;
}

//-------------------------------------------------------------------------------------------------------------------//

/// <summary>
/// Get a string representation for a non printable key (space key for example).
/// </summary>
/// <param name="key">Key to get.</param>
/// <returns>String representation.</returns>
[[nodiscard]] std::string NonPrintableKeyToString(const TRAP::Input::Key key) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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

[[nodiscard]] std::string TRAP::Input::GetKeyName(const Key key)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	const char* const name = INTERNAL::WindowingAPI::GetKeyName(key, 0);

	if (name == nullptr)
		return NonPrintableKeyToString(key);

	return name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Input::GetControllerAxis(const Controller controller, const ControllerAxis axis)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return 0.0f;

	return GetMappedControllerAxis(controller, axis);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPad(const Controller controller, const uint32_t dpad)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return ControllerDPad::Centered;

	return GetMappedControllerDPad(controller, dpad);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::IsControllerButtonPressed(const Controller controller, const ControllerButton button)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return false;

	return GetMappedControllerButton(controller, button);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Input::GetControllerName(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return "";

	if (!PollController(controller, PollMode::Presence))
		return "";

	if(s_controllerInternal[static_cast<uint32_t>(controller)].mapping == nullptr)
		return s_controllerInternal[static_cast<uint32_t>(controller)].Name;

	return s_controllerInternal[static_cast<uint32_t>(controller)].mapping->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Input::GetControllerGUID(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return "";

	if (!PollController(controller, PollMode::Presence))
		return "";

	return s_controllerInternal[static_cast<uint32_t>(controller)].guid;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<float> TRAP::Input::GetAllControllerAxes(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Axes))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].Axes;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<bool> TRAP::Input::GetAllControllerButtons(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Buttons))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].Buttons;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPads(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Buttons))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].DPads;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(const Controller controller, const float leftMotor, const float rightMotor)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return;

	if (!PollController(controller, PollMode::Presence))
		return;

	if (leftMotor != s_controllerInternal[static_cast<uint32_t>(controller)].LeftMotor ||
		rightMotor != s_controllerInternal[static_cast<uint32_t>(controller)].RightMotor)
	{
		s_controllerInternal[static_cast<uint32_t>(controller)].LeftMotor = leftMotor;
		s_controllerInternal[static_cast<uint32_t>(controller)].RightMotor = rightMotor;
		SetControllerVibrationInternal(controller, leftMotor, rightMotor);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(const Controller controller, const Math::Vec2& intensity)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return;

	if (!PollController(controller, PollMode::Presence))
		return;

	if (intensity.x != s_controllerInternal[static_cast<uint32_t>(controller)].LeftMotor ||
		intensity.y != s_controllerInternal[static_cast<uint32_t>(controller)].RightMotor)
	{
		s_controllerInternal[static_cast<uint32_t>(controller)].LeftMotor = intensity.x;
		s_controllerInternal[static_cast<uint32_t>(controller)].RightMotor = intensity.y;
		SetControllerVibrationInternal(controller, intensity.x, intensity.y);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::ControllerBatteryStatus TRAP::Input::GetControllerBatteryStatus(Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return ControllerBatteryStatus::Wired;

	if(!PollController(controller, PollMode::Presence))
		return ControllerBatteryStatus::Wired;

	return GetControllerBatteryStatusInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const float x, const float y)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const float x, const float y, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

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
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), position.x, position.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const Math::Vec2& position, const Window* const window)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (window == nullptr)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to SetMousePosition!");
		return;
	}

	INTERNAL::WindowingAPI::SetCursorPos(*static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), position.x, position.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetClipboard(const std::string& str)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	INTERNAL::WindowingAPI::SetClipboardString(str);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Input::GetClipboard()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetEventCallback(const EventCallbackFn &callback) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_eventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerMappings(const std::string& map)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	Mapping mapping{};

	if(ParseMapping(mapping, map))
	{
		Mapping* const previous = FindMapping(mapping.guid);
		if (previous != nullptr)
			*previous = mapping;
		else
			s_mappings.push_back(mapping);
	}

	for(uint32_t cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		ControllerInternal* const con = &s_controllerInternal[cID];
		if(s_controllerInternal[cID].Connected)
			con->mapping = FindValidMapping(con);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::EventCallbackFn TRAP::Input::GetEventCallback() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return s_eventCallback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerInternal* TRAP::Input::AddInternalController(std::string name,
                                                                    std::string guid,
																	const int32_t axisCount,
																	const int32_t buttonCount,
																	const int32_t dpadCount)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	uint32_t cID = 0;
	for(cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		if (!s_controllerInternal[cID].Connected)
			break;
	}

	if (cID > static_cast<uint32_t>(Controller::Sixteen))
		return nullptr;

	ControllerInternal* const con = &s_controllerInternal[cID];
	con->Connected = true;
	con->Name = std::move(name);
	con->guid = std::move(guid);
	con->Axes.resize(axisCount);
	con->Buttons.resize(buttonCount + dpadCount * 4);
	con->DPads.resize(dpadCount);
	con->ButtonCount = buttonCount;
	con->mapping = FindValidMapping(con);

	TP_INFO(Log::InputControllerPrefix, "Controller: ", (con->mapping != nullptr ? con->mapping->Name : con->Name),
	        " (", cID, ") connected!");

	return con;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller DPad
void TRAP::Input::InternalInputControllerDPad(ControllerInternal* const con, const int32_t dpad, const uint8_t value)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	const int32_t base = con->ButtonCount + dpad * 4;

	con->Buttons[base + 0] = ((value & BIT(0)) != 0); //Up
	con->Buttons[base + 1] = ((value & BIT(1)) != 0); //Right
	con->Buttons[base + 2] = ((value & BIT(2)) != 0); //Down
	con->Buttons[base + 3] = ((value & BIT(3)) != 0); //Left

	if (con->Buttons[base + 1] && con->Buttons[base + 0])
		con->DPads[dpad] = ControllerDPad::Right_Up;
	else if (con->Buttons[base + 1] && con->Buttons[base + 2])
		con->DPads[dpad] = ControllerDPad::Right_Down;
	else if (con->Buttons[base + 3] && con->Buttons[base + 0])
		con->DPads[dpad] = ControllerDPad::Left_Up;
	else if (con->Buttons[base + 3] && con->Buttons[base + 2])
		con->DPads[dpad] = ControllerDPad::Left_Down;
	else if (con->Buttons[base + 0])
		con->DPads[dpad] = ControllerDPad::Up;
	else if (con->Buttons[base + 1])
		con->DPads[dpad] = ControllerDPad::Right;
	else if (con->Buttons[base + 2])
		con->DPads[dpad] = ControllerDPad::Down;
	else if (con->Buttons[base + 3])
		con->DPads[dpad] = ControllerDPad::Left;
	else
		con->DPads[dpad] = ControllerDPad::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller axis
void TRAP::Input::InternalInputControllerAxis(ControllerInternal* const con, const int32_t axis, const float value)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	con->Axes[axis] = value;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller button
void TRAP::Input::InternalInputControllerButton(ControllerInternal* const con, const int32_t button, const bool pressed)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	con->Buttons[button] = pressed;
}

//-------------------------------------------------------------------------------------------------------------------//

//Parse an SDL_GameControllerDB line and adds it to the mapping list
bool TRAP::Input::ParseMapping(Mapping& mapping, const std::string& str)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	struct Fields
	{
		std::string Name;
		MapElement* Element = nullptr;
	};
	const std::array<Fields, 22> fields =
	{
		{
			{"platform", nullptr},
			{"a", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::A)]},
			{"b", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::B)]},
			{"x", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::X)]},
			{"y", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Y)]},
			{"back", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Back)]},
			{"start", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Start)]},
			{"guide", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Guide)]},
			{"leftshoulder", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Left_Bumper)]},
			{"rightshoulder", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Right_Bumper)]},
			{"leftstick", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Left_Thumb)]},
			{"rightstick", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::Right_Thumb)]},
			{"dpup", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::DPad_Up)]},
			{"dpright", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::DPad_Right)]},
			{"dpdown", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::DPad_Down)]},
			{"dpleft", &mapping.Buttons[static_cast<uint32_t>(ControllerButton::DPad_Left)]},
			{"lefttrigger", &mapping.Axes[static_cast<uint32_t>(ControllerAxis::Left_Trigger)]},
			{"righttrigger", &mapping.Axes[static_cast<uint32_t>(ControllerAxis::Right_Trigger)]},
			{"leftx", &mapping.Axes[static_cast<uint32_t>(ControllerAxis::Left_X)]},
			{"lefty", &mapping.Axes[static_cast<uint32_t>(ControllerAxis::Left_Y)]},
			{"rightx", &mapping.Axes[static_cast<uint32_t>(ControllerAxis::Right_X)]},
			{"righty", &mapping.Axes[static_cast<uint32_t>(ControllerAxis::Right_Y)]}
		}
	};

	const std::vector<std::string> splittedString = Utils::String::SplitString(str, ',');

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
	mapping.guid = Utils::String::ToLower(splittedString[0]);

	if(splittedString[1].empty())
	{
		TP_ERROR(Log::InputControllerPrefix, "Mapping name can't be empty!");
		return false;
	}
	mapping.Name = splittedString[1];

	for (uint8_t i = 2; i < splittedString.size();) //Start after Mapping Name
	{
		const std::vector<std::string> splittedField = Utils::String::SplitString(splittedString[i] + ':', ':');
		if (splittedField.empty())
		{
			TP_ERROR(Log::InputControllerPrefix, "Field can't be empty! Mapping: ", splittedString[1]);
			return false;
		}
		if (splittedField.size() < 2)
		{
			TP_ERROR(Log::InputControllerPrefix, "Too few elements inside field: ", static_cast<uint32_t>(i),
			         "! Mapping: ", splittedString[1]);
			return false;
		}
		if (splittedField.size() > 2)
		{
			TP_ERROR(Log::InputControllerPrefix, "Too many elements inside field: ", static_cast<uint32_t>(i),
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
			if (std::isalnum(static_cast<int8_t>(c)) == 0)
			{
				TP_ERROR(Log::InputControllerPrefix, "Invalid char inside field: ", static_cast<uint32_t>(i),
				         "! Mapping: ", splittedString[1]);
				return false;
			}
		}

		bool found = false;
		uint8_t j = 0;
		for (j = 0; j < static_cast<uint8_t>(fields.size()); j++)
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
			int8_t minimum = -1;
			int8_t maximum = 1;
			uint32_t charOffset = 0;

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
				const uint64_t hat = std::stoul(splittedField[1].data() + charOffset);
				const uint64_t bit = std::stoul(splittedField[1].data() + charOffset);
				e->Index = static_cast<uint8_t>((hat << 4u) | bit);
			}
			else
				e->Index = static_cast<uint8_t>(std::stoul(splittedField[1].data() + charOffset));

			if (e->Type == 1) //Axis
			{
				e->AxisScale = static_cast<int8_t>(2 / (maximum - minimum));
				e->AxisOffset = static_cast<int8_t>(-(maximum + minimum));

				//Invert axis input modifier
				if (splittedField[1][charOffset] == '~')
				{
					e->AxisScale = static_cast<int8_t>(-e->AxisScale);
					e->AxisOffset = static_cast<int8_t>(-e->AxisOffset);
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
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	s_mappings.resize(Embed::ControllerMappings.size());

	for(std::size_t i = 0; i < Embed::ControllerMappings.size(); ++i)
		ParseMapping(s_mappings[i], Embed::ControllerMappings[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

//Find a mapping based on controller GUID
[[nodiscard]] TRAP::Input::Mapping* TRAP::Input::FindMapping(const std::string_view guid)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	for (auto& Mapping : s_mappings)
	{
		if(Mapping.guid == guid)
			return &Mapping;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Finds a mapping based on controller GUID and verifies element indices
[[nodiscard]] TRAP::Input::Mapping* TRAP::Input::FindValidMapping(const ControllerInternal* const con)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	Mapping* const mapping = FindMapping(con->guid);

	if(mapping == nullptr)
		return nullptr;

	uint8_t i = 0;

	for(i = 0; i <= static_cast<uint8_t>(ControllerButton::DPad_Left); i++)
	{
		if(!IsValidElementForController(&mapping->Buttons[i], con))
		{
			TP_ERROR(Log::InputControllerPrefix, "Invalid button in controller mapping: ", mapping->guid,
						" ", mapping->Name);
			return nullptr;
		}
	}

	for(i = 0; i <= static_cast<uint8_t>(ControllerAxis::Right_Trigger); i++)
	{
		if(!IsValidElementForController(&mapping->Axes[i], con))
		{
			TP_ERROR(Log::InputControllerPrefix, "Invalid axis in controller mapping: ", mapping->guid,
						" ", mapping->Name);
			return nullptr;
		}
	}

	return mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

//Checks whether a controller mapping element is present in the hardware
[[nodiscard]] bool TRAP::Input::IsValidElementForController(const MapElement* const e, const ControllerInternal* const con)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(e->Type == 3 && (e->Index >> 4u) >= static_cast<int32_t>(con->DPads.size() + 1))
		return false;
	if(e->Type == 2 && e->Index >= (con->Buttons.size() + 1))
		return false;
	if(e->Type == 1 && e->Index >= (con->Axes.size() + 1))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::GetMappedControllerButton(Controller controller, ControllerButton button)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (!PollController(controller, PollMode::Buttons))
		return false;

	const ControllerInternal* const con = &s_controllerInternal[static_cast<uint32_t>(controller)];

	if (con->mapping == nullptr)
		return false;

	const MapElement* const e = &con->mapping->Buttons[static_cast<uint8_t>(button)];
	if (e->Index < con->ButtonCount)
	{
		if (e->Type == 2) //Button
			return con->Buttons[e->Index];
	}
	if (e->Type == 1) //Axis
	{
		const float value = con->Axes[e->Index] * static_cast<float>(e->AxisScale) +
		                    static_cast<float>(e->AxisOffset);
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

[[nodiscard]] float TRAP::Input::GetMappedControllerAxis(const Controller controller, const ControllerAxis axis)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(!PollController(controller, PollMode::Axes))
		return 0.0f;

	const ControllerInternal* const con = &s_controllerInternal[static_cast<uint32_t>(controller)];

	if(con->mapping == nullptr)
		return 0.0f;

	const MapElement* const e = &con->mapping->Axes[static_cast<uint8_t>(axis)];
	if(e->Type == 1) //Axis
	{
		const float value = con->Axes[e->Index] * static_cast<float>(e->AxisScale) +
		                    static_cast<float>(e->AxisOffset);
		return Math::Min(Math::Max(value, -1.0f), 1.0f);
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

[[nodiscard]] TRAP::Input::ControllerDPad TRAP::Input::GetMappedControllerDPad(const Controller controller, const uint32_t dpad)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(!PollController(controller, PollMode::All))
		return ControllerDPad::Centered;

	const ControllerInternal* const con = &s_controllerInternal[static_cast<uint32_t>(controller)];

	if (con->mapping == nullptr)
		return ControllerDPad::Centered;

	const MapElement* const e = &con->mapping->Buttons[11 + (dpad * 4)];
	if(e->Type == 3)
		return con->DPads[e->Index >> 4u];

	return ControllerDPad::Centered;
}

#endif /*TRAP_HEADLESS_MODE*/
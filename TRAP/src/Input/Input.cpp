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
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::InputPrefix, "Initializing");

	InitControllerMappings();

	if(!InitController())
		TP_ERROR(Log::InputControllerPrefix, "Failed to initialize controller support!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Shutdown()
{
	TP_PROFILE_FUNCTION();

	TP_DEBUG(Log::InputPrefix, "Shutting down input");

	ShutdownController();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key)
{
	TP_PROFILE_FUNCTION();

	if (key == Key::Unknown)
	{
		TP_WARN(Log::InputPrefix, "Invalid key provided!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                  (Application::GetWindow()->GetInternalWindow()), key);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key, const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	if (key == Key::Unknown)
	{
		TP_WARN(Log::InputPrefix, "Invalid key provided!");
		return false;
	}
	if(!window)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to IsKeyPressed!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                  (window->GetInternalWindow()), key);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	TP_PROFILE_FUNCTION();

	const auto state = INTERNAL::WindowingAPI::GetMouseButton(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                          (Application::GetWindow()->GetInternalWindow()),
															  button);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button, const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	if (!window)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to IsMouseButtonPressed!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetMouseButton(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                                          (window->GetInternalWindow()), button);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::RawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerConnected(const Controller controller)
{
	return s_controllerInternal[static_cast<uint32_t>(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerGamepad(const Controller controller)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return false;

	if (!PollController(controller, PollMode::Presence))
		return false;

	return s_controllerInternal[static_cast<uint32_t>(controller)].mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	TP_PROFILE_FUNCTION();

	double xPos = 0.0, yPos = 0.0;
	INTERNAL::WindowingAPI::GetCursorPos(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), xPos, yPos);

	return {static_cast<float>(xPos), static_cast<float>(yPos)};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition(const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	if(!window)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to GetMousePosition!");
		return TRAP::Math::Vec2{};
	}

	double xPos = 0.0, yPos = 0.0;
	INTERNAL::WindowingAPI::GetCursorPos(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), xPos, yPos);

	return TRAP::Math::Vec2{ static_cast<float>(xPos), static_cast<float>(yPos) };
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMouseX()
{
	TP_PROFILE_FUNCTION();

	return GetMousePosition().x;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMouseY()
{
	TP_PROFILE_FUNCTION();

	return GetMousePosition().y;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMouseX(const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	return GetMousePosition(window).x;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMouseY(const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	return GetMousePosition(window).y;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetKeyName(const Key key)
{
	TP_PROFILE_FUNCTION();

	if (!INTERNAL::WindowingAPI::GetKeyName(key, 0))
	{
		TP_ERROR(Log::InputPrefix, "Couldn't get name of key: ", static_cast<uint32_t>(key), "!");
		return "";
	}

	return INTERNAL::WindowingAPI::GetKeyName(key, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetControllerAxis(const Controller controller, const ControllerAxis axis)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return 0.0f;

	return GetMappedControllerAxis(controller, axis);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPad(const Controller controller, const uint32_t dpad)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return ControllerDPad::Centered;

	return GetMappedControllerDPad(controller, dpad);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerButtonPressed(const Controller controller, const ControllerButton button)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return false;

	return GetMappedControllerButton(controller, button);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerName(const Controller controller)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return "";

	if (!PollController(controller, PollMode::Presence))
		return "";

	if(!s_controllerInternal[static_cast<uint32_t>(controller)].mapping)
		return s_controllerInternal[static_cast<uint32_t>(controller)].Name;

	return s_controllerInternal[static_cast<uint32_t>(controller)].mapping->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerGUID(const Controller controller)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return "";

	if (!PollController(controller, PollMode::Presence))
		return "";

	return s_controllerInternal[static_cast<uint32_t>(controller)].guid;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxes(const Controller controller)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Axes))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].Axes;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtons(const Controller controller)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Buttons))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].Buttons;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPads(const Controller controller)
{
	TP_PROFILE_FUNCTION();

	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, PollMode::Buttons))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].DPads;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(const Controller controller, const float leftMotor, const float rightMotor)
{
	TP_PROFILE_FUNCTION();

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
	TP_PROFILE_FUNCTION();

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

TRAP::Input::ControllerBatteryStatus TRAP::Input::GetControllerBatteryStatus(Controller controller)
{
	TP_PROFILE_FUNCTION();

	if(!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return ControllerBatteryStatus::Wired;

	if(!PollController(controller, PollMode::Presence))
		return ControllerBatteryStatus::Wired;

	return GetControllerBatteryStatusInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const float x, const float y)
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetCursorPos(static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const float x, const float y, const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	if (!window)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to SetMousePosition!");
		return;
	}

	INTERNAL::WindowingAPI::SetCursorPos(static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), x, y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const Math::Vec2& position)
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetCursorPos(static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (Application::GetWindow()->GetInternalWindow()), position.x, position.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetMousePosition(const Math::Vec2& position, const Scope<Window>& window)
{
	TP_PROFILE_FUNCTION();

	if (!window)
	{
		TP_WARN(Log::InputPrefix, "Tried to pass nullptr to SetMousePosition!");
		return;
	}

	INTERNAL::WindowingAPI::SetCursorPos(static_cast<INTERNAL::WindowingAPI::InternalWindow*>
	                                     (window->GetInternalWindow()), position.x, position.y);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetClipboard(const std::string& str)
{
	TP_PROFILE_FUNCTION();

	INTERNAL::WindowingAPI::SetClipboardString(str);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetClipboard()
{
	TP_PROFILE_FUNCTION();

	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetEventCallback(const EventCallbackFn &callback)
{
	s_eventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerMappings(const std::string_view map)
{
	TP_PROFILE_FUNCTION();

	Mapping mapping{};

	if(ParseMapping(mapping, map))
	{
		Mapping* previous = FindMapping(mapping.guid);
		if (previous)
			*previous = mapping;
		else
			s_mappings.push_back(mapping);
	}

	for(uint32_t cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		ControllerInternal* con = &s_controllerInternal[cID];
		if(s_controllerInternal[cID].Connected)
			con->mapping = FindValidMapping(con);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::EventCallbackFn TRAP::Input::GetEventCallback()
{
	return s_eventCallback;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerInternal* TRAP::Input::AddInternalController(std::string name,
                                                                    std::string guid,
																	const int32_t axisCount,
																	const int32_t buttonCount,
																	const int32_t dpadCount)
{
	uint32_t cID = 0;
	for(cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		if (!s_controllerInternal[cID].Connected)
			break;
	}

	if (cID > static_cast<uint32_t>(Controller::Sixteen))
		return nullptr;

	ControllerInternal* con = &s_controllerInternal[cID];
	con->Connected = true;
	con->Name = std::move(name);
	con->guid = std::move(guid);
	con->Axes.resize(axisCount);
	con->Buttons.resize(buttonCount + dpadCount * 4);
	con->DPads.resize(dpadCount);
	con->ButtonCount = buttonCount;
	con->mapping = FindValidMapping(con);

	TP_INFO(Log::InputControllerPrefix, "Controller: ", (con->mapping ? con->mapping->Name : con->Name),
	        " (", cID, ") connected!");

	return con;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller DPad
void TRAP::Input::InternalInputControllerDPad(ControllerInternal* con, const int32_t dpad, const uint8_t value)
{
	const int32_t base = con->ButtonCount + dpad * 4;

	con->Buttons[base + 0] = (value & BIT(0)); //Up
	con->Buttons[base + 1] = (value & BIT(1)); //Right
	con->Buttons[base + 2] = (value & BIT(2)); //Down
	con->Buttons[base + 3] = (value & BIT(3)); //Left

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
void TRAP::Input::InternalInputControllerAxis(ControllerInternal* con, const int32_t axis, const float value)
{
	con->Axes[axis] = value;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller button
void TRAP::Input::InternalInputControllerButton(ControllerInternal* con, const int32_t button, const bool pressed)
{
	con->Buttons[button] = pressed;
}

//-------------------------------------------------------------------------------------------------------------------//

//Parse an SDL_GameControllerDB line and adds it to the mapping list
bool TRAP::Input::ParseMapping(Mapping& mapping, const std::string_view str)
{
	struct Fields
	{
		std::string Name;
		MapElement* Element = nullptr;
	};
	std::array<Fields, 22> fields =
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

	std::vector<std::string> splittedString = Utils::String::SplitString(str, ',');

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
		std::vector<std::string> splittedField = Utils::String::SplitString(splittedString[i] + ':', ':');
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
			if (!std::isalnum(static_cast<int8_t>(c)))
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

		if(found && fields[j].Element)
		{
			MapElement* e = fields[j].Element;
			int8_t minimum = -1;
			int8_t maximum = 1;
			uint32_t charOffset = 0;

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
				e->Index = static_cast<uint8_t>((hat << 4) | bit);
			}
			else
				e->Index = static_cast<uint8_t>(std::stoul(splittedField[1].data() + charOffset));

			if (e->Type == 1) //Axis
			{
				e->AxisScale = static_cast<int8_t>(2 / (maximum - minimum));
				e->AxisOffset = static_cast<int8_t>(-(maximum + minimum));

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
	s_mappings.resize(Embed::ControllerMappings.size());

	for(std::size_t i = 0; i < Embed::ControllerMappings.size(); ++i)
		ParseMapping(s_mappings[i], Embed::ControllerMappings[i]);
}

//-------------------------------------------------------------------------------------------------------------------//

//Find a mapping based on controller GUID
TRAP::Input::Mapping* TRAP::Input::FindMapping(const std::string_view guid)
{
	for (auto& Mapping : s_mappings)
	{
		if(Mapping.guid == guid)
			return &Mapping;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Finds a mapping based on controller GUID and verifies element indices
TRAP::Input::Mapping* TRAP::Input::FindValidMapping(const ControllerInternal* con)
{
	Mapping* mapping = FindMapping(con->guid);

	if(!mapping)
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
bool TRAP::Input::IsValidElementForController(const MapElement* e, const ControllerInternal* con)
{
	if(e->Type == 3 && (e->Index >> 4) >= static_cast<int32_t>(con->DPads.size() + 1))
		return false;
	if(e->Type == 2 && e->Index >= (con->Buttons.size() + 1))
		return false;
	if(e->Type == 1 && e->Index >= (con->Axes.size() + 1))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::GetMappedControllerButton(Controller controller, ControllerButton button)
{
	if (!PollController(controller, PollMode::Buttons))
		return false;

	ControllerInternal* con = &s_controllerInternal[static_cast<uint32_t>(controller)];

	if (!con->mapping)
		return false;

	const MapElement* e = &con->mapping->Buttons[static_cast<uint8_t>(button)];
	if (e->Index < con->ButtonCount)
		if (e->Type == 2) //Button
			return con->Buttons[e->Index];
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
		if (con->Buttons[e->Index >> 4])
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMappedControllerAxis(Controller controller, ControllerAxis axis)
{
	if(!PollController(controller, PollMode::Axes))
		return 0.0f;

	ControllerInternal* con = &s_controllerInternal[static_cast<uint32_t>(controller)];

	if(!con->mapping)
		return 0.0f;

	const MapElement* e = &con->mapping->Axes[static_cast<uint8_t>(axis)];
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
		if(con->Buttons[e->Index >>  4])
			return 1.0f;

		return -1.0f;
	}

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetMappedControllerDPad(Controller controller, const uint32_t dpad)
{
	if(!PollController(controller, PollMode::All))
		return ControllerDPad::Centered;

	ControllerInternal* con = &s_controllerInternal[static_cast<uint32_t>(controller)];

	if (!con->mapping)
		return ControllerDPad::Centered;

	const MapElement* e = &con->mapping->Buttons[11 + (dpad * 4)];
	if(e->Type == 3)
		return con->DPads[e->Index >> 4];

	return ControllerDPad::Centered;
}
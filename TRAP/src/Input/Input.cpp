#include "TRAPPCH.h"
#include "Input.h"

#include "Application.h"
#include "Utils/String.h"
#include "Window/WindowingAPI.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::EventCallbackFn TRAP::Input::s_eventCallback{};
std::array<TRAP::Input::ControllerInternal, 16> TRAP::Input::s_controllerInternal{};
std::vector<TRAP::Input::Mapping> TRAP::Input::Mappings{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init()
{
	if(!InitController())
		TP_ERROR("[Input][Controller] Failed to initialize Controller support!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Shutdown()
{
	ShutdownController();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key)
{
	if (key == Key::Unknown)
	{
		TP_WARN("[Input] Invalid Key provided!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>(Application::GetWindow()->GetInternalWindow()), key);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key, const Scope<Window>& window)
{
	if (key == Key::Unknown)
	{
		TP_WARN("[Input] Invalid Key provided!");
		return false;
	}
	if(!window)
	{
		TP_WARN("[Input] Tried to pass nullptr to IsKeyPressed!");
		return false;
	}
	
	const auto state = INTERNAL::WindowingAPI::GetKey(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()), key);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	const auto state = INTERNAL::WindowingAPI::GetMouseButton(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>(Application::GetWindow()->GetInternalWindow()), button);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button, const Scope<Window>& window)
{
	if (!window)
	{
		TP_WARN("[Input] Tried to pass nullptr to IsMouseButtonPressed!");
		return false;
	}
	
	const auto state = INTERNAL::WindowingAPI::GetMouseButton(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()), button);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	return INTERNAL::WindowingAPI::RawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerConnected(const Controller controller)
{	
	return s_controllerInternal[static_cast<uint32_t>(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerGamepad(Controller controller)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return false;

	if (!PollController(controller, Poll_Presence))
		return false;
	
	return s_controllerInternal[static_cast<uint32_t>(controller)].mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	double xPos, yPos;
	INTERNAL::WindowingAPI::GetCursorPos(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>(Application::GetWindow()->GetInternalWindow()), xPos, yPos);

	return {static_cast<float>(xPos), static_cast<float>(yPos)};
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition(const Scope<Window>& window)
{
	if(!window)
	{
		TP_WARN("[Input] Tried to pass nullptr to GetMousePosition!");
		return {0.0f, 0.0f};
	}
	
	double xPos, yPos;
	INTERNAL::WindowingAPI::GetCursorPos(static_cast<const INTERNAL::WindowingAPI::InternalWindow*>(window->GetInternalWindow()), xPos, yPos);

	return { static_cast<float>(xPos), static_cast<float>(yPos) };
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMouseX()
{
	auto [x, y] = GetMousePosition();

	return x;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMouseY()
{
	auto [x, y] = GetMousePosition();

	return y;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetKeyName(const Key key)
{
	if (INTERNAL::WindowingAPI::GetKeyName(key, 0))
		return INTERNAL::WindowingAPI::GetKeyName(key, 0);	

	TP_ERROR("[Input] Couldn't get name of Key: ", static_cast<uint32_t>(key), "!");
	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetControllerAxis(Controller controller, const ControllerAxis axis)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return 0.0f;

	return GetMappedControllerAxis(controller, axis);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPad(Controller controller, const uint32_t dpad)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return ControllerDPad::Centered;

	return GetMappedControllerDPad(controller, dpad);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::GetControllerButton(Controller controller, const ControllerButton button)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return false;

	return GetMappedControllerButton(controller, button);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerName(Controller controller)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return "";

	if (!PollController(controller, Poll_Presence))
		return "";

	if(!s_controllerInternal[static_cast<uint32_t>(controller)].mapping)
		return s_controllerInternal[static_cast<uint32_t>(controller)].Name;

	return s_controllerInternal[static_cast<uint32_t>(controller)].mapping->Name;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerGUID(Controller controller)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return "";

	if (!PollController(controller, Poll_Presence))
		return "";

	return s_controllerInternal[static_cast<uint32_t>(controller)].guid;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxes(Controller controller)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, Poll_Axes))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].Axes;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtons(Controller controller)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, Poll_Buttons))
		return {};

	return s_controllerInternal[static_cast<uint32_t>(controller)].Buttons;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPads(Controller controller)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return {};

	if (!PollController(controller, Poll_Buttons))
		return {};
	
	return s_controllerInternal[static_cast<uint32_t>(controller)].DPads;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(Controller controller, const float leftMotor, const float rightMotor)
{
	if (!s_controllerInternal[static_cast<uint32_t>(controller)].Connected)
		return;

	if (!PollController(controller, Poll_Presence))
		return;

	SetControllerVibrationInternal(controller, leftMotor, rightMotor);	
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetClipboard(const std::string& str)
{
	INTERNAL::WindowingAPI::SetClipboardString(str);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetClipboard()
{
	return INTERNAL::WindowingAPI::GetClipboardString();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetEventCallback(const EventCallbackFn &callback)
{
	s_eventCallback = callback;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerMappings(const std::string& map)
{
	Mapping mapping{};

	if(ParseMapping(mapping, map))
	{
		Mapping* previous = FindMapping(mapping.guid);
		if (previous)
			*previous = mapping;
		else
			Mappings.push_back(mapping);
	}
	
	for(uint32_t cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		ControllerInternal* con = &s_controllerInternal[cID];
		if(s_controllerInternal[cID].Connected)
			con->mapping = FindValidMapping(con);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerInternal* TRAP::Input::AddInternalController(const std::string& name, const std::string& guid, const int32_t axisCount, const int32_t buttonCount, const int32_t dpadCount)
{
	uint32_t cID;
	for(cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		if (!s_controllerInternal[cID].Connected)
			break;
	}

	if (cID > static_cast<uint32_t>(Controller::Sixteen))
		return nullptr;
	
	ControllerInternal* con = &s_controllerInternal[cID];
	con->Connected = true;
	con->Name = name;
	con->guid = guid;
	con->Axes.resize(axisCount);
	con->Buttons.resize(buttonCount + dpadCount * 4);
	con->DPads.resize(dpadCount);
	con->ButtonCount = buttonCount;
	con->mapping = FindValidMapping(con);

	TP_INFO("[Input][Controller] Controller: ", (con->mapping ? con->mapping->Name : con->Name), " (", cID, ") Connected!");
	
	return con;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller DPad
void TRAP::Input::InternalInputControllerDPad(ControllerInternal* con, const int32_t dpad, const uint8_t value)
{
	const int32_t base = con->ButtonCount + dpad * 4;

	con->Buttons[base + 0] = (value & 0x01) ? true : false; //Up
	con->Buttons[base + 1] = (value & 0x02) ? true : false; //Right
	con->Buttons[base + 2] = (value & 0x04) ? true : false; //Down
	con->Buttons[base + 3] = (value & 0x08) ? true : false; //Left

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
bool TRAP::Input::ParseMapping(Mapping& mapping, const std::string& str)
{
	struct Fields
	{
		std::string Name = nullptr;
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
		TP_ERROR("[Input][Controller] Map is empty!");
		return false;
	}
	if (splittedString.size() > 24)
	{
		TP_ERROR("[Input][Controller] Map has too many elements! There must be less than 24 elements!");
		return false;
	}

	if(splittedString[0].size() != 32)
	{
		TP_ERROR("[Input][Controller] Invalid GUID size! Must be 32 bytes long");
		return false;
	}

	if (splittedString[0].empty())
	{
		TP_ERROR("[Input][Controller] Mapping GUID can't be empty!");
		return false;
	}
	mapping.guid = splittedString[0] + '\0';
	
	if(splittedString[1].empty())
	{
		TP_ERROR("[Input][Controller] Mapping Name can't be empty!");
		return false;
	}
	mapping.Name = splittedString[1] + '\0';

	for (uint8_t i = 2; i < splittedString.size();) //Start after Mapping Name
	{
		std::vector<std::string> splittedField = Utils::String::SplitString(splittedString[i] + ':', ':');
		if (splittedField.empty())
		{
			TP_ERROR("[Input][Controller] Field can't be empty!");
			return false;
		}
		if (splittedField.size() < 2)
		{
			TP_ERROR("[Input][Controller] Too few elements inside field: ", i, "!");
			return false;
		}
		if (splittedField.size() > 2)
		{
			TP_ERROR("[Input][Controller] Too many elements inside field: ", i, "!");
			return false;
		}

		for (const auto& c : splittedField[0])
			if (!std::isalnum(static_cast<int8_t>(c)))
			{
				TP_ERROR("[Input][Controller] Invalid char inside field: ", i, "!");
				return false;
			}

		uint8_t j;
		for (j = 0; j < fields.size(); j++)
			if (fields[j].Name == splittedField[0])
				break;

		if(fields[j].Element)
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
				e->AxisScale = 2 / (maximum - minimum);
				e->AxisOffset = -(maximum + minimum);

				if (splittedField[1][charOffset] == '~')
				{
					e->AxisScale = -e->AxisScale;
					e->AxisOffset = -e->AxisOffset;
				}
			}
		}

		i++;
	}

	mapping.guid = Utils::String::ToLower(mapping.guid);

	UpdateControllerGUID(mapping.guid);
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Find a mapping based on controller GUID
TRAP::Input::Mapping* TRAP::Input::FindMapping(const std::string& guid)
{
	for (auto& Mapping : Mappings)
		if(Mapping.guid == guid)
			return &Mapping;
	
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Finds a mapping based on controller GUID and verifies element indices
TRAP::Input::Mapping* TRAP::Input::FindValidMapping(const ControllerInternal* con)
{
	Mapping* mapping = FindMapping(con->guid);
	if(mapping)
	{
		uint8_t i;
		
		for(i = 0; i <= static_cast<uint8_t>(ControllerButton::DPad_Left); i++)
		{
			if(!IsValidElementForController(&mapping->Buttons[i], con))
			{
				TP_ERROR("[Input][Controller] Invalid button in Controller mapping: ", mapping->guid, " ", mapping->Name);
				return nullptr;
			}
		}
		
		for(i = 0; i <= static_cast<uint8_t>(ControllerAxis::Right_Trigger); i++)
		{
			if(!IsValidElementForController(&mapping->Axes[i], con))
			{
				TP_ERROR("[Input][Controller] Invalid axis in Controller mapping: ", mapping->guid, " ", mapping->Name);
				return nullptr;
			}
		}
	}
	
	return mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

//Checks whether a controller mapping element is present in the hardware
bool TRAP::Input::IsValidElementForController(const MapElement* e, const ControllerInternal* con)
{
	if(e->Type == 3 && (e->Index >> 4) >= (con->DPads.size() + 1))
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
	if (!PollController(controller, Poll_Buttons))
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
		const float value = con->Axes[e->Index] * static_cast<float>(e->AxisScale) + static_cast<float>(e->AxisOffset);
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
	if(!PollController(controller, Poll_Axes))
		return 0.0f;
		
	ControllerInternal* con = &s_controllerInternal[static_cast<uint32_t>(controller)];
		
	if(!con->mapping)
		return 0.0f;
		
	const MapElement* e = &con->mapping->Axes[static_cast<uint8_t>(axis)];
	if(e->Type == 1) //Axis
	{
		const float value = con->Axes[e->Index] * static_cast<float>(e->AxisScale) + static_cast<float>(e->AxisOffset);
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
	if(!PollController(controller, Poll_All))
		return ControllerDPad::Centered;
		
	ControllerInternal* con = &s_controllerInternal[static_cast<uint32_t>(controller)];
		
	if (!con->mapping)
		return ControllerDPad::Centered;
		
	const MapElement* e = &con->mapping->Buttons[11 + (dpad * 4)];
	if(e->Type == 3)
		return con->DPads[e->Index >> 4];
		
	return ControllerDPad::Centered;
}
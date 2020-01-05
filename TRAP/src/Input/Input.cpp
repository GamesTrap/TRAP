#include "TRAPPCH.h"
#include "Input.h"

#include "Application.h"
#include "Event/ControllerEvent.h"
#include "Utils/String.h"
#include "Window/WindowingAPI.h"

//-------------------------------------------------------------------------------------------------------------------//

std::array<TRAP::Input::ControllerStatus, 4> TRAP::Input::s_controllerStatuses;
TRAP::Input::EventCallbackFn TRAP::Input::s_eventCallback{};
TRAP::Input::ControllerAPI TRAP::Input::s_controllerAPI = ControllerAPI::Unknown;
std::array<TRAP::Input::ControllerInternal, 4> TRAP::Input::s_controllerInternal{};
std::vector<TRAP::Input::Mapping> TRAP::Input::Mappings{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init(const ControllerAPI controllerAPI)
{
	s_controllerAPI = controllerAPI;
	
#ifdef TRAP_PLATFORM_WINDOWS
	if (s_controllerAPI == ControllerAPI::Unknown || s_controllerAPI == ControllerAPI::Linux)
		s_controllerAPI = ControllerAPI::XInput;
#elif defined(TRAP_PLATFORM_LINUX)
	if (s_controllerAPI != ControllerAPI::Linux)
		s_controllerAPI = ControllerAPI::Linux;
#endif

	if(!InitController())
	{
		switch(s_controllerAPI)
		{
		case ControllerAPI::XInput:
			TP_ERROR("[Input][Controller][XInput] Failed to initialize controller support for XInput!");
			break;
			
		case ControllerAPI::DirectInput:
			TP_ERROR("[Input][Controller][DirectInput] Failed to initialize controller support for DirectInput!");
			break;
			
		case ControllerAPI::Linux:
			TP_ERROR("[Input][Controller][Linux] Failed to initialize controller support for Linux!");
			break;
			
		default:
			break;
		}
	}

}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Shutdown()
{
	ShutdownController();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerAPI TRAP::Input::GetControllerAPI()
{
	return s_controllerAPI;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerAPI(const ControllerAPI controllerAPI)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if (controllerAPI != s_controllerAPI)
		ShutdownController();
	
	s_controllerStatuses = {};
	s_controllerAPI = controllerAPI;

	if (s_controllerAPI == ControllerAPI::Unknown || s_controllerAPI == ControllerAPI::Linux)
		s_controllerAPI = ControllerAPI::XInput;

	InitController();
#endif
	//Linux is ignored because it only got 1 supported API for controllers
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key)
{
	if (key == Key::Invalid || key == Key::Unknown)
	{
		TP_WARN("[Input] Invalid Key provided!");
		return false;
	}

	const auto state = INTERNAL::WindowingAPI::GetKey(Application::GetWindow()->GetInternalWindow(), key);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key, const Scope<Window>& window)
{
	if (key == Key::Invalid || key == Key::Unknown)
	{
		TP_WARN("[Input] Invalid Key provided!");
		return false;
	}
	if(!window)
	{
		TP_WARN("[Input] Tried to pass nullptr to IsKeyPressed!");
		return false;
	}
	
	const auto state = INTERNAL::WindowingAPI::GetKey(window->GetInternalWindow(), key);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	const auto state = INTERNAL::WindowingAPI::GetMouseButton(Application::GetWindow()->GetInternalWindow(), button);

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
	
	const auto state = INTERNAL::WindowingAPI::GetMouseButton(window->GetInternalWindow(), button);

	return state;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsGamepadButtonPressed(Controller controller, const ControllerButton button)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return false;
	}

	if (!IsControllerGamepad(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not a Gamepad!");
		return false;
	}

	PollController(controller, 0);
	
	return IsMappedControllerButtonPressed(controller, button);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	return INTERNAL::WindowingAPI::RawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerConnected(const Controller controller)
{	
	return s_controllerStatuses[static_cast<uint8_t>(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerGamepad(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return false;
	}
	
	if(s_controllerAPI == ControllerAPI::XInput)
		return true;
	
	return s_controllerInternal[static_cast<uint8_t>(controller)].mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	double xPos, yPos;
	INTERNAL::WindowingAPI::GetCursorPos(Application::GetWindow()->GetInternalWindow(), xPos, yPos);

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
	INTERNAL::WindowingAPI::GetCursorPos(window->GetInternalWindow(), xPos, yPos);

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
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return 0.0f;
	}

	PollController(controller, 0);
	
	if (s_controllerAPI != ControllerAPI::Unknown)
		return GetMappedControllerAxis(controller, axis);

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPad(Controller controller, const uint32_t dpad)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return ControllerDPad::Centered;
	}

	PollController(controller, 0);
	
	if (s_controllerAPI != ControllerAPI::Unknown)
		return GetMappedControllerDPad(controller, dpad);

	return ControllerDPad::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerName(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return "";
	}

	return GetControllerNameInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Input::ControllerStatus &TRAP::Input::GetControllerStatus(Controller controller)
{
	return s_controllerStatuses[static_cast<uint8_t>(controller)];
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxes(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return {};
	}

	PollController(controller, 0);

	return GetAllControllerAxesInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtons(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return {};
	}

	PollController(controller, 0);

	return GetAllControllerButtonsInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPads(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return {};
	}

	PollController(controller, 0);
	
	return GetAllControllerDPadsInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::Input::ControllerStatus, 4> &TRAP::Input::GetAllControllerStatuses()
{
	return s_controllerStatuses;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(Controller controller, const float leftMotor, const float rightMotor)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<uint32_t>(controller), " is not connected!");
		return;
	}

	PollController(controller, 0);

	SetControllerVibrationInternal(controller, leftMotor, rightMotor);
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
	
	for(uint8_t cID = 0; cID <= static_cast<uint8_t>(Controller::Four); cID++)
	{
		ControllerInternal* js = &s_controllerInternal[cID];
		if(s_controllerStatuses[cID].Connected)
			js->mapping = FindValidMapping(js);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<ControllerConnectEvent>([](ControllerConnectEvent& e) {return OnControllerConnectEvent(e); });
	dispatcher.Dispatch<ControllerDisconnectEvent>([](ControllerDisconnectEvent& e) {return OnControllerDisconnectEvent(e); });
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::OnControllerConnectEvent(ControllerConnectEvent &e)
{
	s_controllerStatuses[static_cast<uint8_t>(e.GetController())].Connected = true;

	if (s_controllerStatuses[static_cast<uint8_t>(e.GetController())].Connected) //Connected
		TP_DEBUG("[Input][Controller] ID: ", static_cast<uint32_t>(e.GetController()), " Controller: \"", GetControllerName(static_cast<Controller>(e.GetController())), "\" Connected!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::OnControllerDisconnectEvent(ControllerDisconnectEvent &e)
{
	s_controllerStatuses[static_cast<uint8_t>(e.GetController())] = {};
	TP_DEBUG("[Input][Controller] ID: ", static_cast<uint32_t>(e.GetController()), " disconnected!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerInternal* TRAP::Input::AddInternalController(const std::string& name, const std::string& guid, const int32_t axisCount, const int32_t buttonCount, const int32_t dpadCount)
{
	uint8_t cID;
	for(cID = 0; cID <= static_cast<uint8_t>(Controller::Four); cID++)
	{
		if (!s_controllerStatuses[cID].Connected)
			break;
	}

	if (cID >= static_cast<uint8_t>(Controller::Four))
		return nullptr;

	ControllerInternal* js = &s_controllerInternal[cID];
	js->Name = name;
	js->guid = guid;
	js->Axes.resize(axisCount);
	js->Buttons.resize(buttonCount + dpadCount * 4);
	js->ButtonCount = buttonCount;
	js->DPads.resize(dpadCount);

	js->mapping = FindValidMapping(js);

	return js;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller DPad
void TRAP::Input::InternalInputControllerDPad(ControllerInternal* js, const int32_t dpad, const uint8_t value)
{
	const int32_t base = js->ButtonCount + dpad * 4;

	js->Buttons[base + 0] = (value & 0x01) ? true : false; //Up
	js->Buttons[base + 1] = (value & 0x02) ? true : false; //Right
	js->Buttons[base + 2] = (value & 0x04) ? true : false; //Down
	js->Buttons[base + 3] = (value & 0x08) ? true : false; //Left

	if (js->Buttons[base + 1] && js->Buttons[base + 0])
		js->DPads[dpad] = ControllerDPad::Right_Up;
	else if (js->Buttons[base + 1] && js->Buttons[base + 2])
		js->DPads[dpad] = ControllerDPad::Right_Down;
	else if (js->Buttons[base + 3] && js->Buttons[base + 0])
		js->DPads[dpad] = ControllerDPad::Left_Up;
	else if (js->Buttons[base + 3] && js->Buttons[base + 2])
		js->DPads[dpad] = ControllerDPad::Left_Down;
	else if (js->Buttons[base + 0])
		js->DPads[dpad] = ControllerDPad::Up;
	else if (js->Buttons[base + 1])
		js->DPads[dpad] = ControllerDPad::Right;
	else if (js->Buttons[base + 2])
		js->DPads[dpad] = ControllerDPad::Down;
	else if (js->Buttons[base + 3])
		js->DPads[dpad] = ControllerDPad::Left;
	else
		js->DPads[dpad] = ControllerDPad::Centered;	
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller axis
void TRAP::Input::InternalInputControllerAxis(ControllerInternal* js, const int32_t axis, const float value)
{
	js->Axes[axis] = value;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller button
void TRAP::Input::InternalInputControllerButton(ControllerInternal* js, const int32_t button, const bool pressed)
{
	js->Buttons[button] = pressed;
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

#ifdef TRAP_PLATFORM_WINDOWS
	UpdateControllerGUIDWindows(mapping.guid);
#endif
	
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
TRAP::Input::Mapping* TRAP::Input::FindValidMapping(const ControllerInternal* js)
{
	Mapping* mapping = FindMapping(js->guid);
	if(mapping)
	{
		uint8_t i;
		
		for(i = 0; i <= static_cast<uint8_t>(ControllerButton::DPad_Left); i++)
		{
			if(!IsValidElementForController(&mapping->Buttons[i], js))
			{
				TP_ERROR("[Input][Controller] Invalid button in Controller mapping: ", mapping->guid, " ", mapping->Name);
				return nullptr;
			}
		}
		
		for(i = 0; i <= static_cast<uint8_t>(ControllerAxis::Right_Trigger); i++)
		{
			if(!IsValidElementForController(&mapping->Axes[i], js))
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
bool TRAP::Input::IsValidElementForController(const MapElement* e, const ControllerInternal* js)
{
	if(e->Type == 3 && (e->Index >> 4) >= (js->DPads.size() + 1))
		return false;
	if(e->Type == 2 && e->Index >= (js->Buttons.size() + 1))
		return false;
	if(e->Type == 1 && e->Index >= (js->Axes.size() + 1))
		return false;
		
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMappedControllerButtonPressed(Controller controller, ControllerButton button)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if(s_controllerAPI == ControllerAPI::XInput)
	{
		const int32_t buttonXInput = ControllerButtonToXInput(button);
		if (buttonXInput == -1)
			return false;
		if (buttonXInput == 0)
		{
			TP_ERROR("[Input][Controller][XInput] Could not get button state!");
			TP_ERROR("[Input][Controller][XInput] Invalid Button!");
			return false;
		}

		XINPUT_STATE state{};
		const uint32_t result = XInputGetState(static_cast<DWORD>(controller), &state);
		if (result == ERROR_SUCCESS)
			return (state.Gamepad.wButtons & buttonXInput) != 0;

		TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while getting button status!");
		return false;
	}
#endif

	if (!PollController(controller, 2))
		return false;

	ControllerInternal* js = &s_controllerInternal[static_cast<uint8_t>(controller)];

	if (!js->mapping)
		return false;

	const MapElement* e = &js->mapping->Buttons[static_cast<uint8_t>(button)];
	if (e->Index < js->ButtonCount)
		if (e->Type == 2) //Button
			return js->Buttons[e->Index];
	if (e->Type == 1) //Axis
	{
		const float value = js->Axes[e->Index] * static_cast<float>(e->AxisScale) + static_cast<float>(e->AxisOffset);
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
		if (js->Buttons[e->Index >> 4])
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMappedControllerAxis(Controller controller, ControllerAxis axis)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if(s_controllerAPI == ControllerAPI::XInput)
	{
		XINPUT_STATE state{};
		const uint32_t result = XInputGetState(static_cast<DWORD>(controller), &state);
		if (result == ERROR_SUCCESS)
		{
			switch (axis)
			{
			case ControllerAxis::Left_X:
				return (static_cast<float>(state.Gamepad.sThumbLX) + 0.5f) / 32767.5f;

			case ControllerAxis::Left_Y:
				return -(static_cast<float>(state.Gamepad.sThumbLY) + 0.5f) / 32767.5f;

			case ControllerAxis::Right_X:
				return (static_cast<float>(state.Gamepad.sThumbRX) + 0.5f) / 32767.5f;

			case ControllerAxis::Right_Y:
				return -(static_cast<float>(state.Gamepad.sThumbRY) + 0.5f) / 32767.5f;

			case ControllerAxis::Left_Trigger:
				return static_cast<float>(state.Gamepad.bLeftTrigger) / 127.5f - 1.0f;

			case ControllerAxis::Right_Trigger:
				return static_cast<float>(state.Gamepad.bRightTrigger) / 127.5f - 1.0f;

			default:
				TP_ERROR("[Input][Controller][XInput] Could not get axis state!");
				return 0.0f;
			}
		}

		TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while getting axis state!");
		return 0.0f;
	}
#endif
	
	if(!PollController(controller, 1))
		return 0.0f;
		
	ControllerInternal* js = &s_controllerInternal[static_cast<uint8_t>(controller)];
		
	if(!js->mapping)
		return 0.0f;
		
	const MapElement* e = &js->mapping->Axes[static_cast<uint8_t>(axis)];
	if(e->Type == 1) //Axis
	{
		const float value = js->Axes[e->Index] * static_cast<float>(e->AxisScale) + static_cast<float>(e->AxisOffset);
		return Math::Min(Math::Max(value, -1.0f), 1.0f);
	}
	if(e->Type == 2) //Button
		return js->Buttons[e->Index] ? 1.0f : -1.0f;
	if(e->Type == 3) //DPad
	{
		if(js->Buttons[e->Index >>  4])
			return 1.0f;
		
		return -1.0f;
	}
		
	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetMappedControllerDPad(Controller controller, const uint32_t dpad)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if(s_controllerAPI == ControllerAPI::XInput)
	{
		if (dpad == 0)
		{
			std::vector<bool> buttons = GetAllControllerButtonsInternal(controller);
			const bool up = buttons[11];
			const bool right = buttons[12];
			const bool down = buttons[13];
			const bool left = buttons[14];

			if (right && up)
				return ControllerDPad::Right_Up;
			if (right && down)
				return ControllerDPad::Right_Down;
			if (left && up)
				return ControllerDPad::Left_Up;
			if (left && down)
				return ControllerDPad::Left_Down;
			if (up)
				return ControllerDPad::Up;
			if (right)
				return ControllerDPad::Right;
			if (down)
				return ControllerDPad::Down;
			if (left)
				return ControllerDPad::Left;
		}

		return ControllerDPad::Centered;
	}
#endif
	
	if(!PollController(controller, 3))
		return ControllerDPad::Centered;
		
	ControllerInternal* js = &s_controllerInternal[static_cast<uint8_t>(controller)];
		
	if(!js->mapping)
		return ControllerDPad::Centered;
		
	const MapElement* e = &js->mapping->Buttons[11 + (dpad * 4)];
	if(e->Type == 3)
		return js->DPads[e->Index >> 4];
		
	return ControllerDPad::Centered;
}
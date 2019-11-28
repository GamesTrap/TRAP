#include "TRAPPCH.h"
#include "Input.h"

#include "Application.h"
#include "Event/ControllerEvent.h"

//-------------------------------------------------------------------------------------------------------------------//

std::array<TRAP::Input::ControllerStatus, 4> TRAP::Input::s_controllerStatuses;
TRAP::Input::EventCallbackFn TRAP::Input::s_eventCallback{};
TRAP::Input::ControllerAPI TRAP::Input::s_controllerAPI = ControllerAPI::Unknown;
std::array<TRAP::Input::ControllerInternal, 4> TRAP::Input::s_controllerInternal{};
std::vector<TRAP::Input::Mapping> TRAP::Input::Mappings{};
uint32_t TRAP::Input::MappingCount = 0;

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
	s_controllerStatuses = {};
	s_controllerAPI = controllerAPI;

#ifdef TRAP_PLATFORM_WINDOWS
	if (s_controllerAPI == ControllerAPI::Unknown || s_controllerAPI == ControllerAPI::Linux)
		s_controllerAPI = ControllerAPI::XInput;

	InitController();
#endif
	//Linux is ignored because it only got 1 supported API for controllers
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key)
{
	const auto window = static_cast<GLFWwindow *>(Application::GetWindow()->GetNativeWindow());
	const auto state = glfwGetKey(window, static_cast<int32_t>(key));

	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	const auto window = static_cast<GLFWwindow *>(Application::GetWindow()->GetNativeWindow());
	const auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));

	return state == GLFW_PRESS;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsGamepadButtonPressed(Controller controller, const ControllerButton button)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return false;
	}

	if (!IsControllerGamepad(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not a Gamepad!");
		return false;
	}

#ifdef TRAP_PLATFORM_WINDOWS
	if (s_controllerAPI == ControllerAPI::XInput)
		return IsGamepadButtonPressedXInput(controller, button);
#endif
	
	return IsMappedControllerButtonPressed(controller, button);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	return glfwRawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerConnected(const Controller controller)
{
	PollController(controller, 0);
	
	return s_controllerStatuses[static_cast<int32_t>(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerGamepad(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return false;
	}
	
	if(s_controllerAPI == ControllerAPI::XInput)
		return true;
	
	return s_controllerInternal[static_cast<int32_t>(controller)].mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	const auto window = static_cast<GLFWwindow *>(Application::GetWindow()->GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	return {static_cast<float>(xPos), static_cast<float>(yPos)};
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
	if (glfwGetKeyName(static_cast<int32_t>(key), 0))
		return glfwGetKeyName(static_cast<int32_t>(key), 0);

	TP_ERROR("[Input] Couldn't get name of Key: ", static_cast<int32_t>(key), "!");
	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetControllerAxis(Controller controller, const ControllerAxis axis)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return 0.0f;
	}

#ifdef TRAP_PLATFORM_WINDOWS
	if(s_controllerAPI == ControllerAPI::XInput)
		return GetControllerAxisXInput(controller, axis);	
#endif
	
	if (s_controllerAPI != ControllerAPI::Unknown)
		return GetMappedControllerAxis(controller, axis);

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPad(Controller controller, const uint32_t dpad)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return ControllerDPad::Centered;
	}

#ifdef TRAP_PLATFORM_WINDOWS
	if(s_controllerAPI == ControllerAPI::XInput)
		return GetControllerDPadXInput(controller, dpad);
#endif
	
	if (s_controllerAPI != ControllerAPI::Unknown)
		return GetMappedControllerDPad(controller, dpad);

	return ControllerDPad::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerName(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return "";
	}

	return GetControllerNameInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Input::ControllerStatus &TRAP::Input::GetControllerStatus(Controller controller)
{
	return s_controllerStatuses[static_cast<uint32_t>(controller)];
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerBattery TRAP::Input::GetControllerBatteryStatus(Controller controller)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if (s_controllerAPI == ControllerAPI::XInput)
		UpdateControllerBatteryAndConnectionTypeXInput(controller);
#endif

	return s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerConnectionType TRAP::Input::GetControllerConnectionType(Controller controller)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if (s_controllerAPI == ControllerAPI::XInput)
		UpdateControllerBatteryAndConnectionTypeXInput(controller);
#endif

	return s_controllerStatuses[static_cast<uint32_t>(controller)].ConnectionType;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxes(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return {};
	}

	return GetAllControllerAxes(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtons(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return {};
	}

	return GetAllControllerButtonsInternal(controller);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPads(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return {};
	}

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
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return;
	}

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
	const char* c = map.c_str();
	
	while(*c)
	{
		if((*c >= '0' && *c <= '9') ||
		   (*c >= 'a' && *c <= 'f') ||
		   (*c >= 'A' && *c <= 'F'))
		{
			std::array<char, 1024> line{};
			
			const std::size_t length = std::strcspn(c, "\r\n");
			if(length < line.size())
			{
				Mapping mapping{};
				
				std::memcpy(line.data(), c, length);
				line[length] = '\0';
				
				if(ParseMapping(&mapping, line.data()))
				{
					Mapping* previous = FindMapping(mapping.guid.data());
					if(previous)
						*previous = mapping;
					else
					{
						MappingCount++;
						Mappings.resize(MappingCount);
						Mappings[MappingCount - 1] = mapping;
					}
				}
			}
			
			c += length;
		}
		else
		{
			c += std::strcspn(c, "\r\n");
			c += std::strspn(c, "\r\n");
		}
	}
	
	for(uint32_t jID = 0; jID <= static_cast<uint32_t>(Controller::Four); jID++)
	{
		ControllerInternal* js = &s_controllerInternal[jID];
		if(s_controllerStatuses[jID].Connected)
			js->mapping = FindValidMapping(js);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::OnEvent(Event &e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<ControllerConnectEvent>(TRAP_BIND_EVENT_FN(Input::OnControllerConnectEvent));
	dispatcher.Dispatch<ControllerDisconnectEvent>(TRAP_BIND_EVENT_FN(Input::OnControllerDisconnectEvent));
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::OnControllerConnectEvent(ControllerConnectEvent &e)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if(s_controllerAPI == ControllerAPI::XInput)
		UpdateControllerBatteryAndConnectionTypeXInput(e.GetController());
	else
#endif
	{
		s_controllerStatuses[static_cast<uint32_t>(e.GetController())].BatteryStatus = ControllerBattery::Unknown;
		s_controllerStatuses[static_cast<uint32_t>(e.GetController())].ConnectionType = ControllerConnectionType::Unknown;
	}

	s_controllerStatuses[static_cast<uint32_t>(e.GetController())].Connected = true;

	if (s_controllerStatuses[static_cast<uint32_t>(e.GetController())].Connected) //Connected
		TP_DEBUG("[Input][Controller] ID: ", static_cast<uint32_t>(e.GetController()), " Controller: \"", GetControllerName(static_cast<Controller>(e.GetController())), "\" Connected!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::OnControllerDisconnectEvent(ControllerDisconnectEvent &e)
{
	s_controllerStatuses[static_cast<uint32_t>(e.GetController())] = {};
	TP_DEBUG("[Input][Controller] ID: ", static_cast<uint32_t>(e.GetController()), " disconnected!");

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerInternal* TRAP::Input::AddInternalController(const std::string& name, const std::string& guid, const int32_t axisCount, const int32_t buttonCount, const int32_t dpadCount)
{
	int jID;
	for(jID = 0; jID <= static_cast<int32_t>(Controller::Four); jID++)
	{
		if (!s_controllerStatuses[jID].Connected)
			break;
	}

	if (jID >= static_cast<int32_t>(Controller::Four))
		return nullptr;

	ControllerInternal* js = &s_controllerInternal[jID];
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
void TRAP::Input::InternalInputControllerDPad(ControllerInternal* js, const int32_t dpad, const char value)
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
bool TRAP::Input::ParseMapping(Mapping* mapping, const char* str)
{
	const char* c = str;
	std::size_t i;
	struct Fields
	{
		const char* Name = nullptr;
		MapElement* Element = nullptr;
	};
	std::array<Fields, 22> fields =
	{
		{
			{"platform", nullptr},
			{"a", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::A)]},
			{"b", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::B)]},
			{"x", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::X)]},
			{"y", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Y)]},
			{"back", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Back)]},
			{"start", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Start)]},
			{"guide", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Guide)]},
			{"leftshoulder", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Left_Bumper)]},
			{"rightshoulder", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Right_Bumper)]},
			{"leftstick", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Left_Thumb)]},
			{"rightstick", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::Right_Thumb)]},
			{"dpup", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::DPad_Up)]},
			{"dpright", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::DPad_Right)]},
			{"dpdown", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::DPad_Down)]},
			{"dpleft", &mapping->Buttons[static_cast<uint32_t>(ControllerButton::DPad_Left)]},
			{"lefttrigger", &mapping->Axes[static_cast<uint32_t>(ControllerAxis::Left_Trigger)]},
			{"righttrigger", &mapping->Axes[static_cast<uint32_t>(ControllerAxis::Right_Trigger)]},
			{"leftx", &mapping->Axes[static_cast<uint32_t>(ControllerAxis::Left_X)]},
			{"lefty", &mapping->Axes[static_cast<uint32_t>(ControllerAxis::Left_Y)]},
			{"rightx", &mapping->Axes[static_cast<uint32_t>(ControllerAxis::Right_X)]},
			{"righty", &mapping->Axes[static_cast<uint32_t>(ControllerAxis::Right_Y)]}
		}
	};
	
	std::size_t length = std::strcspn(c, ",");
	if(length != 32 || c[length] != ',')
	{
		TP_ERROR("[Input][Controller] Invalid value!");
		return false;
	}
	
	std::memcpy(mapping->guid.data(), c, length);
	c += length + 1;
	
	length = std::strcspn(c, ",");
	if(length >= mapping->Name.size() || c[length] != ',')
	{
		TP_ERROR("[Input][Controller] Invalid value!");
		return false;
	}
	
	std::memcpy(mapping->Name.data(), c, length);
	c += length + 1;
	
	while(*c)
	{
		if(*c == '+' || *c == '-')
			return false;
			
		for(i = 0; i < fields.size(); i++)
		{
			length = std::strlen(fields[i].Name);
			if(std::strncmp(c, fields[i].Name, length) != 0 || c[length] != ':')
				continue;
				
			c += length + 1;
			
			if(fields[i].Element)
			{
				MapElement* e = fields[i].Element;
				int8_t minimum = -1;
				int8_t maximum = 1;
				
				if(*c == '+')
				{
					minimum = 0;
					c += 1;
				}
				else if(*c == '-')
				{
					maximum = 0;
					c += 1;
				}
				
				if(*c == 'a')
					e->Type = 1; //Axis
				else if(*c == 'b')
					e->Type = 2; //Button
				else if(*c == 'h')
					e->Type = 3; //DPad
				else
					break;
					
				if(e->Type == 3) //DPad
				{
					const uint64_t hat = std::strtoul(c + 1, const_cast<char**>(&c), 10);
					const uint64_t bit = std::strtoul(c + 1, const_cast<char**>(&c), 10);
					e->Index = static_cast<uint8_t>((hat << 4) | bit);
				}
				else
					e->Index = static_cast<uint8_t>(std::strtoul(c + 1, const_cast<char**>(&c), 10));
					
				if(e->Type == 1) //Axis
				{
					e->AxisScale = 2 / (maximum - minimum);
					e->AxisOffset = -(maximum + minimum);
					
					if(*c == '~')
					{
						e->AxisScale = -e->AxisScale;
						e->AxisOffset = -e->AxisOffset;
					}
				}
			}
			else
			{
				length = std::strlen(MappingName);
				if(std::strncmp(c, MappingName, length) != 0)
					return false;
			}
			
			break;
		}
		
		c += std::strcspn(c, ",");
		c += std::strspn(c, ",");
	}
	
	for(i = 0; i < 32; i++)
	{
		if(mapping->guid[i] >= 'A' && mapping->guid[i] <= 'F')
			mapping->guid[i] += 'a' - 'A';
	}
	
	UpdateControllerGUID(mapping->guid);
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Find a mapping based on controller GUID
TRAP::Input::Mapping* TRAP::Input::FindMapping(const char* guid)
{
	for(uint32_t i = 0; i < MappingCount; i++)
	{
		if(std::strcmp(Mappings[i].guid.data(), guid) == 0)
			return &Mappings[i];
	}
	
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Finds a mapping based on controller GUID and verifies element indices
TRAP::Input::Mapping* TRAP::Input::FindValidMapping(const ControllerInternal* js)
{
	Mapping* mapping = FindMapping(js->guid.data());
	if(mapping)
	{
		uint32_t i;
		
		for(i = 0; i <= static_cast<uint32_t>(ControllerButton::DPad_Left); i++)
		{
			if(!IsValidElementForController(&mapping->Buttons[i], js))
			{
				TP_ERROR("[Input][Controller] Invalid button in Controller mapping: ", mapping->guid.data(), " ", mapping->Name.data());
				return nullptr;
			}
		}
		
		for(i = 0; i <= static_cast<uint32_t>(ControllerAxis::Right_Trigger); i++)
		{
			if(!IsValidElementForController(&mapping->Axes[i], js))
			{
				TP_ERROR("[Input][Controller] Invalid axis in Controller mapping: ", mapping->guid.data(), " ", mapping->Name.data());
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
	if(!PollController(controller, 2))
		return false;
		
	ControllerInternal* js = &s_controllerInternal[static_cast<uint32_t>(controller)];
		
	if(!js->mapping)
		return false;
		
	const MapElement* e = &js->mapping->Buttons[static_cast<uint32_t>(button)];
	if(e->Index < js->ButtonCount)
		if(e->Type == 2) //Button
			return js->Buttons[e->Index];
	if(e->Type == 1) //Axis
	{
		const float value = js->Axes[e->Index] * static_cast<float>(e->AxisScale) + static_cast<float>(e->AxisOffset);
		if(e->AxisOffset < 0 || (e->AxisOffset == 0 && e->AxisScale > 0))
		{
			if(value >= 0.0f)
				return true;
		}
		else
		{
			if(value <= 0.0f)
				return false;
		}
	}
	else if(e->Type == 3) //DPad
	{
		if(js->Buttons[e->Index >>  4])
			return true;
	}
		
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetMappedControllerAxis(Controller controller, ControllerAxis axis)
{
	if(!PollController(controller, 1))
		return 0.0f;
		
	ControllerInternal* js = &s_controllerInternal[static_cast<uint32_t>(controller)];
		
	if(!js->mapping)
		return 0.0f;
		
	const MapElement* e = &js->mapping->Axes[static_cast<uint32_t>(axis)];
	if(e->Type == 1) //Axis
	{
		const float value = js->Axes[e->Index] * static_cast<float>(e->AxisScale) + static_cast<float>(e->AxisOffset);
		return Math::Min(TRAP::Math::Max(value, -1.0f), 1.0f);
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
	if(!PollController(controller, 3))
		return ControllerDPad::Centered;
		
	ControllerInternal* js = &s_controllerInternal[static_cast<uint32_t>(controller)];
		
	if(!js->mapping)
		return ControllerDPad::Centered;
		
	const MapElement* e = &js->mapping->Buttons[11 + (dpad * 4)];
	if(e->Type == 3)
		return js->DPads[e->Index >> 4];
		
	return ControllerDPad::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerGUID(std::array<char, 33>& guid)
{
#ifdef TRAP_PLATFORM_WINDOWS
	if(std::strcmp(guid.data() + 20, "504944564944") == 0)
	{
		std::array<char, 33> original = guid;
		sprintf_s(guid.data(), guid.size(), "03000000%.4s0000%.4s000000000000", original.data(), original.data() + 4);
	}
#endif
}
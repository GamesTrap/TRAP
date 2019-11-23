#include "TRAPPCH.h"
#include "Input/Input.h"

#include "Application.h"
#include "Event/ControllerEvent.h"

#ifdef TRAP_PLATFORM_WINDOWS

std::array<uint32_t, 4> TRAP::Input::s_lastXInputUpdate{};

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::InitControllerWindows()
{
	if (s_controllerAPI == ControllerAPI::XInput)
		InitControllerXInput();
	else
		TP_ERROR("[Input][Controller][DirectInput] Implementation is WIP! Please use XInput for now");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::InitControllerXInput()
{
	//TODO
	//Windows Window Event: WM_DEVICECHANGE
	//if DBT_DEVICEARRIVAL && (dbg && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
	//Create ControllerConnect Event
	//if DBT_DEVICEREMOVECOMPLETE && dbh && (dbh && dbh_devicetype == DBT_DEVTYP_DEVICEINTERFACE
	//Create ControllerDisconnect Event
	//These events should be captured by a hidden helper window that just listens for events
	//
	//TODO
	//Test if connecting/disconnecting controller works while window is focused
	//Test if connecting/disconnecting controller works while window isn't focused

	for (uint32_t i = 0; i < s_controllerStatuses.size(); i++)
	{
		UpdateControllerConnectionXInput(static_cast<Controller>(i));

		if (s_controllerStatuses[i].Connected)
		{
			ControllerConnectEvent event(static_cast<Controller>(i));
			s_eventCallback(event);

			UpdateControllerBatteryAndConnectionTypeXInput(static_cast<Controller>(i));
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerConnectionXInput(Controller controller)
{
	XINPUT_STATE state{};
	const uint32_t result = XInputGetState(static_cast<uint32_t>(controller), &state);
	if (result == ERROR_SUCCESS)
	{
		if (state.dwPacketNumber == s_lastXInputUpdate[static_cast<uint32_t>(controller)]) //If true nothing changed
			return;
		s_lastXInputUpdate[static_cast<uint32_t>(controller)] = state.dwPacketNumber;
		s_controllerStatuses[static_cast<uint32_t>(controller)].Connected = true;

		return;
	}
	if (result != ERROR_NOT_CONNECTED && result != ERROR_DEVICE_NOT_CONNECTED)
		TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, "!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerBatteryAndConnectionTypeXInput(Controller controller)
{
	XINPUT_BATTERY_INFORMATION battery{};
	const uint32_t result = XInputGetBatteryInformation(static_cast<uint32_t>(controller), BATTERY_DEVTYPE_GAMEPAD, &battery);
	if (result == ERROR_SUCCESS)
	{
		switch (battery.BatteryType)
		{
		case BATTERY_TYPE_WIRED:
			s_controllerStatuses[static_cast<uint32_t>(controller)].ConnectionType = ControllerConnectionType::Wired;
			break;

		case BATTERY_TYPE_ALKALINE:
		case BATTERY_TYPE_NIMH:
		case BATTERY_TYPE_UNKNOWN:
			s_controllerStatuses[static_cast<uint32_t>(controller)].ConnectionType = ControllerConnectionType::Wireless;
			break;

		default:
			TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while checking battery information!");
			s_controllerStatuses[static_cast<uint32_t>(controller)].ConnectionType = ControllerConnectionType::Unknown;
			break;
		}

		if (s_controllerStatuses[static_cast<uint32_t>(controller)].ConnectionType == ControllerConnectionType::Wireless)
		{
			switch (battery.BatteryLevel)
			{
			case BATTERY_LEVEL_EMPTY:
				s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus = ControllerBattery::Empty;
				break;

			case BATTERY_LEVEL_LOW:
				s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus = ControllerBattery::Low;
				break;

			case BATTERY_LEVEL_MEDIUM:
				s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus = ControllerBattery::Medium;
				break;

			case BATTERY_LEVEL_FULL:
				s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus = ControllerBattery::Full;
				break;

			default:
				TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while checking battery status!");
				s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus = ControllerBattery::Unknown;
				break;
			}
		}

		return;
	}
	TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, "!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibrationXInput(Controller controller, const float leftMotor, const float rightMotor)
{
	const uint16_t left = static_cast<uint16_t>(static_cast<float>(65535) * leftMotor);
	const uint16_t right = static_cast<uint16_t>(static_cast<float>(65535) * rightMotor);
	XINPUT_VIBRATION vibration{left, right};
	const uint32_t result = XInputSetState(static_cast<uint32_t>(controller), &vibration);
	if (result != ERROR_SUCCESS)
		TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while setting vibration!");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsGamepadButtonPressedXInput(Controller controller, const ControllerButton button)
{
	const int32_t buttonXInput = ControllerButtonToXInput(button);
	if(buttonXInput == -1)
		return false;
	if (buttonXInput == 0)
	{
		TP_ERROR("[Input][Controller][XInput] Could not get button state!");
		TP_ERROR("[Input][Controller][XInput] Invalid Button!");
		return false;
	}

	XINPUT_STATE state{};
	const uint32_t result = XInputGetState(static_cast<uint32_t>(controller), &state);
	if (result == ERROR_SUCCESS)
		return (state.Gamepad.wButtons & buttonXInput) != 0;

	TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while getting button status!");
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetControllerAxisXInput(Controller controller, const ControllerAxis axis)
{
	XINPUT_STATE state{};
	const uint32_t result = XInputGetState(static_cast<uint32_t>(controller), &state);
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerDPad TRAP::Input::GetControllerDPadXInput(const Controller controller, const uint32_t dpad)
{
	if(dpad == 0)
	{
		std::vector<bool> buttons = GetAllControllerButtonsXInput(controller);
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

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxesXInput(Controller controller)
{
	XINPUT_STATE state{};
	const uint32_t result = XInputGetState(static_cast<uint32_t>(controller), &state);
	if (result == ERROR_SUCCESS)
	{
		std::vector<float> axes(6, 0.0f);
		axes[0] = (static_cast<float>(state.Gamepad.sThumbLX) + 0.5f) / 32767.5f;
		axes[1] = -(static_cast<float>(state.Gamepad.sThumbLY) + 0.5f) / 32767.5f;
		axes[2] = (static_cast<float>(state.Gamepad.sThumbRX) + 0.5f) / 32767.5f;
		axes[3] = -(static_cast<float>(state.Gamepad.sThumbRY) + 0.5f) / 32767.5f;
		axes[4] = static_cast<float>(state.Gamepad.bLeftTrigger) / 127.5f - 1.0f;
		axes[5] = static_cast<float>(state.Gamepad.bRightTrigger) / 127.5f - 1.0f;

		return axes;
	}

	TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while getting axes states!");
	return std::vector<float>(6, 0.0f);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtonsXInput(Controller controller)
{
	XINPUT_STATE state{};
	const uint32_t result = XInputGetState(static_cast<uint32_t>(controller), &state);
	if (result == ERROR_SUCCESS)
	{
		std::vector<bool> buttons(15, false);
		buttons[0] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::A)) != 0;
		buttons[1] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::B)) != 0;
		buttons[2] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::X)) != 0;
		buttons[3] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Y)) != 0;
		buttons[4] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Left_Bumper)) != 0;
		buttons[5] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Right_Bumper)) != 0;
		buttons[6] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Back)) != 0;
		buttons[7] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Start)) != 0;
		buttons[8] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Guide)) != 0;
		buttons[9] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Left_Thumb)) != 0;
		buttons[10] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::Right_Thumb)) != 0;
		buttons[11] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::DPad_Up)) != 0;
		buttons[12] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::DPad_Right)) != 0;
		buttons[13] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::DPad_Down)) != 0;
		buttons[14] = (state.Gamepad.wButtons & ControllerButtonToXInput(ControllerButton::DPad_Left)) != 0;

		return buttons;
	}

	TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while getting button states!");
	return std::vector<bool>(15, false);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerDPad> TRAP::Input::GetAllControllerDPadsXInput(const Controller controller)
{
	std::vector<ControllerDPad> dpads;
	dpads.push_back(GetControllerDPadXInput(controller, 0));

	return dpads;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerNameXInput(Controller controller)
{
	XINPUT_CAPABILITIES caps{};
	const uint32_t result = XInputGetCapabilities(static_cast<uint32_t>(controller), 0, &caps);
	if (result == ERROR_SUCCESS)
	{
		switch (caps.SubType)
		{
		case XINPUT_DEVSUBTYPE_WHEEL:
			return "XInput Wheel";

		case XINPUT_DEVSUBTYPE_ARCADE_STICK:
			return "XInput Arcade Stick";

		case XINPUT_DEVSUBTYPE_FLIGHT_STICK:
			return "XInput Flight Stick";

		case XINPUT_DEVSUBTYPE_DANCE_PAD:
			return "XInput Dance Pad";

		case XINPUT_DEVSUBTYPE_GUITAR:
			return "XInput Guitar";

		case XINPUT_DEVSUBTYPE_DRUM_KIT:
			return "XInput Drum Kit";

		case XINPUT_DEVSUBTYPE_GAMEPAD:
			return "XInput Gamepad";

		default:
			return "Unknown XInput Device";
		}
	}

	TP_ERROR("[Input][Controller][XInput] ID: ", static_cast<uint32_t>(controller), " Error: ", result, " while getting controller name!");
	return "Unknown XInput Device";
}

//-------------------------------------------------------------------------------------------------------------------//

int32_t TRAP::Input::ControllerButtonToXInput(const ControllerButton button)
{
	switch (button)
	{
	case ControllerButton::A:
		//case ControllerButton::Cross:
		return 0x1000;

	case ControllerButton::B:
		//case ControllerButton::Circle:
		return 0x2000;

	case ControllerButton::X:
		//case ControllerButton::Square:
		return 0x4000;

	case ControllerButton::Y:
		//case ControllerButton::Triangle:
		return 0x8000;

	case ControllerButton::Left_Bumper:
		return 0x0100;

	case ControllerButton::Right_Bumper:
		return 0x0200;

	case ControllerButton::Back:
		return 0x0020;

	case ControllerButton::Start:
		return 0x0010;

	case ControllerButton::Guide:
		return -1;

	case ControllerButton::Left_Thumb:
		return 0x0040;

	case ControllerButton::Right_Thumb:
		return 0x080;

	case ControllerButton::DPad_Up:
		return 0x0001;

	case ControllerButton::DPad_Right:
		return 0x0008;

	case ControllerButton::DPad_Down:
		return 0x0002;

	case ControllerButton::DPad_Left:
		return 0x0004;

	default:
		return 0;
	}
}

#endif
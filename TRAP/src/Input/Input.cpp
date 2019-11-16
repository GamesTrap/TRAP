#include "TRAPPCH.h"
#include "Input.h"

#include "Application.h"
#include "Event/ControllerEvent.h"

//-------------------------------------------------------------------------------------------------------------------//

std::array<TRAP::Input::ControllerStatus, 4> TRAP::Input::s_controllerStatuses;
TRAP::Input::EventCallbackFn TRAP::Input::s_eventCallback{};

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerAPI TRAP::Input::GetControllerAPI()
{
#ifdef TRAP_PLATFORM_WINDOWS
	return s_controllerAPI;
#endif

	return ControllerAPI::XInput;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerAPI(const ControllerAPI controllerAPI)
{
#ifdef TRAP_PLATFORM_WINDOWS
	s_controllerStatuses = {};
	s_controllerAPI = controllerAPI;
	InitControllerWindows();
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init(const ControllerAPI controllerAPI)
{
#ifdef TRAP_PLATFORM_WINDOWS
	s_controllerAPI = controllerAPI;
	InitControllerWindows();
#elif defined(TRAP_PLATFORM_LINUX)
	//Assuming GLFW is already initialized
	s_controllerStatuses[static_cast<int32_t>(Controller::One)] =
		{
			static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::One))),
			static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::One)))};
	s_controllerStatuses[static_cast<int32_t>(Controller::Two)] =
		{
			static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::Two))),
			static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::Two)))};
	s_controllerStatuses[static_cast<int32_t>(Controller::Three)] =
		{
			static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::Three))),
			static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::Three)))};
	s_controllerStatuses[static_cast<int32_t>(Controller::Four)] =
		{
			static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::Four))),
			static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::Four))),
		};

	for (uint32_t i = 0; i < s_controllerStatuses.size(); i++)
	{
		if (s_controllerStatuses[i].Connected && !s_controllerStatuses[i].IsGamepad) //Connected and not a Gamepad
			TP_DEBUG("[Input][Controller] ID: ", i, " Controller: \"", glfwGetJoystickName(i), "\" Connected!");
		else if (s_controllerStatuses[i].Connected && s_controllerStatuses[i].IsGamepad) //Connected and a Gamepad
			TP_DEBUG("[Input][Gamepad] ID: ", i, " Gamepad: \"", glfwGetGamepadName(i), "\" Connected!");
	}

	glfwSetJoystickCallback(JoystickCallback);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Shutdown()
{
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

bool TRAP::Input::IsGamepadButtonPressed(Controller controller, const GamepadButton button)
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
	return IsGamepadButtonPressedXInput(controller, button);
#elif defined(TRAP_PLATFORM_LINUX)
	GLFWgamepadstate state{};
	if (glfwGetGamepadState(static_cast<int32_t>(controller), &state))
		return state.buttons[static_cast<uint32_t>(button)];
#else
	return false;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	return glfwRawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsControllerConnected(const Controller controller)
{
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

	return s_controllerStatuses[static_cast<int32_t>(controller)].IsGamepad;
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
	return GetControllerAxisXInput(controller, axis);
#elif defined(TRAP_PLATFORM_LINUX)
	GLFWgamepadstate state{};
	if (glfwGetGamepadState(static_cast<int32_t>(controller), &state))
		return state.axes[static_cast<uint32_t>(axis)];
#else
	return 0.0f;
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerHat TRAP::Input::GetControllerHat(Controller controller, const uint32_t hat)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return ControllerHat::Centered;
	}

	//TODO Needed at all?
	uint32_t hatCount = 0;
	const unsigned char *hats = glfwGetJoystickHats(static_cast<int32_t>(controller), reinterpret_cast<int32_t *>(&hatCount));
	if (hats && hat < hatCount)
		return static_cast<ControllerHat>(hats[hat]);

	return ControllerHat::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerName(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return "";
	}

#ifdef TRAP_PLATFORM_WINDOWS
	return GetControllerNameXInput(controller);
#elif defined(TRAP_PLATFORM_LINUX)
	if (IsControllerGamepad(controller))
		return glfwGetGamepadName(static_cast<int32_t>(controller));

	return glfwGetJoystickName(static_cast<int32_t>(controller));
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Input::ControllerStatus &TRAP::Input::GetControllerStatus(Controller controller)
{
	return s_controllerStatuses[static_cast<uint32_t>(controller)];
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxes(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return {};
	}

#ifdef TRAP_PLATFORM_WINDOWS
	return GetAllControllerAxesXInput(controller);
#elif defined(TRAP_PLATFORM_LINUX)
	uint32_t axisCount = 0;
	const float *axes = glfwGetJoystickAxes(static_cast<int32_t>(controller), reinterpret_cast<int32_t *>(&axisCount));
	if (axes)
		return std::vector<float>(axes, axes + axisCount);
#else
	return {};
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtons(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return {};
	}

	if (!IsControllerGamepad(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not a Gamepad!");
		return {};
	}

#ifdef TRAP_PLATFORM_WINDOWS
	return GetAllControllerButtonsXInput(controller);
#elif defined(TRAP_PLATFORM_LINUX)
	uint32_t buttonCount = 0;
	const unsigned char *buttons = glfwGetJoystickButtons(static_cast<int32_t>(controller), reinterpret_cast<int32_t *>(&buttonCount));
	if (buttons)
		return std::vector<bool>(buttons, buttons + buttonCount);
#else
	return {};
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerHat> TRAP::Input::GetAllControllerHats(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return {};
	}

	uint32_t hatCount = 0;
	const unsigned char *hats = glfwGetJoystickHats(static_cast<int32_t>(controller), reinterpret_cast<int32_t *>(&hatCount));
	if (hats)
		return std::vector<ControllerHat>(reinterpret_cast<const ControllerHat *>(hats), reinterpret_cast<const ControllerHat *>(hats) + hatCount);

	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

const std::array<TRAP::Input::ControllerStatus, 4> &TRAP::Input::GetAllControllerStatuses()
{
	return s_controllerStatuses;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerBattery TRAP::Input::GetControllerBatteryStatus(Controller controller)
{
	return s_controllerStatuses[static_cast<uint32_t>(controller)].BatteryStatus;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerConnectionType TRAP::Input::GetControllerConnectionType(Controller controller)
{
	return s_controllerStatuses[static_cast<uint32_t>(controller)].ConnectionType;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetEventCallback(const EventCallbackFn &func)
{
	s_eventCallback = func;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibration(Controller controller, const float leftMotor, const float rightMotor)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not connected!");
		return;
	}

#ifdef TRAP_PLATFORM_WINDOWS
	SetControllerVibrationXInput(controller, leftMotor, rightMotor);
#elif defined(TRAP_PLATFORM_LINUX)
#endif
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
	UpdateControllerBatteryAndConnectionTypeXInput(e.GetController());
	s_controllerStatuses[static_cast<uint32_t>(e.GetController())].Connected = true;
	s_controllerStatuses[static_cast<uint32_t>(e.GetController())].IsGamepad = true;
	TP_DEBUG("[Input][Controller] ID: ", static_cast<int32_t>(e.GetController()), " connected!");
#elif defined(TRAP_PLATFORM_LINUX)
#endif

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

void TRAP::Input::JoystickCallback(const int32_t joystick, const int32_t event)
{
	if (joystick <= static_cast<int32_t>(Controller::Four))
	{
		if (event == GLFW_CONNECTED)
		{
			if (!glfwJoystickIsGamepad(joystick))
				TP_DEBUG("[Input][Controller] ID: ", joystick, " Controller: \"", glfwGetJoystickName(joystick), "\" connected!");
			else
				TP_DEBUG("[Input][Gamepad] ID: ", joystick, " Gamepad: \"", glfwGetGamepadName(joystick), "\" connected!");

			s_controllerStatuses[joystick] = {true, static_cast<bool>(glfwJoystickIsGamepad(joystick))};
		}
		else
		{
			if (!s_controllerStatuses[joystick].IsGamepad)
				TP_DEBUG("[Input][Controller] ID: ", joystick, " disconnected!");
			else
				TP_DEBUG("[Input][Gamepad] ID: ", joystick, " disconnected!");

			s_controllerStatuses[joystick] = {false, false};
		}
	}
}
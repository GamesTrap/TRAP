#include "TRAPPCH.h"
#include "Input.h"

#include <GLFW/glfw3.h>

#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerStatus> TRAP::Input::s_controllerStatuses;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init()
{
	s_controllerStatuses.resize(4); //Only 4 Controllers are supported!
	
	//Assuming GLFW is already initialized
	s_controllerStatuses[static_cast<int32_t>(Controller::One)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::One))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::One)))
	};
	s_controllerStatuses[static_cast<int32_t>(Controller::Two)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::Two))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::Two)))
	};
	s_controllerStatuses[static_cast<int32_t>(Controller::Three)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::Three))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::Three)))
	};
	s_controllerStatuses[static_cast<int32_t>(Controller::Four)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Controller::Four))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Controller::Four))),
	};

	for(uint32_t i = 0; i < s_controllerStatuses.size(); i++)
	{
		if (s_controllerStatuses[i].Connected && !s_controllerStatuses[i].IsGamepad) //Connected and not a Gamepad
			TP_DEBUG("[Input][Controller] ID: ", i, " Controller: \"", glfwGetJoystickName(i), "\" Connected!");
		else if (s_controllerStatuses[i].Connected && s_controllerStatuses[i].IsGamepad) //Connected and a Gamepad
			TP_DEBUG("[Input][Gamepad] ID: ", i, " Gamepad: \"", glfwGetGamepadName(i), "\" Connected!");
	}
	
	glfwSetJoystickCallback(JoystickCallback);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key key)
{
	const auto window = static_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow());
	const auto state = glfwGetKey(window, static_cast<int32_t>(key));

	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	const auto window = static_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow());
	const auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));

	return state == GLFW_PRESS;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsGamepadButtonPressed(Controller controller, GamepadButton button)
{
	if(!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return false;
	}

	if(!IsControllerGamepad(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not a Gamepad!");
		return false;
	}

	GLFWgamepadstate state{};
	if (glfwGetGamepadState(static_cast<int32_t>(controller), &state))
		return state.buttons[static_cast<uint32_t>(button)];		

	return false;
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
	if(!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return false;
	}

	return s_controllerStatuses[static_cast<int32_t>(controller)].IsGamepad;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec2 TRAP::Input::GetMousePosition()
{
	const auto window = static_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

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
	if(glfwGetKeyName(static_cast<int32_t>(key), 0))
		return glfwGetKeyName(static_cast<int32_t>(key), 0);
	
	TP_ERROR("[Input] Couldn't get name of Key: ", static_cast<int32_t>(key), "!");
	return "";
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Input::GetControllerAxis(Controller controller, const ControllerAxis axis)
{
	if(!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return 0.0f;
	}

	GLFWgamepadstate state{};
	if (glfwGetGamepadState(static_cast<int32_t>(controller), &state))
		return state.axes[static_cast<uint32_t>(axis)];

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerHat TRAP::Input::GetControllerHat(Controller controller, const uint32_t hat)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return ControllerHat::Centered;
	}

	uint32_t hatCount = 0;
	const unsigned char* hats = glfwGetJoystickHats(static_cast<int32_t>(controller), reinterpret_cast<int32_t*>(&hatCount));
	if (hats && hat < hatCount)
		return static_cast<ControllerHat>(hats[hat]);

	return ControllerHat::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetControllerName(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return "";
	}

	if(IsControllerGamepad(controller))
		return glfwGetGamepadName(static_cast<int32_t>(controller));

	return glfwGetJoystickName(static_cast<int32_t>(controller));
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Input::ControllerStatus& TRAP::Input::GetControllerStatus(Controller controller)
{
	return s_controllerStatuses[static_cast<uint32_t>(controller)];
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<float> TRAP::Input::GetAllControllerAxes(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return {};
	}

	uint32_t axisCount = 0;
	const float* axes = glfwGetJoystickAxes(static_cast<int32_t>(controller), reinterpret_cast<int32_t*>(&axisCount));
	if(axes)
		return std::vector<float>(axes, axes + axisCount);

	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<bool> TRAP::Input::GetAllControllerButtons(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return {};
	}

	if (!IsControllerGamepad(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not a Gamepad!");
		return {};
	}

	uint32_t buttonCount = 0;
	const unsigned char* buttons = glfwGetJoystickButtons(static_cast<int32_t>(controller), reinterpret_cast<int32_t*>(&buttonCount));
	if (buttons)
		return std::vector<bool>(buttons, buttons + buttonCount);

	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::ControllerHat> TRAP::Input::GetAllControllerHats(Controller controller)
{
	if (!IsControllerConnected(controller))
	{
		TP_WARN("[Input][Controller] ID: ", static_cast<int32_t>(controller), " is not Connected!");
		return {};
	}

	uint32_t hatCount = 0;
	const unsigned char* hats = glfwGetJoystickHats(static_cast<int32_t>(controller), reinterpret_cast<int32_t*>(&hatCount));
	if(hats)
		return std::vector<ControllerHat>(reinterpret_cast<const ControllerHat*>(hats), reinterpret_cast<const ControllerHat*>(hats) + hatCount);

	return {};
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Input::ControllerStatus>& TRAP::Input::GetAllControllerStatuses()
{
	return s_controllerStatuses;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::JoystickCallback(const int32_t joystick, const int32_t event)
{
	if(joystick <= static_cast<int32_t>(Controller::Four))
	{
		if(event == GLFW_CONNECTED)
		{
			if (!glfwJoystickIsGamepad(joystick))
				TP_DEBUG("[Input][Controller] ID: ", joystick, " Controller: \"", glfwGetJoystickName(joystick), "\" Connected!");
			else
				TP_DEBUG("[Input][Gamepad] ID: ", joystick, " Gamepad: \"", glfwGetGamepadName(joystick), "\" Connected!");

			s_controllerStatuses[joystick] = { true, static_cast<bool>(glfwJoystickIsGamepad(joystick)) };
		}
		else
		{
			if (!s_controllerStatuses[joystick].IsGamepad)
				TP_DEBUG("[Input][Controller] ID: ", joystick, " Disconnected!");
			else
				TP_DEBUG("[Input][Gamepad] ID: ", joystick, " Disconnected!");

			s_controllerStatuses[joystick] = { false, false };
		}
	}
}
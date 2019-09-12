#include "TRAPPCH.h"
#include "Input.h"

#include <GLFW/glfw3.h>

#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

std::vector<TRAP::Input::Controller> TRAP::Input::m_joysticks;

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::Init()
{
	m_joysticks.resize(4); //Only 4 Controllers are supported!
	
	//Assuming GLFW is already initialized
	m_joysticks[static_cast<int32_t>(Joystick::ONE)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Joystick::ONE))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Joystick::ONE)))
	};
	m_joysticks[static_cast<int32_t>(Joystick::TWO)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Joystick::TWO))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Joystick::TWO)))
	};
	m_joysticks[static_cast<int32_t>(Joystick::THREE)] = 
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Joystick::THREE))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Joystick::THREE)))
	};
	m_joysticks[static_cast<int32_t>(Joystick::FOUR)] =
	{
		static_cast<bool>(glfwJoystickPresent(static_cast<int32_t>(Joystick::FOUR))),
		static_cast<bool>(glfwJoystickIsGamepad(static_cast<int32_t>(Joystick::FOUR))),
	};

	for(uint32_t i = 0; i < m_joysticks.size(); i++)
	{
		if (m_joysticks[i].Connected && !m_joysticks[i].IsGamepad) //Connected and not a Gamepad
			TP_DEBUG("[Input][Joystick] ID: ", i, " Joystick: \"", glfwGetJoystickName(i), "\" Connected!");
		else if (m_joysticks[i].Connected && m_joysticks[i].IsGamepad) //Connected and a Gamepad
			TP_DEBUG("[Input][Gamepad] ID: ", i, " Gamepad: \"", glfwGetGamepadName(i), "\" Connected!");
	}
	
	glfwSetJoystickCallback(JoystickCallback);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key keycode)
{
	const auto window = static_cast<GLFWwindow*>(Application::GetWindow()->GetNativeWindow());
	const auto state = glfwGetKey(window, static_cast<int32_t>(keycode));

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

bool TRAP::Input::IsGamepadButtonPressed(Joystick joystick, GamepadButton button)
{
	if(!IsJoystickConnected(joystick))
	{
		TP_WARN("[Input][Joystick] ID: ", static_cast<int32_t>(joystick), " is not Connected!");
		return false;
	}

	if(!IsJoystickGamepad(joystick))
	{
		TP_WARN("[Input][Joystick] ID: ", static_cast<int32_t>(joystick), " is not a Gamepad!");
		return false;
	}

	GLFWgamepadstate state{};
	if (glfwGetGamepadState(static_cast<int32_t>(joystick), &state))
		return state.buttons[static_cast<uint32_t>(button)];		

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	return glfwRawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsJoystickConnected(const Joystick joystick)
{
	return m_joysticks[static_cast<int32_t>(joystick)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsJoystickGamepad(Joystick joystick)
{
	if(!IsJoystickConnected(joystick))
	{
		TP_WARN("[Input][Joystick] ID: ", static_cast<int32_t>(joystick), " is not Connected!");
		return false;
	}

	return m_joysticks[static_cast<int32_t>(joystick)].IsGamepad;
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

float TRAP::Input::GetJoystickAxis(Joystick joystick, const JoystickAxis axis)
{
	if(!IsJoystickConnected(joystick))
	{
		TP_WARN("[Input][Joystick] ID: ", static_cast<int32_t>(joystick), " is not Connected!");
		return 0.0f;
	}

	GLFWgamepadstate state{};
	if (glfwGetGamepadState(static_cast<int32_t>(joystick), &state))
		return state.axes[static_cast<uint32_t>(axis)];

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetJoystickName(Joystick joystick)
{
	if (!IsJoystickConnected(joystick))
	{
		TP_WARN("[Input][Joystick] ID: ", static_cast<int32_t>(joystick), " is not Connected!");
		return "";
	}

	if(IsJoystickGamepad(joystick))
		return glfwGetGamepadName(static_cast<int32_t>(joystick));

	return glfwGetJoystickName(static_cast<int32_t>(joystick));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::JoystickCallback(const int32_t joystick, const int32_t event)
{
	if(joystick <= static_cast<int32_t>(Joystick::FOUR))
	{
		if(event == GLFW_CONNECTED)
		{
			if (!glfwJoystickIsGamepad(joystick))
				TP_DEBUG("[Input][Joystick] ID: ", joystick, " Joystick: \"", glfwGetJoystickName(joystick), "\" Connected!");
			else
				TP_DEBUG("[Input][Gamepad] ID: ", joystick, " Gamepad: \"", glfwGetGamepadName(joystick), "\" Connected!");

			m_joysticks[joystick] = { true, static_cast<bool>(glfwJoystickIsGamepad(joystick)) };
		}
		else
		{
			if (!m_joysticks[joystick].IsGamepad)
				TP_DEBUG("[Input][Joystick] ID: ", joystick, " Disconnected!");
			else
				TP_DEBUG("[Input][Gamepad] ID: ", joystick, " Disconnected!");

			m_joysticks[joystick] = { false, false };
		}
	}
}
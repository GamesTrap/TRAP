#include "TRAPPCH.h"
#include "Input.h"

#include <GLFW/glfw3.h>

#include "Application.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const Key keycode)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
	const auto state = glfwGetKey(window, static_cast<int32_t>(keycode));

	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const MouseButton button)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
	const auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));

	return state == GLFW_PRESS;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsRawMouseInputSupported()
{
	return glfwRawMouseMotionSupported();
}

//-------------------------------------------------------------------------------------------------------------------//

std::pair<float, float> TRAP::Input::GetMousePosition()
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
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

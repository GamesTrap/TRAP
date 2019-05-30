#include "TRAPPCH.h"
#include "Input.h"

#include <GLFW/glfw3.h>

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsKeyPressed(const int keycode)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
	const auto state = glfwGetKey(window, keycode);

	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::IsMouseButtonPressed(const int button)
{
	const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
	const auto state = glfwGetMouseButton(window, button);

	return state == GLFW_PRESS;
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
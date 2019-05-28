#include "TRAPPCH.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace TRAP
{
	bool Input::IsKeyPressed(const int keycode)
	{
		const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
		const auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(const int button)
	{
		const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
		const auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		const auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { static_cast<float>(xPos), static_cast<float>(yPos) };
	}

	float Input::GetMouseX()
	{
		auto[x, y] = GetMousePosition();

		return x;
	}

	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePosition();

		return y;
	}
}

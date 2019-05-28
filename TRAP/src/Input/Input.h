#pragma once

namespace TRAP
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);

		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	private:
		static const float m_lastTime;
	};
}

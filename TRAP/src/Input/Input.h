#ifndef _TRAP_INPUT_H_
#define _TRAP_INPUT_H_

#include "Utils/Singleton.h"

namespace TRAP
{
	class Input final : public Singleton
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

#endif /*_TRAP_INPUT_H_*/
#ifndef _TRAP_INPUT_H_
#define _TRAP_INPUT_H_

#include "Utils/Singleton.h"

namespace TRAP
{
	
	class Input final : public Singleton
	{
	public:
		static bool IsKeyPressed(int32_t keycode);
		static bool IsMouseButtonPressed(int32_t button);
		static bool IsRawMouseInputSupported();
		
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	private:
		static const float m_lastTime;
	};
}

#endif /*_TRAP_INPUT_H_*/
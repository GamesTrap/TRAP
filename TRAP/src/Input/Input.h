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
		
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

		static void SetCursorMode(const CursorMode& mode);
		static CursorMode GetCursorMode();

	private:
		static const float m_lastTime;
		static CursorMode m_currentCursorMode;
	};
}

#endif /*_TRAP_INPUT_H_*/
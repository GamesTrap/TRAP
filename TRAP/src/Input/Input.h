#ifndef _TRAP_INPUT_H_
#define _TRAP_INPUT_H_

#include "Utils/Singleton.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace TRAP
{
	class Input final : public Singleton
	{
	public:
		static bool IsKeyPressed(Key keycode);
		static bool IsMouseButtonPressed(MouseButton button);
		static bool IsRawMouseInputSupported();
		
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		static std::string GetKeyName(Key key);

	private:
		static const float m_lastTime;
	};
}

#endif /*_TRAP_INPUT_H_*/
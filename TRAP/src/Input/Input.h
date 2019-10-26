#ifndef _TRAP_INPUT_H_
#define _TRAP_INPUT_H_

#include "Utils/Singleton.h"
#include "Maths/Math.h"

namespace TRAP
{
	class Input final : public Singleton
	{
	public:
		enum class Key
		{
			Space          = 32,
			Apostrophe     = 39,
			Comma          = 44,
			Minus          = 45,
			Period         = 46,
			Slash          = 47,
			Zero           = 48,
			One            = 49,
			Two            = 50,
			Three          = 51,
			Four           = 52,
			Five           = 53,
			Six            = 54,
			Seven          = 55,
			Eight          = 56,
			Nine           = 57,
			Semicolon      = 59,
			Equal          = 61,
			A              = 65,
			B              = 66,
			C              = 67,
			D              = 68,
			E              = 69,
			F              = 70,
			G              = 71,
			H              = 72,
			I              = 73,
			J              = 74,
			K              = 75,
			L              = 76,
			M              = 77,
			N              = 78,
			O              = 79,
			P              = 80,
			Q              = 81,
			R              = 82,
			S              = 83,
			T              = 84,
			U              = 85,
			V              = 86,
			W              = 87,
			X              = 88,
			Y              = 89,
			Z              = 90,
			Left_Bracket   = 91,
			Backslash      = 92,
			Right_Bracket  = 93,
			Grave_Accent   = 96,
			World_1        = 161,
			World_2        = 162,

			/* Function keys */
			Escape         = 256,
			Enter          = 257,
			Tab            = 258,
			Backspace      = 259,
			Insert         = 260,
			Delete         = 261,
			Right          = 262,
			Left           = 263,
			Down           = 264,
			Up             = 265,
			Page_Up        = 266,
			Page_Down      = 267,
			Home           = 268,
			End            = 269,
			Caps_Lock      = 280,
			Scroll_Lock    = 281,
			NUM_Lock       = 282,
			Print_Screen   = 283,
			Pause          = 284,
			F1             = 290,
			F2             = 291,
			F3             = 292,
			F4             = 293,
			F5             = 294,
			F6             = 295,
			F7             = 296,
			F8             = 297,
			F9             = 298,
			F10            = 299,
			F11            = 300,
			F12            = 301,
			F13            = 302,
			F14            = 303,
			F15            = 304,
			F16            = 305,
			F17            = 306,
			F18            = 307,
			F19            = 308,
			F20            = 309,
			F21            = 310,
			F22            = 311,
			F23            = 312,
			F24            = 313,
			F25            = 314,
			KP_0           = 320,
			KP_1           = 321,
			KP_2           = 322,
			KP_3           = 323,
			KP_4           = 324,
			KP_5           = 325,
			KP_6           = 326,
			KP_7           = 327,
			KP_8           = 328,
			KP_9           = 329,
			KP_Decimal     = 330,
			KP_Divide      = 331,
			KP_Multiply    = 332,
			KP_Subtract    = 333,
			KP_Add         = 334,
			KP_Enter       = 335,
			KP_Equal       = 336,
			Left_Shift     = 340,
			Left_Control   = 341,
			Left_ALT       = 342,
			Left_Super     = 343,
			Right_Shift    = 344,
			Right_Control  = 345,
			Right_ALT      = 346,
			Right_Super    = 347,
			Menu           = 348
		};
		
		enum class MouseButton
		{
			One    = 0,
			Two    = 1,
			Three  = 2,
			Four   = 3,
			Five   = 4,
			Six    = 5,
			Seven  = 6,
			Eight  = 7,
			Left   = One,
			Right  = Two,
			Middle = Three
		};
		
		enum class Controller
		{
			One   = 0,
			Two   = 1,
			Three = 2,
			Four  = 3
		};

		enum class ControllerAxis
		{
			Left_X        = 0,
			Left_Y        = 1,
			Right_X       = 2,
			Right_Y       = 3,
			Left_Trigger  = 4,
			Right_Trigger = 5
		};
		
		enum class GamepadButton
		{
			A            = 0,
			B            = 1,
			X            = 2,
			Y            = 3,
			Left_Bumper  = 4,
			Right_Bumper = 5,
			Back         = 6,
			Start        = 7,
			Guide        = 8,
			Left_Thumb   = 9,
			Right_Thumb  = 10,
			DPad_Up      = 11,
			DPad_Right   = 12,
			DPad_Down    = 13,
			DPad_Left    = 14,
			Cross        = A,
			Circle       = B,
			Square       = X,
			Triangle     = Y
		};

		enum class ControllerHat
		{
			Centered   = 0,
			Up         = 1,
			Right      = 2,
			Down       = 4,
			Left       = 8,
			Right_Up   = Right | Up,
			Right_Down = Right | Down,
			Left_Up    = Left | Up,
			Left_Down  = Left | Down
		};

		struct ControllerStatus
		{
			bool Connected = false;
			bool IsGamepad = false;
		};
		
		static void Init();
		
		static bool IsKeyPressed(Key key);
		static bool IsMouseButtonPressed(MouseButton button);
		static bool IsGamepadButtonPressed(Controller controller, GamepadButton button);
		static bool IsRawMouseInputSupported();
		static bool IsControllerConnected(Controller controller);
		static bool IsControllerGamepad(Controller controller);

		static Math::Vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		static std::string GetKeyName(Key key);
		static float GetControllerAxis(Controller controller, ControllerAxis axis);
		static ControllerHat GetControllerHat(Controller controller, uint32_t hat);
		static std::string GetControllerName(Controller controller);
		static const ControllerStatus& GetControllerStatus(Controller controller);
		static std::vector<float> GetAllControllerAxes(Controller controller);
		static std::vector<bool> GetAllControllerButtons(Controller controller);
		static std::vector<ControllerHat> GetAllControllerHats(Controller controller);
		static const std::array<ControllerStatus, 4>& GetAllControllerStatuses();
		
	private:
		static void JoystickCallback(int32_t joystick, int32_t event);
		static std::array<ControllerStatus, 4> s_controllerStatuses;
	};
}

#endif /*_TRAP_INPUT_H_*/
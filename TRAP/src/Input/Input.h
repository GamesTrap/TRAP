/*
Copyright (c) 2002-2006 Marcus Geelnard

Copyright (c) 2006-2019 Camilla Loewy

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would
   be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.

Modified by: Jan "GamesTrap" Schuerkamp
The above license only applies to some of the Controller specific parts of this file!
*/

#ifndef TRAP_INPUT_H
#define TRAP_INPUT_H

#ifndef TRAP_HEADLESS_MODE

#include "Core/PlatformDetection.h" //Needed for OS dependent includes

#include "Core/Types.h"
#include "Utils/NumericCasts.h"

#include <functional>
#include <array>
#include <string_view>
#include <regex>
#include <optional>

#ifdef TRAP_PLATFORM_LINUX
#include "Utils/Linux.h"
#endif /*TRAP_PLATFORM_LINUX*/

#include "Events/Event.h"

namespace TRAP
{
	class Application;

	namespace INTERNAL
	{
		class WindowingAPI;
	}

	class Window;

	/// @remark @headless This class is not available in headless mode.
	class Input final
	{
	public:
		/// @brief Constructor.
		consteval Input() = delete;
		/// @brief Destructor.
		constexpr ~Input() = default;

		/// @brief Copy constructor.
		consteval Input(const Input&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Input& operator=(const Input&) noexcept = delete;
		/// @brief Move constructor.
		consteval Input(Input&&) noexcept = delete;
		/// @brief Move assignment operator.
		consteval Input& operator=(Input&&) noexcept = delete;

		/// @brief Keyboard keys.
		enum class Key
		{
			Unknown = -1,

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
			Num_Lock       = 282,
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

		/// @brief Key state.
		enum class KeyState : u32
		{
			Released,
			Pressed,
			Repeat
		};

		/// @brief Mouse buttons.
		enum class MouseButton : u32
		{
			One = 0,
			Two = 1,
			Three = 2,
			Four = 3,
			Five = 4,
			Six = 5,
			Seven = 6,
			Eight = 7,
			Left = One,
			Right = Two,
			Middle = Three
		};

		/// @brief Controllers
		enum class Controller : u32
		{
			One      = 0,
			Two      = 1,
			Three    = 2,
			Four     = 3,
			Five     = 4,
			Six      = 5,
			Seven    = 6,
			Eight    = 7,
			Nine     = 8,
			Ten      = 9,
			Eleven   = 10,
			Twelve   = 11,
			Thirteen = 12,
			Fourteen = 13,
			Fifteen  = 14,
			Sixteen  = 15
		};

		/// @brief Controller axes.
		enum class ControllerAxis : u32
		{
			Left_X        = 0,
			Left_Y        = 1,
			Right_X       = 2,
			Right_Y       = 3,
			Left_Trigger  = 4,
			Right_Trigger = 5
		};

		/// @brief Controller buttons.
		enum class ControllerButton : u32
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

		/// @brief Controller DPad.
		enum class ControllerDPad : u32
		{
			Centered   = 0,
			Up         = BIT(0u),
			Right      = BIT(1u),
			Down       = BIT(2u),
			Left       = BIT(3u),
			Right_Up   = Right | Up,
			Right_Down = Right | Down,
			Left_Up    = Left | Up,
			Left_Down  = Left | Down
		};

		/// @brief Controller Battery status.
		enum class ControllerBatteryStatus : u32
		{
			Wired = 0,
			Empty,
			Low,
			Medium,
			Full
		};

		/// @brief Check if a button on the keyboard is pressed.
		/// @param key Key to check.
		/// @return True if provided key is pressed, false otherwise.
		[[nodiscard]] static bool IsKeyPressed(Key key);
		/// @brief Check if a button on the keyboard in the specified window is pressed.
		/// @param key Key to check.
		/// @param window Window to check on.
		/// @return True if provided key is pressed in provided window, false otherwise.
		[[nodiscard]] static bool IsKeyPressed(Key key, const Window* window);
		/// @brief Check if a mouse button on the mouse is pressed.
		/// @param button Mouse button to check.
		/// @return True if provided mouse button is pressed, false otherwise.
		[[nodiscard]] static bool IsMouseButtonPressed(MouseButton button);
		/// @brief Check if a mouse button on the mouse in the specified window is pressed.
		/// @param button Mouse button to check.
		/// @param window Window to check on.
		/// @return True if provided mouse button is pressed in provided window, false otherwise.
		[[nodiscard]] static bool IsMouseButtonPressed(MouseButton button, const Window* window);
		/// @brief Retrieve whether raw mouse input is supported.
		/// @return True if raw mouse input is supported, false otherwise.
		[[nodiscard]] static bool IsRawMouseInputSupported();
		/// @brief Check if a specific controller is connected.
		/// @param controller Controller to check
		/// @return True if provided controller is connected, false otherwise.
		[[nodiscard]] static constexpr bool IsControllerConnected(Controller controller);
		/// @brief Check if a controller is a Gamepad (is able to map to an XBox like controller).
		/// @param controller Controller to check.
		/// @return True if controller is a Gamepad, false otherwise.
		[[nodiscard]] static bool IsControllerGamepad(Controller controller);

		/// @brief Retrieve the current mouse position.
		/// @return Mouse position as a Math::Vec2.
		[[nodiscard]] static Math::Vec2 GetMousePosition();
		/// @brief Retrieve the current mouse position for the provided window.
		/// @param window Window to check.
		/// @return Mouse position of the window as a Math::Vec2.
		[[nodiscard]] static Math::Vec2 GetMousePosition(const Window* window);
		/// @brief Retrieve the current mouse position on the x axis.
		/// @return X mouse position.
		[[nodiscard]] static f32 GetMouseX();
		/// @brief Retrieve the current mouse position on the y axis.
		/// @return Y mouse position.
		[[nodiscard]] static f32 GetMouseY();
		/// @brief Retrieve the current mouse position on the x axis for the provided window.
		/// @param window Window to check.
		/// @return X mouse position.
		[[nodiscard]] static f32 GetMouseX(const Window* window);
		/// @brief Retrieve the current mouse position on the y axis for the provided window.
		/// @param window Window to check.
		/// @return Y mouse position.
		[[nodiscard]] static f32 GetMouseY(const Window* window);
		/// @brief Retrieve the string representation of a key.
		/// @param key Key to retrieve.
		/// @return String representation of the key.
		[[nodiscard]] static std::string GetKeyName(Key key);
		/// @brief Retrieve the value of the specified controller axis from the provided controller.
		/// @param controller Controller to query.
		/// @param axis Controller axis to check.
		/// @return Value of the axis.
		[[nodiscard]] static f32 GetControllerAxis(Controller controller, ControllerAxis axis);
		/// @brief Retrieve the state of the specified DPad from the provided controller.
		/// @param controller Controller to query.
		/// @param dpad DPad to check.
		/// @return State of the DPad.
		[[nodiscard]] static ControllerDPad GetControllerDPad(Controller controller, u32 dpad);
		/// @brief Retrieve the battery status of the specified controller.
		/// @param controller Controller to query.
		/// @return Status of controller's battery.
		/// @remark @win32 This function only works with XInput controllers.
		/// @remark If controller is not used via XInput then ControllerBatteryStatus::Wired is returned.
		[[nodiscard]] static ControllerBatteryStatus GetControllerBatteryStatus(Controller controller);

		/// @brief Retrieve whether the specified controller button from the provided controller is pressed or not.
		/// @param controller Controller to query.
		/// @param button Controller button to check.
		/// @return True if controller button is pressed, false otherwise.
		[[nodiscard]] static bool IsControllerButtonPressed(Controller controller, ControllerButton button);
		/// @brief Retrieve the name of the provided controller.
		/// @param controller Controller to query.
		/// @return Name of the controller.
		[[nodiscard]] static std::string GetControllerName(Controller controller);
		/// @brief Retrieve the GUID of the provided controller.
		/// @param controller Controller to query.
		/// @return GUID of the controller.
		[[nodiscard]] static std::string GetControllerGUID(Controller controller);
		/// @brief Retrieve all states of all axes from the provided controller.
		/// @param controller Controller to query.
		/// @return Vector with axes state.
		[[nodiscard]] static std::vector<f32> GetAllControllerAxes(Controller controller);
		/// @brief Retrieve all states of every button from the provided controller.
		/// @param controller Controller to query.
		/// @return Vector with every button state.
		[[nodiscard]] static std::vector<bool> GetAllControllerButtons(Controller controller);
		/// @brief Retrieve all states of every DPad from the provided controller.
		/// @param controller Controller to query.
		/// @return Vector with every DPad state.
		[[nodiscard]] static std::vector<ControllerDPad> GetAllControllerDPads(Controller controller);

		/// @brief Retrieve the name of the current keyboard layout.
		/// @return Name of keyboard layout on success, empty optional otherwise.
		[[nodiscard]] static std::optional<std::string> GetKeyboardLayoutName();

		/// @brief Set the vibration level(s) for the specified controller.
		/// Intensity values are normalized and range from 0-1.
		/// @param controller Controller to affect.
		/// @param leftMotor Normalized intensity for the left motor.
		/// @param rightMotor Normalized intensity for the right motor.
		/// @remark @win32 Only takes effect on XInput controllers (DirectInput doesn't support vibrations).
		static void SetControllerVibration(Controller controller, f32 leftMotor, f32 rightMotor);
		/// @brief Set the vibration level(s) for the specified controller.
		/// Intensity values are normalized and range from 0-1.
		/// @param controller Controller to affect.
		/// @param intensity Normalized intensity for both motors.
		/// @remark @win32 Only takes effect on XInput controllers (DirectInput doesn't support vibrations).
		static void SetControllerVibration(Controller controller, const Math::Vec2& intensity);

		/// @brief Set the mouse position.
		/// @param x New x position.
		/// @param y New y position.
		static void SetMousePosition(f32 x, f32 y);
		/// @brief Set the mouse position for the provided Window.
		/// @param x New x position.
		/// @param y New y position.
		/// @param window Window to affect.
		static void SetMousePosition(f32 x, f32 y, const Window* window);
		/// @brief Set the mouse position.
		/// @param position New position.
		static void SetMousePosition(const Math::Vec2& position);
		/// @brief Set the mouse position for the provided window.
		/// @param position New position.
		/// @param window Window to affect.
		static void SetMousePosition(const Math::Vec2& position, const Window* window);

		/// @brief Set the system clipboard to the specified UTF-8 encoded string.
		/// @param str UTF-8 encoded string.
		static void SetClipboard(const std::string& str);
		/// @brief Retrieve the contents of the system clipboard.
		/// @return Contents of clipboard.
		[[nodiscard]] static std::string GetClipboard();

		/// @brief Update an existing Controller mapping or add a new one.
		///
		/// Description:
		/// Each mapping is a single line of comma-separated values describing the GUID,
		/// name and layout of the Controller.
		/// Note: Lines that do not begin with a hexadecimal digit are ignored!
		///
		/// The first value is always the Controller GUID, a 32 character long hexadecimal string
		/// that typically identifies its make, model, revision and the type
		/// of connection to the computer.
		/// When this information is not available, the GUID is generated using the Controller name.
		/// TRAP uses the SDL 2.0.5+ GUID format but can convert from the older formats.
		///
		/// The second value is always the human-readable name of the Controller.
		///
		/// All subsequent values are in the form "<field>:<value>" and describe the layout of the mapping.
		/// These fields may not all be present and may occur in any order.
		///
		/// The button fields are "a", "b", "c", "d", "back", "start", "guide", "dpup", "dpright", "dpdown",
		/// "dpleft", "leftshoulder", "rightshoulder", "leftstick" and "rightstick".
		/// The axis fields are "leftx", "lefty", "rightx", "righty", "lefttrigger" and "righttrigger".
		///
		/// The value of an axis or button field can be a Controller button, a Controller axis,
		/// a hat bitmask or empty.
		/// Controller buttons are specified as "bN", for example "b2" for the third button.
		/// Controller axes are specified as "aN", for example "a7" for the eighth axis.
		/// Controller hit bit masks are specified as "hN.N", for example "h0.8" for left on the first hat.
		/// More than one bit may be set in the mask.
		///
		/// Before an axis there may be a "+" or "-" range modifier, for example "+a3" for the
		/// positive half of the fourth axis.
		/// This restricts input to only the positive or negative halves of the Controller axis.
		/// After an axis of half-axis there may be the "~" inversion modifier, for example "a2~" or "-a7~".
		/// This negates the values of the Controller axis.
		///
		/// There is also the special "platform" field that specifies which platform the mapping is valid for.
		/// Possible values are "Windows" and "Linux".
		/// @param map Controller mapping to update or add.
		static void UpdateControllerMappings(std::string_view map);

		/// @brief Describes a callback function which gets called when an input event occurs.
		using EventCallbackFn = std::function<void(Events::Event&)>;

		/// @brief Get the function to call when an input event occurred.
		/// @return EventCallbackFn.
		[[nodiscard]] static EventCallbackFn GetEventCallback() noexcept;
	private:
		/// @brief Set the function to call when an input event occurred.
		/// @param callback Function to call.
		static void SetEventCallback(const EventCallbackFn& callback) noexcept;

		/// @brief Initialize input.
		static void Init();
		/// @brief Shutdown input.
		static void Shutdown();

		friend class TRAP::Application;

#ifdef TRAP_PLATFORM_WINDOWS
		/// @brief Check for new DirectInput & XInput controller connections.
		static void DetectControllerConnectionWin32();
		/// @brief Check for disconnections of DirectInput & XInput controllers.
		static void DetectControllerDisconnectionWin32();
#elif defined(TRAP_PLATFORM_LINUX)
		/// @brief Check for new connections and disconnection of controllers.
		static void DetectControllerConnectionLinux();
#endif
		friend class TRAP::INTERNAL::WindowingAPI;

		/// @brief Modes for polling.
		enum class PollMode : u32
		{
			Presence = 0,
			Axes = 1,
			Buttons = 2,
			All = (Axes | Buttons)
		};

		//Universal API Methods

		/// @brief Initialize controller(s).
		/// @return True on success, false otherwise.
		[[nodiscard]] static bool InitController();
		/// @brief Shutdown controller(s).
		static void ShutdownController();
		/// @brief OS-dependent function for controller vibrations.
		static void SetControllerVibrationInternal(Controller controller, f32 leftMotor, f32 rightMotor);
		/// @brief OS-dependent function for controller battery status.
		[[nodiscard]] static ControllerBatteryStatus GetControllerBatteryStatusInternal(Controller controller);
		/// @brief OS-dependent function for controller polling.
		/// @param controller Controller to poll.
		/// @param mode Mode for polling.
		/// @return True on success, false otherwise.
		static bool PollController(Controller controller, PollMode mode);
		/// @brief OS-dependent function for controller disconnections.
		/// @param controller Controller to disconnect.
		static void CloseController(Controller controller);
		/// @brief OS-dependent function for updating a controller's GUID.
		/// @param guid New GUID.
		static void UpdateControllerGUID(std::string& guid);

#ifdef TRAP_PLATFORM_WINDOWS
#define TRAP_BUILD_WIN32_MAPPINGS
		static constexpr std::string_view MappingName = "Windows";
		///////////////
		//DirectInput//
		///////////////
		//dinput8.dll function pointer typedefs
		static constexpr u32 TRAP_TYPE_AXIS = 0;
		static constexpr u32 TRAP_TYPE_SLIDER = 1;
		static constexpr u32 TRAP_TYPE_BUTTON = 2;
		static constexpr u32 TRAP_TYPE_DPAD = 3;
		//Define local copies of the necessary GUIDs
		static constexpr GUID TRAP_IID_IDirectInput8W =
		{ 0xbf798031,0x483a,0x4da2,{0xaa,0x99,0x5d,0x64,0xed,0x36,0x97,0x00} };
		static constexpr GUID TRAP_GUID_XAxis =
		{ 0xa36d02e0,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_YAxis =
		{ 0xa36d02e1,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_ZAxis =
		{ 0xa36d02e2,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_RxAxis =
		{ 0xa36d02f4,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_RyAxis =
		{ 0xa36d02f5,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_RzAxis =
		{ 0xa36d02e3,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_Slider =
		{ 0xa36d02e4,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_DPad =
		{ 0xa36d02f2,0xc9f3,0x11cf,{0xbf,0xc7,0x44,0x45,0x53,0x54,0x00,0x00} };
		static constexpr GUID TRAP_GUID_IID_IDirectInputDevice2W =
		{ 0x5944E683,0xC92E,0x11CF,{0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00} };
		//Object data array for our clone of c_dfDIJoystick
		inline static std::array<DIOBJECTDATAFORMAT, 44> TRAPObjectDataFormats =
		{
			{
				{ &TRAP_GUID_XAxis,DIJOFS_X,DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_YAxis,DIJOFS_Y,DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_ZAxis,DIJOFS_Z,DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_RxAxis,DIJOFS_RX,DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_RyAxis,DIJOFS_RY,DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_RzAxis,DIJOFS_RZ,DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_Slider,DIJOFS_SLIDER(0),DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_Slider,DIJOFS_SLIDER(1),DIDFT_AXIS | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,DIDOI_ASPECTPOSITION },
				{ &TRAP_GUID_DPad,DIJOFS_POV(0),DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ &TRAP_GUID_DPad,DIJOFS_POV(1),DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ &TRAP_GUID_DPad,DIJOFS_POV(2),DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ &TRAP_GUID_DPad,DIJOFS_POV(3),DIDFT_POV | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(0),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(1),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(2),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(3),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(4),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(5),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(6),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(7),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(8),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(9),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(10),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(11),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(12),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(13),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(14),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(15),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(16),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(17),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(18),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(19),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(20),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(21),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(22),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(23),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(24),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(25),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(26),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(27),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(28),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(29),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(30),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 },
				{ nullptr,DIJOFS_BUTTON(31),DIDFT_BUTTON | DIDFT_OPTIONAL | DIDFT_ANYINSTANCE,0 }
			}
		};
		//Our clone of c_dfDIJoystick
		inline static const DIDATAFORMAT TRAPDataFormat =
		{
			sizeof(DIDATAFORMAT),
			sizeof(DIOBJECTDATAFORMAT),
			DIDFT_ABSAXIS,
			sizeof(DIJOYSTATE2),
			TRAPObjectDataFormats.size(),
			TRAPObjectDataFormats.data()
		};
		using PFN_DirectInput8Create = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
		struct DInput8
		{
			HINSTANCE Instance{};
			PFN_DirectInput8Create Create{};
			IDirectInput8W* API = nullptr;
		} inline static s_dinput8{};

		struct Object;

		//////////
		//XInput//
		//////////
		//xinput.dll function pointer typedefs
		static constexpr u32 TRAP_XINPUT_CAPS_WIRELESS = 0x0002;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_GAMEPAD = 0x01;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_WHEEL = 0x02;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_ARCADE_STICK = 0x03;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_FLIGHT_STICK = 0x04;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_DANCE_PAD = 0x05;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_GUITAR = 0x06;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_DRUM_KIT = 0x08;
		static constexpr u32 TRAP_XINPUT_DEVSUBTYPE_ARCADE_PAD = 0x13;
		static constexpr u32 TRAP_XUSER_MAX_COUNT = 4;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_DPAD_UP = 0x0001;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_DPAD_DOWN = 0x0002;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_DPAD_LEFT = 0x0004;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_DPAD_RIGHT = 0x0008;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_START = 0x0010;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_BACK = 0x0020;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_LEFT_THUMB = 0x0040;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_RIGHT_THUMB = 0x0080;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_LEFT_SHOULDER = 0x0100;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_RIGHT_SHOULDER = 0x0200;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_A = 0x1000;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_B = 0x2000;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_X = 0x4000;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_Y = 0x8000;
		static constexpr u32 TRAP_XINPUT_GAMEPAD_GUIDE = 0x0400;
		static constexpr u32 TRAP_XINPUT_DEVTYPE_GAMEPAD = 0x00;
		static constexpr u32 TRAP_XINPUT_BATTERY_TYPE_DISCONNECTED = 0x00;
		static constexpr u32 TRAP_XINPUT_BATTERY_TYPE_WIRED = 0x01;
		static constexpr u32 TRAP_XINPUT_BATTERY_TYPE_ALKALINE = 0x02;
		static constexpr u32 TRAP_XINPUT_BATTERY_TYPE_NIMH = 0x03;
		static constexpr u32 TRAP_XINPUT_BATTERY_TYPE_UNKNOWN = 0xFF;
		static constexpr u32 TRAP_XINPUT_BATTERY_LEVEL_EMPTY = 0x00;
		static constexpr u32 TRAP_XINPUT_BATTERY_LEVEL_LOW = 0x01;
		static constexpr u32 TRAP_XINPUT_BATTERY_LEVEL_MEDIUM = 0x02;
		static constexpr u32 TRAP_XINPUT_BATTERY_LEVEL_FULL = 0x03;
		using PFN_XInputGetCapabilities = DWORD(WINAPI*)(DWORD, DWORD, XINPUT_CAPABILITIES*);
		using PFN_XInputGetState = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
		using PFN_XInputGetStateSecret = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
		using PFN_XInputSetState = DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*);
		using PFN_XInputGetBatteryInformation = DWORD(WINAPI*)(DWORD, BYTE, XINPUT_BATTERY_INFORMATION*);
		/// @brief DirectInput device object enumeration callback.
		/// @param doi DirectInput device object instance.
		/// @param user User data.
		/// @return True to continue enumeration, false to stop.
		static BOOL CALLBACK DeviceObjectCallback(const DIDEVICEOBJECTINSTANCEW* doi, void* user);
		/// @brief Lexically compare DirectInput device objects.
		/// @param first First object.
		/// @param second Second object.
		/// @return Difference between the two objects.
		[[nodiscard]] static bool CompareControllerObjects(const Object& first, const Object& second);
		/// @brief DirectInput device enumeration callback.
		/// @param deviceInstance DirectInput device instance.
		/// @param user User data.
		/// @return True to continue enumeration, false to stop.
		static BOOL CALLBACK DeviceCallback(const DIDEVICEINSTANCE* deviceInstance, void* user);
		/// @brief Checks whether the specified device supports XInput.
		/// @brief @param guid Device GUID to test.
		/// @return True if the device supports XInput, false otherwise.
		[[nodiscard]] static bool SupportsXInput(const GUID* guid);
		/// @brief Retrieve a description fitting to the specified XInput capabilities.
		/// @param xic XInput controller capabilities.
		/// @return Device description.
		[[nodiscard]] static std::string GetDeviceDescription(const XINPUT_CAPABILITIES* xic);
		struct XInput
		{
			HINSTANCE Instance{};
			PFN_XInputGetCapabilities GetCapabilities{};
			PFN_XInputGetState GetState{};
			PFN_XInputGetStateSecret GetStateSecret{};
			PFN_XInputSetState SetState{};
			PFN_XInputGetBatteryInformation GetBatteryInformation{};
			bool HasGuideButton = false;
		} inline static s_xinput{};

		struct Object
		{
			i32 Offset = 0;
			i32 Type = 0;
		};
		struct ControllerWindows
		{
			std::vector<Object> Objects{};
			i32 ObjectCount = 0;
			IDirectInputDevice8W* Device = nullptr;
			DWORD Index = 0;
			GUID guid{};
			bool XInput = false;
			bool ForceFeedback = false;
		};
		struct ObjectEnum
		{
			IDirectInputDevice8W* Device = nullptr;
			std::vector<Object> Objects;
			i32 ObjectCount = 0;
			i32 AxisCount = 0;
			i32 SliderCount = 0;
			i32 ButtonCount = 0;
			i32 PoVCount = 0;
		};
#elif defined(TRAP_PLATFORM_LINUX)
#define TRAP_BUILD_LINUX_MAPPINGS
		static constexpr std::string_view MappingName = "Linux";
		struct ControllerInternal;
		/// @brief Attempt to open the specified controller device.
		/// @param path Path to the controller file.
		/// @return True if the controller was opened successfully, false otherwise.
		static bool OpenControllerDeviceLinux(std::filesystem::path path);
		/// @brief Poll state of absolute axes for the specified controller.
		/// @param con Controller to poll.
		static void PollABSStateLinux(ControllerInternal* con);
		/// @brief Handle absolute event for to the specified controller.
		/// @param con Controller to handle event.
		/// @param code Event code.
		/// @param value Event value.
		static void HandleABSEventLinux(ControllerInternal* con, u32 code, i32 value);
		/// @brief Handle key event for the specified controller.
		/// @param con Controller to handle event.
		/// @param code Event code.
		/// @param value Event value.
		static void HandleKeyEventLinux(ControllerInternal* con, u32 code, i32 value);
		struct ControllerLinuxLibrary
		{
			i32 INotify = 0;
			i32 Watch = 0;
			std::regex Regex{};
			bool Dropped = false;
		};
		static ControllerLinuxLibrary s_linuxController;
		struct ControllerLinux
		{
			i32 FD = 0;
			bool VibrationSupported = false;
			std::optional<u16> CurrentVibration = std::nullopt;
			std::filesystem::path Path{};
			std::array<i32, KEY_CNT - BTN_MISC> KeyMap{};
			std::array<i32, ABS_CNT> ABSMap{};
			std::array<input_absinfo, ABS_CNT> ABSInfo{};
			std::array<std::array<i32, 4>, 2> DPads{};
		};
#endif

		static EventCallbackFn s_eventCallback;

		//Controller mapping element
		struct MapElement
		{
			u8 Type = 0;
			u8 Index = 0;
			i8 AxisScale = 0;
			i8 AxisOffset = 0;
		};

		//Controller mapping
		struct Mapping
		{
			std::string Name{};
			std::string guid{};
			std::array<MapElement, 15> Buttons{};
			std::array<MapElement, 6> Axes{};
		};

		struct ControllerInternal
		{
			std::vector<f32> Axes{};
			std::vector<bool> Buttons{};
			i32 ButtonCount = 0;
			std::vector<ControllerDPad> DPads{};
			std::string Name{};
			void* UserPointer = nullptr;
			std::string guid{};
			Mapping* mapping = nullptr;
			bool Connected = false;
			f32 LeftMotor = 0.0f;
			f32 RightMotor = 0.0f;

#ifdef TRAP_PLATFORM_WINDOWS
			ControllerWindows WinCon;
#elif defined(TRAP_PLATFORM_LINUX)
			ControllerLinux LinuxCon;
#endif
		};
		static std::array<ControllerInternal, 16> s_controllerInternal;
		/// @brief Internally add a controller.
		/// @param name Name of the controller.
		/// @param guid GUID of the controller.
		/// @param axisCount Amount of axes.
		/// @param buttonCount Amount of buttons.
		/// @param dpadCount Amount of DPads.
		/// @return Pointer to the new controller internal or nullptr if there are already 16 controllers connected.
		static ControllerInternal* AddInternalController(std::string name, std::string guid,
		                                                 u32 axisCount, u32 buttonCount, u32 dpadCount);
		/// @brief Internally DPad input processing.
		/// @param con Which controller.
		/// @param dpad Which DPad.
		/// @param value New state.
		static constexpr void InternalInputControllerDPad(ControllerInternal* con, i32 dpad, u8 value);
		/// @brief Internally axis input processing.
		/// @param con Which controller
		/// @param axis Which axis.
		/// @param value New axis value.
		static constexpr void InternalInputControllerAxis(ControllerInternal* con, i32 axis, f32 value);
		/// @brief Internally button input processing.
		/// @param con Which controller.
		/// @param button Which button.
		/// @param pressed New state, pressed or not pressed.
		static constexpr void InternalInputControllerButton(ControllerInternal* con, i32 button, bool pressed);

		///////////
		//Mapping//
		///////////
		inline static std::vector<Mapping> s_mappings{};

		/// @brief Initializes controller mappings.
		static void InitControllerMappings();
		/// @brief Parse a string to a mapping.
		/// @param mapping Output variable for the new mapping.
		/// @param str Mapping as a string representation.
		/// @return True on success, false otherwise.
		static bool ParseMapping(Mapping& mapping, std::string_view str);
		/// @brief Find a loaded mapping via its GUID.
		/// @param guid GUID to search for.
		/// @return When found pointer to mapping, nullptr otherwise.
		[[nodiscard]] static constexpr Mapping* FindMapping(std::string_view guid);
		/// @brief Find a mapping based on controller GUID and verifies validity.
		/// @param con Controller from which to use GUID.
		/// @return When found and valid pointer to mapping, nullptr otherwise.
		[[nodiscard]] static constexpr Mapping* FindValidMapping(const ControllerInternal* con);
		/// @brief Checks whether a controller mapping element is present in the hardware.
		/// @param e Map element to check.
		/// @param con Controller to validate with.
		/// @return True if map element is present in hardware, false otherwise.
		[[nodiscard]] static constexpr bool IsValidElementForController(const MapElement* e, const ControllerInternal* con);
		/// @brief Retrieve state of a specific button from a controller.
		/// @param controller Controller to query.
		/// @param button Button to get state from.
		/// @return On success button state, false otherwise.
		[[nodiscard]] static bool GetMappedControllerButton(Controller controller, ControllerButton button);
		/// @brief Retrieve state of a specific axis from a controller.
		/// @param controller Controller to query.
		/// @param axis Axis to get state from.
		/// @return On success axis state, 0.0f otherwise.
		[[nodiscard]] static f32 GetMappedControllerAxis(Controller controller, ControllerAxis axis);
		/// @brief Retrieve state of a specific DPad from a controller.
		/// @param controller Controller to query.
		/// @param dpad DPad to get state from.
		/// @return On success DPad state, ControllerDPad::Centered otherwise.
		[[nodiscard]] static ControllerDPad GetMappedControllerDPad(Controller controller, u32 dpad);
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Input::IsControllerConnected(const Controller controller)
{
	return s_controllerInternal[std::to_underlying(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller DPad
constexpr void TRAP::Input::InternalInputControllerDPad(ControllerInternal* const con, const i32 dpad, const u8 value)
{
	const u32 base = NumericCast<u32>(con->ButtonCount) + NumericCast<u32>(dpad) * 4u;

	con->Buttons[base + 0u] = ((value & BIT(0u)) != 0u); //Up
	con->Buttons[base + 1u] = ((value & BIT(1u)) != 0u); //Right
	con->Buttons[base + 2u] = ((value & BIT(2u)) != 0u); //Down
	con->Buttons[base + 3u] = ((value & BIT(3u)) != 0u); //Left

	if (con->Buttons[base + 1u] && con->Buttons[base + 0u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Right_Up;
	else if (con->Buttons[base + 1u] && con->Buttons[base + 2u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Right_Down;
	else if (con->Buttons[base + 3u] && con->Buttons[base + 0u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Left_Up;
	else if (con->Buttons[base + 3u] && con->Buttons[base + 2u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Left_Down;
	else if (con->Buttons[base + 0u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Up;
	else if (con->Buttons[base + 1u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Right;
	else if (con->Buttons[base + 2u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Down;
	else if (con->Buttons[base + 3u])
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Left;
	else
		con->DPads[NumericCast<usize>(dpad)] = ControllerDPad::Centered;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller axis
constexpr void TRAP::Input::InternalInputControllerAxis(ControllerInternal* const con, const i32 axis, const f32 value)
{
	con->Axes[NumericCast<usize>(axis)] = value;
}

//-------------------------------------------------------------------------------------------------------------------//

//Notifies shared code of the new value of a controller button
constexpr void TRAP::Input::InternalInputControllerButton(ControllerInternal* const con, const i32 button, const bool pressed)
{
	con->Buttons[NumericCast<usize>(button)] = pressed;
}

//-------------------------------------------------------------------------------------------------------------------//

//Find a mapping based on controller GUID
[[nodiscard]] constexpr TRAP::Input::Mapping* TRAP::Input::FindMapping(const std::string_view guid)
{
	for (auto& Mapping : s_mappings)
	{
		if(Mapping.guid == guid)
			return &Mapping;
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

//Finds a mapping based on controller GUID and verifies element indices
[[nodiscard]] constexpr TRAP::Input::Mapping* TRAP::Input::FindValidMapping(const ControllerInternal* const con)
{
	Mapping* const mapping = FindMapping(con->guid);

	if(mapping == nullptr)
		return nullptr;

	u32 i = 0;

	for(i = 0; i <= std::to_underlying(ControllerButton::DPad_Left); i++)
	{
		if(!IsValidElementForController(&mapping->Buttons[i], con))
		{
			TP_ERROR(Log::InputControllerPrefix, "Invalid button in controller mapping: ", mapping->guid,
						" ", mapping->Name);
			return nullptr;
		}
	}

	for(i = 0; i <= std::to_underlying(ControllerAxis::Right_Trigger); i++)
	{
		if(!IsValidElementForController(&mapping->Axes[i], con))
		{
			TP_ERROR(Log::InputControllerPrefix, "Invalid axis in controller mapping: ", mapping->guid,
						" ", mapping->Name);
			return nullptr;
		}
	}

	return mapping;
}

//-------------------------------------------------------------------------------------------------------------------//

//Checks whether a controller mapping element is present in the hardware
[[nodiscard]] constexpr bool TRAP::Input::IsValidElementForController(const MapElement* const e, const ControllerInternal* const con)
{
	if(e->Type == 3 && (e->Index >> 4u) >= NumericCast<i32>(con->DPads.size() + 1))
		return false;
	if(e->Type == 2 && e->Index >= (con->Buttons.size() + 1))
		return false;
	if(e->Type == 1 && e->Index >= (con->Axes.size() + 1))
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Input::MouseButton>
{
    static constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Input::MouseButton button, fmt::format_context& ctx)
    {
		std::string enumStr{};
		switch(button)
		{
		case TRAP::Input::MouseButton::Left:
			enumStr = "Left";
			break;

		case TRAP::Input::MouseButton::Right:
			enumStr = "Right";
			break;

		case TRAP::Input::MouseButton::Middle:
			enumStr = "Middle";
			break;

		default:
            break;
		}

		if(enumStr.empty())
			enumStr = fmt::format("Button {}", std::to_underlying(button));

		return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

template<>
struct fmt::formatter<TRAP::Input::KeyState>
{
    static constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    static fmt::format_context::iterator format(const TRAP::Input::KeyState keyState, fmt::format_context& ctx)
    {
		std::string enumStr{};
		switch(keyState)
		{
		case TRAP::Input::KeyState::Released:
			enumStr = "Released";
			break;

		case TRAP::Input::KeyState::Pressed:
			enumStr = "Pressed";
			break;

		case TRAP::Input::KeyState::Repeat:
			enumStr = "Repeat";
			break;

		default:
            TRAP_ASSERT(false, "fmt::formatter<TRAP::Input::KeyState: Missing enum value!");
            enumStr = "<MISSING ENUM VALUE>";
            break;
		}

		return fmt::format_to(ctx.out(), "{}", enumStr);
    }
};

//-------------------------------------------------------------------------------------------------------------------//

MAKE_ENUM_FLAG(TRAP::Input::ControllerDPad)

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_INPUT_H*/
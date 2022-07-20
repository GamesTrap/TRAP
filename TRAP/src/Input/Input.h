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

#include "Core/PlatformDetection.h" //Needed for OS dependent includes

#include <functional>
#include <array>
#include <string_view>

#ifdef TRAP_PLATFORM_LINUX
#include "Utils/Linux.h"
#endif

#include "Maths/Math.h"
#include "Events/Event.h"

namespace TRAP
{
	class Application;

	namespace INTERNAL
	{
		class WindowingAPI;
	}

	class Window;

	class Input final
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		Input() = delete;
		/// <summary>
		/// Destructor.
		/// </summary>
		~Input() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Input(const Input&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Input& operator=(const Input&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Input(Input&&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Input& operator=(Input&&) = delete;

		/// <summary>
		/// Keyboard keys.
		/// </summary>
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

		/// <summary>
		/// Key state.
		/// </summary>
		enum class KeyState
		{
			Released,
			Pressed,
			Repeat
		};

		/// <summary>
		/// Mouse buttons.
		/// </summary>
		enum class MouseButton
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

		/// <summary>
		/// Controllers
		/// </summary>
		enum class Controller
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

		/// <summary>
		/// Controller axes.
		/// </summary>
		enum class ControllerAxis
		{
			Left_X        = 0,
			Left_Y        = 1,
			Right_X       = 2,
			Right_Y       = 3,
			Left_Trigger  = 4,
			Right_Trigger = 5
		};

		/// <summary>
		/// Controller buttons.
		/// </summary>
		enum class ControllerButton
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

		/// <summary>
		/// Controller DPad.
		/// </summary>
		enum class ControllerDPad
		{
			Centered   = 0,
			Up         = BIT(0),
			Right      = BIT(1),
			Down       = BIT(2),
			Left       = BIT(3),
			Right_Up   = Right | Up,
			Right_Down = Right | Down,
			Left_Up    = Left | Up,
			Left_Down  = Left | Down
		};

		/// <summary>
		/// Controller Battery status.
		/// </summary>
		enum class ControllerBatteryStatus
		{
			Wired = 0,
			Empty,
			Low,
			Medium,
			Full
		};

		/// <summary>
		/// Check if a button on the keyboard is pressed.
		/// </summary>
		/// <param name="key">Key to check.</param>
		/// <returns>True if provided key is pressed, false otherwise.</returns>
		static bool IsKeyPressed(Key key);
		/// <summary>
		/// Check if a button on the keyboard in the specified window is pressed.
		/// </summary>
		/// <param name="key">Key to check.</param>
		/// <param name="window">Window to check on.</param>
		/// <returns>True if provided key is pressed in provided window, false otherwise.</returns>
		static bool IsKeyPressed(Key key, const Window* const window);
		/// <summary>
		/// Check if a mouse button on the mouse is pressed.
		/// </summary>
		/// <param name="button">Mouse button to check.</param>
		/// <returns>True if provided mouse button is pressed, false otherwise.</returns>
		static bool IsMouseButtonPressed(MouseButton button);
		/// <summary>
		/// Check if a mouse button on the mouse in the specified window is pressed.
		/// </summary>
		/// <param name="button">Mouse button to check.</param>
		/// <param name="window">Window to check on.</param>
		/// <returns>True if provided mouse button is pressed in provided window, false otherwise.</returns>
		static bool IsMouseButtonPressed(MouseButton button, const Window* const window);
		/// <summary>
		/// Retrieve whether raw mouse input is supported.
		/// </summary>
		/// <returns>True if raw mouse input is supported, false otherwise.</returns>
		static bool IsRawMouseInputSupported();
		/// <summary>
		/// Check if a specific controller is connected.
		/// </summary>
		/// <param name="controller">Controller to check</param>
		/// <returns>True if provided controller is connected, false otherwise.</returns>
		static bool IsControllerConnected(Controller controller);
		/// <summary>
		/// Check if a controller is a Gamepad (is able to map to an XBox like controller).
		/// </summary>
		/// <param name="controller">Controller to check.</param>
		/// <returns>True if controller is a Gamepad, false otherwise.</returns>
		static bool IsControllerGamepad(Controller controller);

		/// <summary>
		/// Retrieve the current mouse position.
		/// </summary>
		/// <returns>Mouse position as a Math::Vec2.</returns>
		static Math::Vec2 GetMousePosition();
		/// <summary>
		/// Retrieve the current mouse position for the provided window.
		/// </summary>
		/// <param name="window">Window to check.</param>
		/// <returns>Mouse position of the window as a Math::Vec2.</returns>
		static Math::Vec2 GetMousePosition(const Window* const window);
		/// <summary>
		/// Retrieve the current mouse position on the x axis.
		/// </summary>
		/// <returns>X mouse position.</returns>
		static float GetMouseX();
		/// <summary>
		/// Retrieve the current mouse position on the y axis.
		/// </summary>
		/// <returns>Y mouse position.</returns>
		static float GetMouseY();
		/// <summary>
		/// Retrieve the current mouse position on the x axis for the provided window.
		/// </summary>
		/// <param name="window">Window to check.</param>
		/// <returns>X mouse position.</returns>
		static float GetMouseX(const Window* const window);
		/// <summary>
		/// Retrieve the current mouse position on the y axis for the provided window.
		/// </summary>
		/// <param name="window">Window to check.</param>
		/// <returns>Y mouse position.</returns>
		static float GetMouseY(const Window* const window);
		/// <summary>
		/// Retrieve the string representation of a key.
		/// </summary>
		/// <param name="key">Key to retrieve.</param>
		/// <returns>String representation of the key.</returns>
		static std::string GetKeyName(Key key);
		/// <summary>
		/// Retrieve the value of the specified controller axis from the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <param name="axis">Controller axis to check.</param>
		/// <returns>Value of the axis.</returns>
		static float GetControllerAxis(Controller controller, ControllerAxis axis);
		/// <summary>
		/// Retrieve the state of the specified DPad from the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <param name="dpad">DPad to check.</param>
		/// <returns>State of the DPad.</returns>
		static ControllerDPad GetControllerDPad(Controller controller, uint32_t dpad);
		/// <summary>
		/// Retrieve the battery status of the specified controller.
		/// Note: This function is only available on Windows using XInput.
		///       On unsupported platforms & APIs, it will always return ControllerBatteryStatus::Wired.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <returns>Status of controller's battery.</returns>
		static ControllerBatteryStatus GetControllerBatteryStatus(Controller controller);

		/// <summary>
		/// Retrieve whether the specified controller button from the provided controller is pressed or not.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <param name="button">Controller button to check.</param>
		/// <returns>True if controller button is pressed, false otherwise.</returns>
		static bool IsControllerButtonPressed(Controller controller, ControllerButton button);
		/// <summary>
		/// Retrieve the name of the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <returns>Name of the controller.</returns>
		static std::string GetControllerName(Controller controller);
		/// <summary>
		/// Retrieve the GUID of the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <returns>GUID of the controller.</returns>
		static std::string GetControllerGUID(Controller controller);
		/// <summary>
		/// Retrieve all states of all axes from the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <returns>Vector with axes state.</returns>
		static std::vector<float> GetAllControllerAxes(Controller controller);
		/// <summary>
		/// Retrieve all states of every button from the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <returns>Vector with every button state.</returns>
		static std::vector<bool> GetAllControllerButtons(Controller controller);
		/// <summary>
		/// Retrieve all states of every DPad from the provided controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <returns>Vector with every DPad state.</returns>
		static std::vector<ControllerDPad> GetAllControllerDPads(Controller controller);

		/// <summary>
		/// Retrieve the name of the current keyboard layout.
		/// </summary>
		/// <returns>Name of keyboard layout.</returns>
		static std::string GetKeyboardLayoutName();

		/// <summary>
		/// Set the vibration level(s) for the specified controller.
		/// Intensity values are normalized and range from 0-1.
		/// Note: Only takes effect when on Linux or Windows via XInput (DirectInput doesn't support vibrations)!
		/// </summary>
		/// <param name="controller">Controller to affect.</param>
		/// <param name="leftMotor">Normalized intensity for the left motor.</param>
		/// <param name="rightMotor">Normalized intensity for the right motor.</param>
		static void SetControllerVibration(Controller controller, float leftMotor, float rightMotor);
		/// <summary>
		/// Set the vibration level(s) for the specified controller.
		/// Intensity values are normalized and range from 0-1.
		/// Note: Only takes effect when on Linux or Windows via XInput (DirectInput doesn't support vibrations)!
		/// </summary>
		/// <param name="controller">Controller to affect.</param>
		/// <param name="intensity">Normalized intensity for both motors.</param>
		static void SetControllerVibration(Controller controller, const Math::Vec2& intensity);

		/// <summary>
		/// Set the mouse position.
		/// </summary>
		/// <param name="x">New x position.</param>
		/// <param name="y">New y position.</param>
		static void SetMousePosition(float x, float y);
		/// <summary>
		/// Set the mouse position for the provided Window.
		/// </summary>
		/// <param name="x">New x position.</param>
		/// <param name="y">New y position.</param>
		/// <param name="window">Window to affect.</param>
		static void SetMousePosition(float x, float y, const Window* const window);
		/// <summary>
		/// Set the mouse position.
		/// </summary>
		/// <param name="position">New position.</param>
		static void SetMousePosition(const Math::Vec2& position);
		/// <summary>
		/// Set the mouse position for the provided window.
		/// </summary>
		/// <param name="position">New position.</param>
		/// <param name="window">Window to affect.</param>
		static void SetMousePosition(const Math::Vec2& position, const Window* const window);

		/// <summary>
		/// Set the system clipboard to the specified UTF-8 encoded string.
		/// </summary>
		/// <param name="str">UTF-8 encoded string.</param>
		static void SetClipboard(const std::string& str);
		/// <summary>
		/// Retrieve the contents of the system clipboard.
		/// </summary>
		/// <returns>Contents of clipboard.</returns>
		static std::string GetClipboard();

		/// <summary>
		/// Update an existing Controller mapping or add a new one.
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
		/// </summary>
		/// <param name="map">Controller mapping to update or add.</param>
		static void UpdateControllerMappings(const std::string& map);

		/// <summary>
		/// Describes a callback function which gets called when an input event occurs.
		/// </summary>
		using EventCallbackFn = std::function<void(Events::Event&)>;

		/// <summary>
		/// Get the function to call when an input event occurred.
		/// </summary>
		/// <returns>EventCallbackFn.</returns>
		static EventCallbackFn GetEventCallback();
	private:
		/// <summary>
		/// Set the function to call when an input event occurred.
		/// </summary>
		/// <param name="callback">Function to call.</param>
		static void SetEventCallback(const EventCallbackFn& callback);

		/// <summary>
		/// Initialize input.
		/// </summary>
		static void Init();
		/// <summary>
		/// Shutdown input.
		/// </summary>
		static void Shutdown();

		friend class TRAP::Application;

#ifdef TRAP_PLATFORM_WINDOWS
		/// <summary>
		/// Check for new DirectInput & XInput controller connections.
		/// </summary>
		static void DetectControllerConnectionWin32();
		/// <summary>
		/// Check for disconnections of DirectInput & XInput controllers.
		/// </summary>
		static void DetectControllerDisconnectionWin32();
#elif defined(TRAP_PLATFORM_LINUX)
		/// <summary>
		/// Check for new connections and disconnection of controllers.
		/// </summary>
		static void DetectControllerConnectionLinux();
#endif
		friend class TRAP::INTERNAL::WindowingAPI;

		/// <summary>
		/// Modes for polling.
		/// </summary>
		enum class PollMode
		{
			Presence = 0,
			Axes = 1,
			Buttons = 2,
			All = (Axes | Buttons)
		};

		//Universal API Methods

		/// <summary>
		/// Initialize controller(s).
		/// </summary>
		/// <returns>True on success, false otherwise.</returns>
		static bool InitController();
		/// <summary>
		/// Shutdown controller(s).
		/// </summary>
		static void ShutdownController();
		/// <summary>
		/// OS-dependent function for controller vibrations.
		/// </summary>
		static void SetControllerVibrationInternal(Controller controller, float leftMotor, float rightMotor);
		/// <summary>
		/// OS-dependent function for controller battery status.
		/// </summary>
		static ControllerBatteryStatus GetControllerBatteryStatusInternal(Controller controller);
		/// <summary>
		/// OS-dependent function for controller polling.
		/// </summary>
		/// <param name="controller">Controller to poll.</param>
		/// <param name="mode">Mode for polling.</param>
		/// <returns>True on success, false otherwise.</returns>
		static bool PollController(Controller controller, PollMode mode);
		/// <summary>
		/// OS-dependent function for controller disconnections.
		/// </summary>
		/// <param name="controller">Controller to disconnect.</param>
		static void CloseController(Controller controller);
		/// <summary>
		/// OS-dependent function for updating a controller's GUID.
		/// </summary>
		/// <param name="guid">New GUID.</param>
		static void UpdateControllerGUID(std::string& guid);

#ifdef TRAP_PLATFORM_WINDOWS
#define TRAP_BUILD_WIN32_MAPPINGS
		static constexpr std::string_view MappingName = "Windows";
		///////////////
		//DirectInput//
		///////////////
		//dinput8.dll function pointer typedefs
		static constexpr uint32_t TRAP_TYPE_AXIS = 0;
		static constexpr uint32_t TRAP_TYPE_SLIDER = 1;
		static constexpr uint32_t TRAP_TYPE_BUTTON = 2;
		static constexpr uint32_t TRAP_TYPE_DPAD = 3;
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
			static_cast<DWORD>(TRAPObjectDataFormats.size()),
			TRAPObjectDataFormats.data()
		};
		using PFN_DirectInput8Create = HRESULT(WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
		struct DInput8
		{
			HINSTANCE Instance{};
			PFN_DirectInput8Create Create{};
			IDirectInput8W* API = nullptr;
		} inline static s_dinput8{};

		//////////
		//XInput//
		//////////
		//xinput.dll function pointer typedefs
		static constexpr uint32_t TRAP_XINPUT_CAPS_WIRELESS = 0x0002;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_GAMEPAD = 0x01;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_WHEEL = 0x02;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_ARCADE_STICK = 0x03;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_FLIGHT_STICK = 0x04;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_DANCE_PAD = 0x05;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_GUITAR = 0x06;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_DRUM_KIT = 0x08;
		static constexpr uint32_t TRAP_XINPUT_DEVSUBTYPE_ARCADE_PAD = 0x13;
		static constexpr uint32_t TRAP_XUSER_MAX_COUNT = 4;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_DPAD_UP = 0x0001;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_DPAD_DOWN = 0x0002;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_DPAD_LEFT = 0x0004;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_DPAD_RIGHT = 0x0008;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_START = 0x0010;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_BACK = 0x0020;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_LEFT_THUMB = 0x0040;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_RIGHT_THUMB = 0x0080;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_LEFT_SHOULDER = 0x0100;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_RIGHT_SHOULDER = 0x0200;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_A = 0x1000;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_B = 0x2000;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_X = 0x4000;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_Y = 0x8000;
		static constexpr uint32_t TRAP_XINPUT_GAMEPAD_GUIDE = 0x0400;
		static constexpr uint32_t TRAP_XINPUT_DEVTYPE_GAMEPAD = 0x00;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_TYPE_DISCONNECTED = 0x00;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_TYPE_WIRED = 0x01;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_TYPE_ALKALINE = 0x02;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_TYPE_NIMH = 0x03;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_TYPE_UNKNOWN = 0xFF;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_LEVEL_EMPTY = 0x00;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_LEVEL_LOW = 0x01;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_LEVEL_MEDIUM = 0x02;
		static constexpr uint32_t TRAP_XINPUT_BATTERY_LEVEL_FULL = 0x03;
		using PFN_XInputGetCapabilities = DWORD(WINAPI*)(DWORD, DWORD, XINPUT_CAPABILITIES*);
		using PFN_XInputGetState = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
		using PFN_XInputGetStateSecret = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
		using PFN_XInputSetState = DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*);
		using PFN_XInputGetBatteryInformation = DWORD(WINAPI*)(DWORD, BYTE, XINPUT_BATTERY_INFORMATION*);
		/// <summary>
		/// DirectInput device object enumeration callback.
		/// </summary>
		/// <param name="doi">DirectInput device object instance.</param>
		/// <param name="user">User data.</param>
		/// <returns>True to continue enumeration, false to stop.</returns>
		static BOOL CALLBACK DeviceObjectCallback(const DIDEVICEOBJECTINSTANCEW* doi, void* user);
		/// <summary>
		/// Lexically compare DirectInput device objects.
		/// </summary>
		/// <param name="first">First object.</param>
		/// <param name="second">Second object.</param>
		/// <returns>Difference between the two objects.</returns>
		static int CompareControllerObjects(const void* first, const void* second);
		/// <summary>
		/// DirectInput device enumeration callback.
		/// </summary>
		/// <param name="deviceInstance">DirectInput device instance.</param>
		/// <param name="user">User data.</param>
		/// <returns>True to continue enumeration, false to stop.</returns>
		static BOOL CALLBACK DeviceCallback(const DIDEVICEINSTANCE* deviceInstance, void* user);
		/// <summary>
		/// Checks whether the specified device supports XInput.
		/// <summary>
		/// <param name="guid">Device GUID to test.</param>
		/// <returns>True if the device supports XInput, false otherwise.</returns>
		static bool SupportsXInput(const GUID* guid);
		/// <summary>
		/// Retrieve a description fitting to the specified XInput capabilities.
		/// </summary>
		/// <param name="xic">XInput controller capabilities.</param>
		/// <returns>Device description.</returns>
		static std::string GetDeviceDescription(const XINPUT_CAPABILITIES* xic);
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
			int32_t Offset = 0;
			int32_t Type = 0;
		};
		struct ControllerWindows
		{
			std::vector<Object> Objects{};
			int32_t ObjectCount = 0;
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
			int32_t ObjectCount = 0;
			int32_t AxisCount = 0;
			int32_t SliderCount = 0;
			int32_t ButtonCount = 0;
			int32_t PoVCount = 0;
		};
#elif defined(TRAP_PLATFORM_LINUX)
#define TRAP_BUILD_LINUX_MAPPINGS
		static constexpr std::string_view MappingName = "Linux";
		struct ControllerInternal;
		/// <summary>
		/// Attempt to open the specified controller device.
		/// </summary>
		/// <param name="path">Path to the controller file.</param>
		/// <returns>True if the controller was opened successfully, false otherwise.</returns>
		static bool OpenControllerDeviceLinux(std::filesystem::path path);
		/// <summary>
		/// Poll state of absolute axes for the specified controller.
		/// </summary>
		/// <param name="con">Controller to poll.</param>
		static void PollABSStateLinux(ControllerInternal* con);
		/// <summary>
		/// Handle absolute event for to the specified controller.
		/// </summary>
		/// <param name="con">Controller to handle event.</param>
		/// <param name="code">Event code.</param>
		/// <param name="value">Event value.</param>
		static void HandleABSEventLinux(ControllerInternal* con, int32_t code, int32_t value);
		/// <summary>
		/// Handle key event for the specified controller.
		/// </summary>
		/// <param name="con">Controller to handle event.</param>
		/// <param name="code">Event code.</param>
		/// <param name="value">Event value.</param>
		static void HandleKeyEventLinux(ControllerInternal* con, int32_t code, int32_t value);
		struct ControllerLinuxLibrary
		{
			int32_t INotify = 0;
			int32_t Watch = 0;
			regex_t Regex{};
			bool Dropped = false;
		};
		static ControllerLinuxLibrary s_linuxController;
		struct ControllerLinux
		{
			int32_t FD = 0;
			bool VibrationSupported = false;
			int16_t CurrentVibration = -1;
			std::filesystem::path Path{};
			std::array<int32_t, KEY_CNT - BTN_MISC> KeyMap{};
			std::array<int32_t, ABS_CNT> ABSMap{};
			std::array<input_absinfo, ABS_CNT> ABSInfo{};
			std::array<std::array<int32_t, 4>, 2> DPads{};
		};
#endif

		static EventCallbackFn s_eventCallback;

		//Controller mapping element
		struct MapElement
		{
			uint8_t Type = 0;
			uint8_t Index = 0;
			int8_t AxisScale = 0;
			int8_t AxisOffset = 0;
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
			std::vector<float> Axes{};
			std::vector<bool> Buttons{};
			int32_t ButtonCount = 0;
			std::vector<ControllerDPad> DPads{};
			std::string Name{};
			void* UserPointer = nullptr;
			std::string guid{};
			Mapping* mapping = nullptr;
			bool Connected = false;
			float LeftMotor = 0.0f;
			float RightMotor = 0.0f;

#ifdef TRAP_PLATFORM_WINDOWS
			ControllerWindows WinCon;
#elif defined(TRAP_PLATFORM_LINUX)
			ControllerLinux LinuxCon;
#endif
		};
		static std::array<ControllerInternal, 16> s_controllerInternal;
		/// <summary>
		/// Internally add a controller.
		/// </summary>
		/// <param name="name">Name of the controller.</param>
		/// <param name="guid">GUID of the controller.</param>
		/// <param name="axisCount">Amount of axes.</param>
		/// <param name="buttonCount">Amount of buttons.</param>
		/// <param name="dpadCount">Amount of DPads.</param>
		/// <returns>Pointer to the new controller internal or nullptr if there are already 16 controllers connected.</returns>
		static ControllerInternal* AddInternalController(std::string name, std::string guid,
		                                                 int32_t axisCount, int32_t buttonCount, int32_t dpadCount);
		/// <summary>
		/// Internally DPad input processing.
		/// </summary>
		/// <param name="con">Which controller.</param>
		/// <param name="dpad">Which DPad.</param>
		/// <param name="value">New state.</param>
		static void InternalInputControllerDPad(ControllerInternal* con, int32_t dpad, uint8_t value);
		/// <summary>
		/// Internally axis input processing.
		/// </summary>
		/// <param name="con">Which controller</param>
		/// <param name="axis">Which axis.</param>
		/// <param name="value">New axis value.</param>
		static void InternalInputControllerAxis(ControllerInternal* con, int32_t axis, float value);
		/// <summary>
		/// Internally button input processing.
		/// </summary>
		/// <param name="con">Which controller.</param>
		/// <param name="button">Which button.</param>
		/// <param name="pressed">New state, pressed or not pressed.</param>
		static void InternalInputControllerButton(ControllerInternal* con, int32_t button, bool pressed);

		///////////
		//Mapping//
		///////////
		static std::vector<Mapping> s_mappings;

		/// <summary>
		/// Initializes controller mappings.
		/// </summary>
		static void InitControllerMappings();
		/// <summary>
		/// Parse a string to a mapping.
		/// </summary>
		/// <param name="mapping">Output variable for the new mapping.</param>
		/// <param name="str">Mapping as a string representation.</param>
		/// <returns>True on success, false otherwise.</returns>
		static bool ParseMapping(Mapping& mapping, const std::string& str);
		/// <summary>
		/// Find a loaded mapping via its GUID.
		/// </summary>
		/// <param name="guid">GUID to search for.</param>
		/// <returns>When found pointer to mapping, nullptr otherwise.</returns>
		static Mapping* FindMapping(std::string_view guid);
		/// <summary>
		/// Find a mapping based on controller GUID and verifies validity.
		/// </summary>
		/// <param name="con">Controller from which to use GUID.</param>
		/// <returns>When found and valid pointer to mapping, nullptr otherwise.</returns>
		static Mapping* FindValidMapping(const ControllerInternal* con);
		/// <summary>
		/// Checks whether a controller mapping element is present in the hardware.
		/// </summary>
		/// <param name="e">Map element to check.</param>
		/// <param name="con">Controller to validate with.</param>
		/// <returns>True if map element is present in hardware, false otherwise.</returns>
		static bool IsValidElementForController(const MapElement* e, const ControllerInternal* con);
		/// <summary>
		/// Retrieve state of a specific button from a controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <param name="button">Button to get state from.</param>
		/// <returns>On success button state, false otherwise.</returns>
		static bool GetMappedControllerButton(Controller controller, ControllerButton button);
		/// <summary>
		/// Retrieve state of a specific axis from a controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <param name="axis">Axis to get state from.</param>
		/// <returns>On success axis state, 0.0f otherwise.</returns>
		static float GetMappedControllerAxis(Controller controller, ControllerAxis axis);
		/// <summary>
		/// Retrieve state of a specific DPad from a controller.
		/// </summary>
		/// <param name="controller">Controller to query.</param>
		/// <param name="dpad">DPad to get state from.</param>
		/// <returns>On success DPad state, ControllerDPad::Centered otherwise.</returns>
		static ControllerDPad GetMappedControllerDPad(Controller controller, uint32_t dpad);
	};
}

MAKE_ENUM_FLAG(TRAP::Input::ControllerDPad);

#endif /*TRAP_INPUT_H*/
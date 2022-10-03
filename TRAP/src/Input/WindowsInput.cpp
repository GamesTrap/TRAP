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
*/

#include "TRAPPCH.h"
#include "Core/PlatformDetection.h"

#include "Input/Input.h"
#include "Utils/String/String.h"

#ifdef TRAP_PLATFORM_WINDOWS

#define TRAP_BUILD_WIN32_MAPPINGS

#include "Window/WindowingAPI.h"

#include "Events/ControllerEvent.h"
#include "ControllerMappings.h"

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::InitController()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(!s_dinput8.Instance)
		s_dinput8.Instance = LoadLibraryA("dinput8.dll");
	if (s_dinput8.Instance)
		s_dinput8.Create = reinterpret_cast<PFN_DirectInput8Create>(GetProcAddress(s_dinput8.Instance,
		                                                                           "DirectInput8Create"));

	if(!s_dinput8.Instance)
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to open dinput8.dll!");
		return false;
	}

	if (!s_dinput8.Create || FAILED(s_dinput8.Create(INTERNAL::WindowingAPI::GetWin32HInstance(), DIRECTINPUT_VERSION,
														TRAP_IID_IDirectInput8W,
														reinterpret_cast<void**>(&s_dinput8.API), nullptr)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to create interface for DirectInput!");
		return false;
	}

	{
		if (!s_xinput.Instance)
		{
			const std::array<std::string, 5> names =
			{
				"xinput1_4.dll",
				"xinput1_3.dll",
				"xinput9_1_0.dll",
				"xinput1_2.dll",
				"xinput1_1.dll"
			};

			for (const std::string_view dll : names)
			{
				s_xinput.Instance = LoadLibraryA(dll.data());
				if (s_xinput.Instance)
				{
					s_xinput.GetCapabilities = reinterpret_cast<PFN_XInputGetCapabilities>
					(
						GetProcAddress
						(
							s_xinput.Instance,
							"XInputGetCapabilities"
						)
					);

					//Ordinal 100 is the same as XInputGetState, except it doesn't dummy out the guide button info.
					//Try loading it and fall back if needed.
					s_xinput.GetStateSecret = reinterpret_cast<PFN_XInputGetStateSecret>(GetProcAddress(s_xinput.Instance,
					                                                                                    reinterpret_cast<LPCSTR>(100)));
					if (s_xinput.GetStateSecret)
						s_xinput.HasGuideButton = true;

					s_xinput.GetState = reinterpret_cast<PFN_XInputGetState>(GetProcAddress(s_xinput.Instance,
																								"XInputGetState"));
					s_xinput.SetState = reinterpret_cast<PFN_XInputSetState>(GetProcAddress(s_xinput.Instance,
																							"XInputSetState"));

					s_xinput.GetBatteryInformation = reinterpret_cast<PFN_XInputGetBatteryInformation>
						(
							GetProcAddress(s_xinput.Instance, "XInputGetBatteryInformation")
						);
					break;
				}
			}

			if(!s_xinput.Instance)
			{
				TP_ERROR(Log::InputControllerXInputPrefix, "Failed to create interface for XInput!");
				return false;
			}
		}
	}

	DetectControllerConnectionWin32();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::ShutdownController()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	for (uint32_t cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
			CloseController(static_cast<Controller>(cID));

	if (s_dinput8.API)
		IDirectInput8_Release(s_dinput8.API);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerGUID(std::string& guid)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (std::string_view(guid.data() + 20) == "504944564944")
	{
		const std::string original = guid;

		guid = "03000000" + std::string(original.begin(), original.begin() + 4) + "0000" +
		       std::string(original.begin() + 4, original.begin() + 4 + 4) + "000000000000";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::DetectControllerConnectionWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(s_xinput.Instance)
	{
		for(DWORD index = 0; index < TRAP_XUSER_MAX_COUNT; index++)
		{
			uint32_t cID;
			std::string guid;
			guid.resize(33);
			XINPUT_CAPABILITIES xic;

			for(cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
			{
				if (!s_controllerInternal[cID].Connected &&
					s_controllerInternal[cID].WinCon.Device == nullptr &&
					s_controllerInternal[cID].WinCon.Index == index)
					break;
			}

			if (cID > static_cast<uint32_t>(Controller::Sixteen))
				continue;

			if (s_xinput.GetCapabilities(index, 0, &xic) != ERROR_SUCCESS)
				continue;

			//Generate a Controller GUID that matches the SDL 2.0.5+ one
			sprintf_s(guid.data(), guid.size(), "78696e707574%02x000000000000000000",
				      xic.SubType & 0xFF);
			guid.pop_back();

			ControllerInternal* con = AddInternalController(GetDeviceDescription(&xic), guid, 6, 11, 1);
			if (!con)
				continue;

			con->WinCon.Index = index;
			con->WinCon.XInput = true;

			if (!s_eventCallback)
				continue;

			Events::ControllerConnectEvent event(static_cast<Controller>(cID));
			s_eventCallback(event);
		}
	}

	if (s_dinput8.API)
	{
		if (FAILED(IDirectInput8_EnumDevices(s_dinput8.API, DI8DEVCLASS_GAMECTRL, DeviceCallback, nullptr,
		                                     DIEDFL_ALLDEVICES)))
			TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to enumerate DirectInput devices!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::DetectControllerDisconnectionWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	for (uint32_t cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		if (s_controllerInternal[cID].Connected)
			PollController(static_cast<Controller>(cID), PollMode::Presence);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibrationInternal(Controller controller, const float leftMotor,
                                                 const float rightMotor)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(!s_controllerInternal[static_cast<uint32_t>(controller)].WinCon.XInput)
		return;

	const uint16_t left = static_cast<uint16_t>(static_cast<float>(65535) * leftMotor);
	const uint16_t right = static_cast<uint16_t>(static_cast<float>(65535) * rightMotor);
	XINPUT_VIBRATION vibration{ left, right };
	const uint32_t result = s_xinput.SetState(static_cast<DWORD>(controller), &vibration);
	if (result != ERROR_SUCCESS)
	{
		TP_ERROR(Log::InputControllerXInputPrefix, "ID: ", static_cast<uint32_t>(controller), " Error: ",
					result, " while setting vibration!");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerBatteryStatus TRAP::Input::GetControllerBatteryStatusInternal(const Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if(!s_controllerInternal[static_cast<uint32_t>(controller)].WinCon.XInput)
		return ControllerBatteryStatus::Wired;

	XINPUT_BATTERY_INFORMATION batteryInformation{};
	s_xinput.GetBatteryInformation(static_cast<DWORD>(controller), TRAP_XINPUT_DEVTYPE_GAMEPAD, &batteryInformation);

	if(batteryInformation.BatteryType == TRAP_XINPUT_BATTERY_TYPE_WIRED)
		return ControllerBatteryStatus::Wired;
	if(batteryInformation.BatteryType == TRAP_XINPUT_BATTERY_TYPE_ALKALINE ||
       batteryInformation.BatteryLevel == TRAP_XINPUT_BATTERY_TYPE_NIMH)
	{
		if(batteryInformation.BatteryLevel == TRAP_XINPUT_BATTERY_LEVEL_EMPTY)
			return ControllerBatteryStatus::Empty;
		if(batteryInformation.BatteryLevel == TRAP_XINPUT_BATTERY_LEVEL_LOW)
			return ControllerBatteryStatus::Low;
		if(batteryInformation.BatteryLevel == TRAP_XINPUT_BATTERY_LEVEL_MEDIUM)
			return ControllerBatteryStatus::Medium;
		/*if(batteryInformation.BatteryLevel == TRAP_XINPUT_BATTERY_LEVEL_FULL)*/
		return ControllerBatteryStatus::Full;
	}

	return ControllerBatteryStatus::Wired;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::PollController(const Controller controller, const PollMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	ControllerInternal* con = &s_controllerInternal[static_cast<uint32_t>(controller)];
	if (con->WinCon.Device)
	{
		int ai = 0, bi = 0, pi = 0;
		DIJOYSTATE state{};

		IDirectInputDevice8_Poll(con->WinCon.Device);
		HRESULT result = IDirectInputDevice8_GetDeviceState(con->WinCon.Device, sizeof(state), &state);

		if (result == DIERR_NOTACQUIRED || result == DIERR_INPUTLOST)
		{
			IDirectInputDevice8_Acquire(con->WinCon.Device);
			IDirectInputDevice8_Poll(con->WinCon.Device);
			result = IDirectInputDevice8_GetDeviceState(con->WinCon.Device, sizeof(state), &state);
		}

		if (FAILED(result))
		{
			CloseController(controller);
			return false;
		}

		if (mode == PollMode::Presence)
			return true;

		for (int32_t i = 0; i < con->WinCon.ObjectCount; i++)
		{
			void* data = reinterpret_cast<char*>(&state) + con->WinCon.Objects[i].Offset;

			switch (con->WinCon.Objects[i].Type)
			{
			case TRAP_TYPE_AXIS:
				[[fallthrough]];
			case TRAP_TYPE_SLIDER:
			{
				const float value = (*static_cast<LONG*>(data) + 0.5f) / 32767.5f;
				InternalInputControllerAxis(con, ai, value);
				ai++;
				break;
			}

			case TRAP_TYPE_BUTTON:
			{
				const char value = (*static_cast<BYTE*>(data) & 0x80) != 0;
				InternalInputControllerButton(con, bi, value);
				bi++;
				break;
			}

			case TRAP_TYPE_DPAD:
			{
				constexpr std::array<uint8_t, 9> states =
				{
					static_cast<uint8_t>(ControllerDPad::Up),
					static_cast<uint8_t>(ControllerDPad::Right_Up),
					static_cast<uint8_t>(ControllerDPad::Right),
					static_cast<uint8_t>(ControllerDPad::Right_Down),
					static_cast<uint8_t>(ControllerDPad::Down),
					static_cast<uint8_t>(ControllerDPad::Left_Down),
					static_cast<uint8_t>(ControllerDPad::Left),
					static_cast<uint8_t>(ControllerDPad::Left_Up),
					static_cast<uint8_t>(ControllerDPad::Centered)
				};

				//Screams of horror are appropriate at this point
				int32_t status = LOWORD(*static_cast<DWORD*>(data)) / (45 * DI_DEGREES);
				if (status < 0 || status > 8)
					status = 8;

				InternalInputControllerDPad(con, pi, states[status]);
				pi++;
				break;
			}

			default:
				break;
			}
		}
	}
	else
	{
		uint32_t dpad = 0;
		XINPUT_STATE xis;
		constexpr std::array<WORD, 10> buttons =
		{
			TRAP_XINPUT_GAMEPAD_A,
			TRAP_XINPUT_GAMEPAD_B,
			TRAP_XINPUT_GAMEPAD_X,
			TRAP_XINPUT_GAMEPAD_Y,
			TRAP_XINPUT_GAMEPAD_LEFT_SHOULDER,
			TRAP_XINPUT_GAMEPAD_RIGHT_SHOULDER,
			TRAP_XINPUT_GAMEPAD_BACK,
			TRAP_XINPUT_GAMEPAD_START,
			TRAP_XINPUT_GAMEPAD_LEFT_THUMB,
			TRAP_XINPUT_GAMEPAD_RIGHT_THUMB
		};

		const DWORD result = s_xinput.GetState(con->WinCon.Index, &xis);
		if(result != ERROR_SUCCESS)
		{
			if (result == ERROR_DEVICE_NOT_CONNECTED)
				CloseController(controller);

			return false;
		}

		if (mode == PollMode::Presence)
			return true;

		InternalInputControllerAxis(con, 0, (xis.Gamepad.sThumbLX + 0.5f) / 32767.5f);
		InternalInputControllerAxis(con, 1, -(xis.Gamepad.sThumbLY + 0.5f) / 32767.5f);
		InternalInputControllerAxis(con, 2, (xis.Gamepad.sThumbRX + 0.5f) / 32767.5f);
		InternalInputControllerAxis(con, 3, -(xis.Gamepad.sThumbRY + 0.5f) / 32767.5f);
		InternalInputControllerAxis(con, 4, xis.Gamepad.bLeftTrigger / 127.5f - 1.f);
		InternalInputControllerAxis(con, 5, xis.Gamepad.bRightTrigger / 127.5f - 1.f);

		for(std::size_t i = 0; i < buttons.size(); i++)
		{
			const bool value = (xis.Gamepad.wButtons & buttons[i]) ? true : false;
			InternalInputControllerButton(con, i, value);
		}

		//Guide button is never reported in caps
		if(s_xinput.HasGuideButton)
		{
			XINPUT_STATE xiss{};
			const DWORD res = s_xinput.GetStateSecret(con->WinCon.Index, &xiss);
			if(res == ERROR_SUCCESS)
			{
				const bool guidePressed = (xiss.Gamepad.wButtons & TRAP_XINPUT_GAMEPAD_GUIDE) ? true : false;
				InternalInputControllerButton(con, 10, guidePressed);
			}
		}

		if (xis.Gamepad.wButtons & TRAP_XINPUT_GAMEPAD_DPAD_UP)
			dpad |= static_cast<uint32_t>(ControllerDPad::Up);
		if (xis.Gamepad.wButtons & TRAP_XINPUT_GAMEPAD_DPAD_RIGHT)
			dpad |= static_cast<uint32_t>(ControllerDPad::Right);
		if (xis.Gamepad.wButtons & TRAP_XINPUT_GAMEPAD_DPAD_DOWN)
			dpad |= static_cast<uint32_t>(ControllerDPad::Down);
		if (xis.Gamepad.wButtons & TRAP_XINPUT_GAMEPAD_DPAD_LEFT)
			dpad |= static_cast<uint32_t>(ControllerDPad::Left);

		InternalInputControllerDPad(con, 0, static_cast<uint8_t>(dpad));
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::CloseController(Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	if (s_controllerInternal[static_cast<uint32_t>(controller)].WinCon.Device)
	{
		IDirectInputDevice8_Unacquire(s_controllerInternal[static_cast<uint32_t>(controller)].WinCon.Device);
		IDirectInputDevice8_Release(s_controllerInternal[static_cast<uint32_t>(controller)].WinCon.Device);
	}

	const bool connected = s_controllerInternal[static_cast<uint32_t>(controller)].Connected;

	if(connected)
	{
		TP_INFO(Log::InputControllerPrefix, "Controller: ",
		        (s_controllerInternal[static_cast<uint32_t>(controller)].mapping
			         ? s_controllerInternal[static_cast<uint32_t>(controller)].mapping->Name
			         : s_controllerInternal[static_cast<uint32_t>(controller)].Name),
		        " (", static_cast<uint32_t>(controller), ") disconnected!");
	}

	s_controllerInternal[static_cast<uint32_t>(controller)] = {};

	if (!s_eventCallback)
		return;

	if(connected)
	{
		Events::ControllerDisconnectEvent event(controller);
		s_eventCallback(event);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//DirectInput device object enumeration callback
BOOL CALLBACK TRAP::Input::DeviceObjectCallback(const DIDEVICEOBJECTINSTANCEW* doi, void* user)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	ObjectEnum* data = static_cast<ObjectEnum*>(user);
	Object* object = &data->Objects[data->ObjectCount];

	if (DIDFT_GETTYPE(doi->dwType) & DIDFT_AXIS)
	{
		DIPROPRANGE dipr;

		if (std::memcmp(&doi->guidType, &TRAP_GUID_Slider, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_SLIDER(data->SliderCount);
		else if (std::memcmp(&doi->guidType, &TRAP_GUID_XAxis, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_X;
		else if (std::memcmp(&doi->guidType, &TRAP_GUID_YAxis, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_Y;
		else if (std::memcmp(&doi->guidType, &TRAP_GUID_ZAxis, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_Z;
		else if (std::memcmp(&doi->guidType, &TRAP_GUID_RxAxis, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_RX;
		else if (std::memcmp(&doi->guidType, &TRAP_GUID_RyAxis, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_RY;
		else if (std::memcmp(&doi->guidType, &TRAP_GUID_RzAxis, sizeof(GUID)) == 0)
			object->Offset = DIJOFS_RZ;
		else
			return DIENUM_CONTINUE;

		dipr.diph.dwSize = sizeof(dipr);
		dipr.diph.dwHeaderSize = sizeof(dipr.diph);
		dipr.diph.dwObj = doi->dwType;
		dipr.diph.dwHow = DIPH_BYID;
		dipr.lMin = -32768;
		dipr.lMax = 32767;

		if (FAILED(IDirectInputDevice8_SetProperty(data->Device, DIPROP_RANGE, &dipr.diph)))
			return DIENUM_CONTINUE;

		if (std::memcmp(&doi->guidType, &TRAP_GUID_Slider, sizeof(GUID)) == 0)
		{
			object->Type = TRAP_TYPE_SLIDER;
			data->SliderCount++;
		}
		else
		{
			object->Type = TRAP_TYPE_AXIS;
			data->AxisCount++;
		}
	}
	else if (DIDFT_GETTYPE(doi->dwType) & DIDFT_BUTTON)
	{
		object->Offset = DIJOFS_BUTTON(data->ButtonCount);
		object->Type = TRAP_TYPE_BUTTON;
		data->ButtonCount++;
	}
	else if (DIDFT_GETTYPE(doi->dwType) & DIDFT_POV)
	{
		object->Offset = DIJOFS_POV(data->PoVCount);
		object->Type = TRAP_TYPE_DPAD;
		data->PoVCount++;
	}

	data->ObjectCount++;
	return DIENUM_CONTINUE;
}

//-------------------------------------------------------------------------------------------------------------------//

//Checks whether the specified device supports XInput
bool TRAP::Input::SupportsXInput(const GUID* guid)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	uint32_t count = 0;
	std::vector<RAWINPUTDEVICELIST> ridl{};
	bool result = false;

	if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0)
		return false;

	ridl.resize(count);

	if (GetRawInputDeviceList(ridl.data(), &count, sizeof(RAWINPUTDEVICELIST)) == static_cast<uint32_t>(-1))
		return false;

	for (uint32_t i = 0; i < count; i++)
	{
		RID_DEVICE_INFO rdi;
		std::array<char, 256> name{};
		uint32_t size;

		if (ridl[i].dwType != RIM_TYPEHID)
			continue;

		rdi.cbSize = sizeof(rdi);
		size = sizeof(rdi);

		if (static_cast<int32_t>(GetRawInputDeviceInfoA(ridl[i].hDevice,
			RIDI_DEVICEINFO, &rdi, &size)) == -1)
			continue;

		if (MAKELONG(rdi.hid.dwVendorId, rdi.hid.dwProductId) != static_cast<int64_t>(guid->Data1))
			continue;

		size = static_cast<uint32_t>(name.size());

		if (static_cast<int32_t>(GetRawInputDeviceInfoA(ridl[i].hDevice,
			RIDI_DEVICENAME,
			name.data(),
			&size)) == -1)
			break;

		name[name.size() - 1] = '\0';
		if (strstr(name.data(), "IG_"))
		{
			result = true;
			break;
		}
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

//Returns a description fitting the specified XInput capabilities
std::string TRAP::Input::GetDeviceDescription(const XINPUT_CAPABILITIES* xic)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch (xic->SubType)
	{
		case TRAP_XINPUT_DEVSUBTYPE_WHEEL:
			return "XInput Wheel";
		case TRAP_XINPUT_DEVSUBTYPE_ARCADE_STICK:
			return "XInput Arcade Stick";
		case TRAP_XINPUT_DEVSUBTYPE_FLIGHT_STICK:
			return "XInput Flight Stick";
		case TRAP_XINPUT_DEVSUBTYPE_DANCE_PAD:
			return "XInput Dance Pad";
		case TRAP_XINPUT_DEVSUBTYPE_GUITAR:
			return "XInput Guitar";
		case TRAP_XINPUT_DEVSUBTYPE_DRUM_KIT:
			return "XInput Drum Kit";
		case TRAP_XINPUT_DEVSUBTYPE_GAMEPAD:
		{
			if (xic->Flags & TRAP_XINPUT_CAPS_WIRELESS)
				return "Wireless Xbox Controller";

			return "Xbox Controller";
		}

		default:
			return "Unknown XInput Device";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Lexically compare device objects
int TRAP::Input::CompareControllerObjects(const void* first, const void* second)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	const Object* fo = static_cast<const Object*>(first);
	const Object* so = static_cast<const Object*>(second);

	if (fo->Type != so->Type)
		return fo->Type - so->Type;

	return fo->Offset - so->Offset;
}

//-------------------------------------------------------------------------------------------------------------------//

//DirectInput device enumeration callback
BOOL CALLBACK TRAP::Input::DeviceCallback(const DIDEVICEINSTANCE* deviceInstance, void*)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	DIDEVCAPS dc{};
	DIPROPDWORD dipd{};
	IDirectInputDevice8* device = nullptr;
	ControllerInternal* controller;
	ObjectEnum data{};
	std::string guid;
	guid.resize(33);
	std::string name;
	name.resize(256);

	for (uint32_t cID = 0; cID <= static_cast<uint32_t>(Controller::Sixteen); cID++)
	{
		controller = &s_controllerInternal[cID];
		if (s_controllerInternal[cID].Connected)
		{
			if (std::memcmp(&controller->WinCon.guid, &deviceInstance->guidInstance, sizeof(GUID)) == 0)
				return DIENUM_CONTINUE;
		}
	}

	if (SupportsXInput(&deviceInstance->guidProduct))
		return DIENUM_CONTINUE;

	if (FAILED(IDirectInput8_CreateDevice(s_dinput8.API, deviceInstance->guidInstance, &device, nullptr)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to create device!");
		return DIENUM_CONTINUE;
	}

	/*if(FAILED(IDirectInputDevice8_SetCooperativeLevel(device,
		static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>(TRAP::Application::GetWindow()->GetInternalWindow())->Handle,
		DISCL_EXCLUSIVE | DISCL_BACKGROUND)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to set cooperative level!");
		IDirectInputDevice8_Release(device);
		return DIENUM_CONTINUE;
	}*/

	if (FAILED(IDirectInputDevice8_SetDataFormat(device, &TRAPDataFormat)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to set device data format!");
		IDirectInputDevice8_Release(device);
		return DIENUM_CONTINUE;
	}

	dc.dwSize = sizeof(dc);

	if (FAILED(IDirectInputDevice8_GetCapabilities(device, &dc)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to query device capabilities!");
		IDirectInputDevice8_Release(device);
		return DIENUM_CONTINUE;
	}

	//bool forceFeedback = false;
	if(dc.dwFlags & DIDC_FORCEFEEDBACK)
	{
		//This device supports Force Feedback
		/*LPDIRECTINPUTDEVICE2 lpDirectInputJoystick;
		device->QueryInterface(TRAP_GUID_IID_IDirectInputDevice2W,
		                       reinterpret_cast<LPVOID*>(&lpDirectInputJoystick));
		//Try to use it
		if (FAILED(lpDirectInputJoystick->SetCooperativeLevel(static_cast<TRAP::INTERNAL::WindowingAPI::InternalWindow*>
			(TRAP::Application::GetWindow()->GetInternalWindow())->Handle,
			DISCL_BACKGROUND | DISCL_EXCLUSIVE)))
			TP_ERROR(Log::InputControllerDirectInputPrefix,
			         "Failed to set cooperation level to exclusive! Ignoring Force Feedback feature");
		else
		{
			forceFeedback = true;
			TP_TRACE(Log::InputControllerDirectInputPrefix, "Force Feedback enabled!");
		}

		lpDirectInputJoystick->Release();*/
		//TODO
		TP_DEBUG(Log::InputControllerDirectInputPrefix, "Controller supports Force Feedback!\n",
		         "This feature is not implemented because all my controllers do not set this flag\nPls help thx");
	}

	dipd.diph.dwSize = sizeof(dipd);
	dipd.diph.dwHeaderSize = sizeof(dipd.diph);
	dipd.diph.dwHow = DIPH_DEVICE;
	dipd.dwData = DIPROPAXISMODE_ABS;

	if (FAILED(IDirectInputDevice8_SetProperty(device, DIPROP_AXISMODE, &dipd.diph)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to set device axis mode!");
		IDirectInputDevice8_Release(device);
		return DIENUM_CONTINUE;
	}

	data.Device = device;
	data.Objects.resize(dc.dwAxes + dc.dwButtons + dc.dwPOVs);

	if (FAILED(IDirectInputDevice8_EnumObjects(device, DeviceObjectCallback, &data,
	                                           DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV)))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to enumerate device objects!");
		IDirectInputDevice8_Release(device);
		return DIENUM_CONTINUE;
	}

	std::qsort(data.Objects.data(), data.ObjectCount, sizeof(Object), CompareControllerObjects);

	if (!WideCharToMultiByte(CP_UTF8, 0, deviceInstance->tszInstanceName, -1, name.data(),
	                         static_cast<int32_t>(name.size()), nullptr, nullptr))
	{
		TP_ERROR(Log::InputControllerDirectInputPrefix, "Failed to convert controller name to UTF-8");
		IDirectInputDevice8_Release(device);
		return DIENUM_STOP;
	}

	//Generate a Controller GUID that matches the SDL 2.0.5+ one
	if (std::memcmp(&deviceInstance->guidProduct.Data4[2], "PIDVID", 6) == 0)
	{
		sprintf_s(guid.data(), guid.size(), "03000000%02x%02x0000%02x%02x000000000000",
			static_cast<uint8_t>(deviceInstance->guidProduct.Data1),
			static_cast<uint8_t>(deviceInstance->guidProduct.Data1 >> 8),
			static_cast<uint8_t>(deviceInstance->guidProduct.Data1 >> 16),
			static_cast<uint8_t>(deviceInstance->guidProduct.Data1 >> 24)
		);
	}
	else
	{
		sprintf_s(guid.data(), guid.size(), "05000000%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x00",
			name[0], name[1], name[2], name[3],
			name[4], name[5], name[6], name[7],
			name[8], name[9], name[10]
		);
	}

	name.erase(std::find(name.begin(), name.end(), '\0'), name.end());

	controller = AddInternalController(name, guid, data.AxisCount + data.SliderCount, data.ButtonCount,
	                                   data.PoVCount);

	if (!controller)
	{
		IDirectInputDevice8_Release(device);
		return DIENUM_STOP;
	}

	controller->WinCon.Device = device;
	controller->WinCon.guid = deviceInstance->guidInstance;
	controller->WinCon.Objects = data.Objects;
	controller->WinCon.ObjectCount = data.ObjectCount;
	/*if (forceFeedback)
		controller->WinCon.ForceFeedback = true;*/

	if (!s_eventCallback)
		return DIENUM_STOP;

	//Get index of our ControllerInternal
	uint8_t index;
	for (index = 0; index <= static_cast<uint8_t>(Controller::Sixteen); index++)
		if (&s_controllerInternal[index] == controller)
			break;

	Events::ControllerConnectEvent event(static_cast<Controller>(index));
	s_eventCallback(event);

	return DIENUM_STOP;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetKeyboardLayoutName()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Input);

	std::array<WCHAR, KL_NAMELENGTH> keyboardLayoutID{};

	if(!GetKeyboardLayoutNameW(keyboardLayoutID.data()))
	{
		TP_ERROR(Log::InputWinAPIPrefix, "Failed to retrieve keyboard layout name");
		return "";
	}

	//NOTE: Only care about the language part of the keyboard layout ID
	const LCID lcID = MAKELCID(LANGIDFROMLCID(wcstoul(keyboardLayoutID.data(), nullptr, 16)), 0);

	const uint32_t size = GetLocaleInfoW(lcID, LOCALE_SLANGUAGE, nullptr, 0);
	if(!size)
	{
		TP_ERROR(Log::InputWinAPIPrefix, "Failed to retrieve keyboard layout name length");
		return "";
	}

	std::wstring language(size, 0);

	if(!GetLocaleInfoW(lcID, LOCALE_SLANGUAGE, language.data(), size))
	{
		TP_ERROR(Log::InputWinAPIPrefix, "Failed to translate keyboard layout name");
		return "";
	}

	return TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(language);
}

#endif
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
#include "Input/Input.h"

#ifndef TRAP_HEADLESS_MODE

#ifdef TRAP_PLATFORM_LINUX

#include <regex>

#include "Events/ControllerEvent.h"
#include "Window/WindowingAPI.h"
#include "Utils/String/String.h"

TRAP::Input::ControllerLinuxLibrary TRAP::Input::s_linuxController{};

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Input::InitController()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	static constexpr std::string_view dirName = "/dev/input";

	s_linuxController.INotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
	if(s_linuxController.INotify > 0)
	{
		//HACK: Register for IN_ATTRIB to get notified when udev is done
		//This works well in practice but the true way is sd-device
		s_linuxController.Watch = inotify_add_watch(s_linuxController.INotify, dirName.data(),
		                                            IN_CREATE | IN_ATTRIB | IN_DELETE);
		if(s_linuxController.Watch < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to add watch for ", dirName, "!");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		}
	}
	else
	{
		TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to initialize inotify!");
		TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
	}

	//Continue without device connection notifications if inotify fails
	s_linuxController.Regex = std::regex("^event[0-9]+$", std::regex_constants::extended);

	for(const auto& entry : std::filesystem::directory_iterator(dirName))
	{
		if(std::regex_match(entry.path().filename().string(), s_linuxController.Regex))
			OpenControllerDeviceLinux(entry);
	}

	//Continue with no controllers if enumeration fails
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::ShutdownController()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	for(u32 cID = 0; cID <= std::to_underlying(Controller::Sixteen); cID++)
	{
		if(s_controllerInternal[cID].LinuxCon.CurrentVibration)
			SetControllerVibration(static_cast<Controller>(cID), 0.0f, 0.0f);
		if (s_controllerInternal[cID].Connected)
			CloseController(static_cast<Controller>(cID));
	}

	if(s_linuxController.INotify > 0)
	{
		if (s_linuxController.Watch > 0)
		{
			if(inotify_rm_watch(s_linuxController.INotify, s_linuxController.Watch) < 0)
			{
				TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to remove watch!");
				TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
			}
		}

		if(close(s_linuxController.INotify) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to close inotify!");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		}
	}

	s_controllerInternal = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibrationInternal(Controller controller, const f32 leftMotor, const f32 rightMotor)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if(!PollController(controller, PollMode::Presence))
		return;

	ControllerInternal* const con = &s_controllerInternal[std::to_underlying(controller)];

	if(!con->LinuxCon.VibrationSupported)
		return;

	input_event play{};
	//Delete any existing effect
	if(con->LinuxCon.CurrentVibration)
	{
		//Stop the effect
		play.type = EV_FF;
		play.code = *con->LinuxCon.CurrentVibration;
		play.value = 0;

		if(write(con->LinuxCon.FD, static_cast<const void*>(&play), sizeof(play)) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to stop vibration");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
			return;
		}

		//Delete the effect
		if(ioctl(con->LinuxCon.FD, EVIOCRMFF, *con->LinuxCon.CurrentVibration) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to delete vibration");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
			return;
		}
	}

	//If VibrationSupported is true, start the new effect
	if(leftMotor != 0.0f || rightMotor != 0.0f)
	{
		ff_effect ff{};

		//Define an effect for this vibration setting
		ff.type = FF_RUMBLE;
		ff.id = -1;
		ff.u.rumble.strong_magnitude = NumericCast<u16>(leftMotor * std::numeric_limits<u16>::max());
		ff.u.rumble.weak_magnitude = NumericCast<u16>(rightMotor * std::numeric_limits<u16>::max());
		ff.replay.length = std::numeric_limits<u16>::max();
		ff.replay.delay = 0;

		//Upload the effect
		if(ioctl(con->LinuxCon.FD, EVIOCSFF, &ff) >= 0)
			con->LinuxCon.CurrentVibration = NumericCast<u16>(ff.id);
		else
		{
			con->LinuxCon.CurrentVibration = std::nullopt;
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to upload vibration");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
			return;
		}

		//Play the effect
		play.type = EV_FF;
		play.code = *con->LinuxCon.CurrentVibration;
		play.value = 1;

		if(write(con->LinuxCon.FD, static_cast<const void*>(&play), sizeof(play)) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to start vibration");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
			return;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Input::ControllerBatteryStatus TRAP::Input::GetControllerBatteryStatusInternal([[maybe_unused]] Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return ControllerBatteryStatus::Wired;
}

//-------------------------------------------------------------------------------------------------------------------//

//Attempt to open the specified controller device
bool TRAP::Input::OpenControllerDeviceLinux(std::filesystem::path path)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	for(u32 cID = 0; cID <= std::to_underlying(Controller::Sixteen); cID++)
	{
		if (!s_controllerInternal[cID].Connected)
			continue;
		if (s_controllerInternal[cID].LinuxCon.Path == path)
			return false;
	}

	ControllerLinux LinuxCon = {};
	LinuxCon.FD = open(path.c_str(), O_RDWR | O_NONBLOCK); //O_RDWR is needed for vibrations
	if(LinuxCon.FD != -1)
		LinuxCon.VibrationSupported = true;

	if(errno == EACCES)
		LinuxCon.FD = open(path.c_str(), O_RDONLY | O_NONBLOCK);

	if (LinuxCon.FD < 0)
	{
		TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to open controller device!");
		TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		return false;
	}

	const std::array<u8, (EV_CNT + 7) / 8> EVBits{};
	const std::array<u8, (KEY_CNT + 7) / 8> keyBits{};
	const std::array<u8, (ABS_CNT + 7) / 8> ABSBits{};
	input_id ID{};

	if (ioctl(LinuxCon.FD, EVIOCGBIT(0, EVBits.size()), EVBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGBIT(EV_KEY, keyBits.size()), keyBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGBIT(EV_ABS, ABSBits.size()), ABSBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGID, &ID) < 0)
	{
		TP_ERROR(Log::InputControllerLinuxPrefix, "Could not query input device");
		TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		if(close(LinuxCon.FD) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to close controller device!");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		}
		return false;
	}

	//Ensure this device supports the events expected of a controller
	if((EVBits[EV_ABS / 8u] & (1u << (EV_ABS % 8u))) == 0)
	{
		if(close(LinuxCon.FD) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to close controller device!");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		}
		return false;
	}

	std::string name;
	name.resize(256);
	if (ioctl(LinuxCon.FD, EVIOCGNAME(name.size()), name.data()) < 0)
		name = "Unknown";

	std::string guid;
	guid.resize(32);
	//Generate a controller GUID that matches the SDL 2.0.5+ one
	if((ID.vendor != 0u) && (ID.product != 0u) && (ID.version != 0u))
	{
		if(sprintf(guid.data(), "%02x%02x0000%02x%02x0000%02x%02x0000%02x%02x0000",
			ID.bustype & 0xFFu, ID.bustype >> 8u,
			ID.vendor & 0xFFu, ID.vendor >> 8u,
			ID.product & 0xFFu, ID.product >> 8u,
			ID.version & 0xFFu, ID.version >> 8u) < 0)
		{
			return false;
		}
	}
	else
	{
		if(sprintf(guid.data(), "%02x%02x0000%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x00",
			ID.bustype & 0xFFu, ID.bustype >> 8u,
			name[0], name[1], name[2], name[3],
			name[4], name[5], name[6], name[7],
			name[8], name[9], name[10]) < 0)
		{
			return false;
		}
	}

	std::erase(name, '\0');

	i32 axisCount = 0, buttonCount = 0, dpadCount = 0;
	for(u32 code = BTN_MISC; code < KEY_CNT; code++)
	{
		if((keyBits[code / 8u] & (1u << (code % 8u))) == 0u)
			continue;

		LinuxCon.KeyMap[code - BTN_MISC] = buttonCount;
		buttonCount++;
	}

	for(u32 code = 0; code < ABS_CNT; code++)
	{
		LinuxCon.ABSMap[code] = -1;
		if((ABSBits[code / 8u] & (1u << (code % 8u))) == 0u)
			continue;

		if(code >= ABS_HAT0X && code <= ABS_HAT3Y)
		{
			LinuxCon.ABSMap[code] = dpadCount;
			dpadCount++;
			//Skip the Y axis
			code++;
		}
		else
		{
			if (ioctl(LinuxCon.FD, EVIOCGABS(code), &LinuxCon.ABSInfo[code]) < 0)
				continue;

			LinuxCon.ABSMap[code] = axisCount;
			axisCount++;
		}
	}

	ControllerInternal* const con = AddInternalController(name, guid, NumericCast<u32>(axisCount),
	                                                      NumericCast<u32>(buttonCount),
														  NumericCast<u32>(dpadCount));
	if(con == nullptr)
	{
		if(close(LinuxCon.FD) < 0)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to close controller device!");
			TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
		}
		return false;
	}

	LinuxCon.Path = std::move(path);
	con->LinuxCon = LinuxCon;

	PollABSStateLinux(con);

	if (!s_eventCallback)
		return false;

	//Get index of our ControllerInternal
	u32 index = 0;
	for (index = 0; index <= std::to_underlying(Controller::Sixteen); index++)
	{
		if (&s_controllerInternal[index] == con)
			break;
	}

	Events::ControllerConnectEvent event(static_cast<Controller>(index));
	s_eventCallback(event);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Frees all resources associated with the specified controller
void TRAP::Input::CloseController(Controller controller)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	ControllerInternal* const con = &s_controllerInternal[std::to_underlying(controller)];

	if(close(con->LinuxCon.FD) < 0)
	{
		TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to close controller device!");
		TP_ERROR(Log::InputControllerLinuxPrefix, Utils::String::GetStrError());
	}

	const bool connected = con->Connected;

	if(connected)
	{
		TP_INFO(Log::InputControllerPrefix, "Controller: ",
		        (con->mapping != nullptr
			        ? con->mapping->Name
			        : con->Name),
		        " (", std::to_underlying(controller), ") disconnected!");
	}

	*con = {};

	if (!s_eventCallback)
		return;

	if (connected)
	{
		Events::ControllerDisconnectEvent event(static_cast<Controller>(controller));
		s_eventCallback(event);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::DetectControllerConnectionLinux()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if (s_linuxController.INotify <= 0)
		return;

	ssize_t offset = 0;
	std::array<char, 16384> buffer{};
	const ssize_t size = read(s_linuxController.INotify, buffer.data(), buffer.size());

	while(size > offset)
	{
		const inotify_event* const e = reinterpret_cast<const inotify_event*>(&buffer[NumericCast<usize>(offset)]); //Must use reinterpret_cast because of flexible array member

		offset += NumericCast<ssize_t>(sizeof(inotify_event)) + e->len;

		if(!std::regex_match(e->name, s_linuxController.Regex))
			continue;

		const std::filesystem::path path = std::filesystem::path("/dev/input") / e->name;

		if ((e->mask & (IN_CREATE | IN_ATTRIB)) != 0u)
			OpenControllerDeviceLinux(path);
		else if((e->mask & IN_DELETE) != 0u)
		{
			for(u32 cID = 0; cID <= std::to_underlying(Controller::Sixteen); cID++)
			{
				if(s_controllerInternal[cID].LinuxCon.Path == path)
				{
					CloseController(static_cast<Controller>(cID));
					break;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::PollController(Controller controller, [[maybe_unused]] PollMode mode)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	if(s_controllerInternal[std::to_underlying(controller)].Connected)
	{
		ControllerInternal* const con = &s_controllerInternal[std::to_underlying(controller)];

		//Read all queued events (non-blocking)
		while(true)
		{
			input_event e{};

			errno = 0;
			if(read(con->LinuxCon.FD, &e, sizeof(e)) < 0)
			{
				//Reset the controller slot if the device was disconnected
				if (errno == ENODEV)
					CloseController(controller);

				break;
			}

			if(e.type == EV_SYN)
			{
				if (e.code == SYN_DROPPED)
					s_linuxController.Dropped = true;
				else if(e.code == SYN_REPORT)
				{
					s_linuxController.Dropped = false;
					PollABSStateLinux(con);
				}
			}

			if (s_linuxController.Dropped)
				continue;

			if (e.type == EV_KEY)
				HandleKeyEventLinux(con, e.code, e.value);
			else if (e.type == EV_ABS)
				HandleABSEventLinux(con, e.code, e.value);
		}
	}

	return s_controllerInternal[std::to_underlying(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

//Poll state of absolute axes
void TRAP::Input::PollABSStateLinux(ControllerInternal* const con)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	for (u32 code = 0; code < ABS_CNT; code++)
	{
		if (con->LinuxCon.ABSMap[code] < 0)
			continue;

		const input_absinfo* const info = &con->LinuxCon.ABSInfo[code];

		if (ioctl(con->LinuxCon.FD, EVIOCGABS(code), info) < 0)
			continue;

		HandleABSEventLinux(con, code, info->value);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Apply an EV_ABS event to the specified controller
void TRAP::Input::HandleABSEventLinux(ControllerInternal* const con, u32 code, i32 value)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	const i32 index = con->LinuxCon.ABSMap[code];

	if (code >= ABS_HAT0X && code <= ABS_HAT3Y)
	{
		static constexpr std::array<std::array<u8, 3>, 3> stateMap =
		{
			{
				{
					NumericCast<u8>(std::to_underlying(ControllerDPad::Centered)),
					NumericCast<u8>(std::to_underlying(ControllerDPad::Up)),
					NumericCast<u8>(std::to_underlying(ControllerDPad::Down))
				},
				{
					NumericCast<u8>(std::to_underlying(ControllerDPad::Left)),
					NumericCast<u8>(std::to_underlying(ControllerDPad::Left_Up)),
					NumericCast<u8>(std::to_underlying(ControllerDPad::Left_Down))
				},
				{
					NumericCast<u8>(std::to_underlying(ControllerDPad::Right)),
					NumericCast<u8>(std::to_underlying(ControllerDPad::Right_Up)),
					NumericCast<u8>(std::to_underlying(ControllerDPad::Right_Down))
				}
			}
		};

		const u32 dpad = (code - ABS_HAT0X) / 2u;
		const u32 axis = (code - ABS_HAT0X) % 2u;
		std::array<i32, 4>& state = con->LinuxCon.DPads[dpad];

		//NOTE: Looking at several input drivers, it seems all DPad events use
		//-1 for left / up, 0 for centered and 1 for right / down
		if (value == 0)
			state[axis] = 0;
		else if (value < 0)
			state[axis] = 1;
		else if (value > 0)
			state[axis] = 2;

		InternalInputControllerDPad(con, index, stateMap[NumericCast<usize>(state[0u])][NumericCast<usize>(state[1u])]);
	}
	else
	{
		const input_absinfo* const info = &con->LinuxCon.ABSInfo[code];
		f32 normalized = NumericCast<f32>(value);

		const i32 range = info->maximum - info->minimum;
		if (range != 0)
		{
			//Normalize to 0.0f -> 1.0f
			normalized = (normalized - NumericCast<f32>(info->minimum)) / NumericCast<f32>(range);
			//Normalize to -1.0f -> 1.0f
			normalized = normalized * 2.0f - 1.0f;
		}

		InternalInputControllerAxis(con, index, normalized);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::HandleKeyEventLinux(ControllerInternal* const con, u32 code, i32 value)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	InternalInputControllerButton(con, con->LinuxCon.KeyMap[code - BTN_MISC], value != 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerGUID([[maybe_unused]] std::string& guid)
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None &&
	                                        (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::optional<std::string> TRAP::Input::GetKeyboardLayoutName()
{
	ZoneNamedC(__tracy, tracy::Color::Gold, (GetTRAPProfileSystems() & ProfileSystems::Input) != ProfileSystems::None);

	return TRAP::INTERNAL::WindowingAPI::GetLinuxKeyboardLayoutName();
}

#endif /*TRAP_PLATFORM_LINUX*/

#endif /*TRAP_HEADLESS_MODE*/

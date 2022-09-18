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

#ifdef TRAP_PLATFORM_LINUX

#define TRAP_BUILD_LINUX_MAPPINGS

#include "Events/ControllerEvent.h"
#include "Window/WindowingAPI.h"
#include "ControllerMappings.h"
#include "Utils/Utils.h"
#include "Utils/String/String.h"

TRAP::Input::ControllerLinuxLibrary TRAP::Input::s_linuxController{};

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::InitController()
{
	constexpr std::string_view dirName = "/dev/input";

	s_linuxController.INotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
	if(s_linuxController.INotify > 0)
	{
		//HACK: Register for IN_ATTRIB to get notified when udev is done
		//This works well in practice but the true way is libudev
		s_linuxController.Watch = inotify_add_watch(s_linuxController.INotify, dirName.data(),
		                                            IN_CREATE | IN_ATTRIB | IN_DELETE);
	}

	//Continue without device connection notifications if inotify fails
	if(regcomp(&s_linuxController.Regex, "^event[0-9]\\+$", 0) != 0)
	{
		TP_ERROR(Log::InputControllerLinuxPrefix, "Could not compile regex!");
		return false;
	}

	int32_t count = 0;

	DIR* const dir = opendir(dirName.data());
	if(dir)
	{
		const dirent* entry = nullptr;

		while((entry = readdir(dir)))
		{
			regmatch_t match;

			if (regexec(&s_linuxController.Regex, entry->d_name, 1, &match, 0) != 0)
				continue;

			const std::filesystem::path path = std::filesystem::path(dirName) / entry->d_name;

			if (OpenControllerDeviceLinux(path))
				count++;
		}

		closedir(dir);
	}

	//Continue with no controllers if enumeration fails
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::ShutdownController()
{
	for(uint8_t cID = 0; cID <= static_cast<uint8_t>(Controller::Sixteen); cID++)
	{
		if(s_controllerInternal[cID].LinuxCon.CurrentVibration != -1)
			SetControllerVibration(static_cast<Controller>(cID), 0.0f, 0.0f);
		if (s_controllerInternal[cID].Connected)
			CloseController(static_cast<Controller>(cID));
	}

	regfree(&s_linuxController.Regex);

	if(s_linuxController.INotify > 0)
	{
		if (s_linuxController.Watch > 0)
			inotify_rm_watch(s_linuxController.INotify, s_linuxController.Watch);

		close(s_linuxController.INotify);
	}

	s_controllerInternal = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::SetControllerVibrationInternal(Controller controller, const float leftMotor, const float rightMotor)
{
	if(!PollController(controller, PollMode::Presence))
		return;

	ControllerInternal* const con = &s_controllerInternal[static_cast<uint8_t>(controller)];

	if(!con->LinuxCon.VibrationSupported)
		return;

	input_event play{};
	//Delete any existing effect
	if(con->LinuxCon.CurrentVibration != -1)
	{
		//Stop the effect
		play.type = EV_FF;
		play.code = con->LinuxCon.CurrentVibration;
		play.value = 0;

		if(write(con->LinuxCon.FD, static_cast<const void*>(&play), sizeof(play)) == -1)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to stop vibration");
			return;
		}

		//Delete the effect
		ioctl(con->LinuxCon.FD, EVIOCRMFF, con->LinuxCon.CurrentVibration);
	}

	//If VibrationSupported is true, start the new effect
	if(leftMotor != 0.0f || rightMotor != 0.0f)
	{
		ff_effect ff{};

		//Define an effect for this vibration setting
		ff.type = FF_RUMBLE;
		ff.id = -1;
		ff.u.rumble.strong_magnitude = static_cast<uint16_t>(leftMotor * 65535);
		ff.u.rumble.weak_magnitude = static_cast<uint16_t>(rightMotor * 65535);
		ff.replay.length = 65535;
		ff.replay.delay = 0;

		//Upload the effect
		if(ioctl(con->LinuxCon.FD, EVIOCSFF, &ff) != -1)
			con->LinuxCon.CurrentVibration = ff.id;

		//Play the effect
		play.type = EV_FF;
		play.code = con->LinuxCon.CurrentVibration;
		play.value = 1;

		if(write(con->LinuxCon.FD, static_cast<const void*>(&play), sizeof(play)) == -1)
		{
			TP_ERROR(Log::InputControllerLinuxPrefix, "Failed to start vibration");
			return;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Input::ControllerBatteryStatus TRAP::Input::GetControllerBatteryStatusInternal(Controller /*controller*/)
{
	return ControllerBatteryStatus::Wired;
}

//-------------------------------------------------------------------------------------------------------------------//

//Attempt to open the specified controller device
bool TRAP::Input::OpenControllerDeviceLinux(const std::filesystem::path path)
{
	for(uint8_t cID = 0; cID <= static_cast<uint8_t>(Controller::Sixteen); cID++)
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

	if (LinuxCon.FD == -1)
		return false;

	const std::array<char, (EV_CNT + 7) / 8> EVBits{};
	const std::array<char, (KEY_CNT + 7) / 8> keyBits{};
	const std::array<char, (ABS_CNT + 7) / 8> ABSBits{};
	input_id ID{};

	if (ioctl(LinuxCon.FD, EVIOCGBIT(0, EVBits.size()), EVBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGBIT(EV_KEY, keyBits.size()), keyBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGBIT(EV_ABS, ABSBits.size()), ABSBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGID, &ID) < 0)
	{
		TP_ERROR(Log::InputControllerLinuxPrefix, "Could not query input device: ", Utils::String::GetStrError(), "!");
		close(LinuxCon.FD);
		return false;
	}

	//Ensure this device supports the events expected of a controller
	if(!(EVBits[EV_ABS / 8] & (1 << (EV_ABS % 8))))
	{
		close(LinuxCon.FD);
		return false;
	}

	std::string name;
	name.resize(256);
	if (ioctl(LinuxCon.FD, EVIOCGNAME(name.size()), name.data()) < 0)
		name = "Unknown";

	std::string guid;
	guid.resize(32);
	//Generate a controller GUID that matches the SDL 2.0.5+ one
	if(ID.vendor && ID.product && ID.version)
	{
		sprintf(guid.data(), "%02x%02x0000%02x%02x0000%02x%02x0000%02x%02x0000",
			ID.bustype & 0xFF, ID.bustype >> 8,
			ID.vendor & 0xFF, ID.vendor >> 8,
			ID.product & 0xFF, ID.product >> 8,
			ID.version & 0xFF, ID.version >> 8);
	}
	else
	{
		sprintf(guid.data(), "%02x%02x0000%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x00",
			ID.bustype & 0xFF, ID.bustype >> 8,
			name[0], name[1], name[2], name[3],
			name[4], name[5], name[6], name[7],
			name[8], name[9], name[10]);
	}

	name.erase(std::find(name.begin(), name.end(), '\0'), name.end());

	int axisCount = 0, buttonCount = 0, dpadCount = 0;
	for(int32_t code = BTN_MISC; code < KEY_CNT; code++)
	{
		if(!(keyBits[code / 8] & (1 << (code % 8))))
			continue;

		LinuxCon.KeyMap[code - BTN_MISC] = buttonCount;
		buttonCount++;
	}

	for(int32_t code = 0; code < ABS_CNT; code++)
	{
		LinuxCon.ABSMap[code] = -1;
		if(!(ABSBits[code / 8] & (1 << (code % 8))))
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

	ControllerInternal* const con = AddInternalController(name, guid, axisCount, buttonCount, dpadCount);
	if(!con)
	{
		close(LinuxCon.FD);
		return false;
	}

	LinuxCon.Path = std::move(path);
	con->LinuxCon = LinuxCon;

	PollABSStateLinux(con);

	if (!s_eventCallback)
		return false;

	//Get index of our ControllerInternal
	uint8_t index = 0;
	for (index = 0; index <= static_cast<uint8_t>(Controller::Sixteen); index++)
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
	ControllerInternal* const con = &s_controllerInternal[static_cast<uint8_t>(controller)];

	close(con->LinuxCon.FD);

	const bool connected = con->Connected;

	if(connected)
	{
		TP_INFO(Log::InputControllerPrefix, "Controller: ",
		        (con->mapping
			        ? con->mapping->Name
			        : con->Name),
		        " (", static_cast<uint32_t>(controller), ") disconnected!");
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
	if (s_linuxController.INotify <= 0)
		return;

	ssize_t offset = 0;
	std::array<char, 16384> buffer{};
	const ssize_t size = read(s_linuxController.INotify, buffer.data(), buffer.size());

	while(size > offset)
	{
		regmatch_t match;
		const inotify_event* const e = reinterpret_cast<const inotify_event*>(&buffer[offset]); //Must use reinterpret_cast because of flexible array member

		offset += static_cast<ssize_t>(sizeof(inotify_event)) + e->len;

		if (regexec(&s_linuxController.Regex, e->name, 1, &match, 0) != 0)
			continue;

		const std::filesystem::path path = std::filesystem::path("/dev/input") / e->name;

		if (e->mask & (IN_CREATE | IN_ATTRIB))
			OpenControllerDeviceLinux(path);
		else if(e->mask & IN_DELETE)
		{
			for(uint8_t cID = 0; cID <= static_cast<uint8_t>(Controller::Sixteen); cID++)
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

bool TRAP::Input::PollController(Controller controller, PollMode)
{
	if(s_controllerInternal[static_cast<uint8_t>(controller)].Connected)
	{
		ControllerInternal* const con = &s_controllerInternal[static_cast<uint8_t>(controller)];

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

	return s_controllerInternal[static_cast<uint8_t>(controller)].Connected;
}

//-------------------------------------------------------------------------------------------------------------------//

//Poll state of absolute axes
void TRAP::Input::PollABSStateLinux(ControllerInternal* const con)
{
	for (int32_t code = 0; code < ABS_CNT; code++)
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
void TRAP::Input::HandleABSEventLinux(ControllerInternal* const con, int32_t code, int32_t value)
{
	const int32_t index = con->LinuxCon.ABSMap[code];

	if (code >= ABS_HAT0X && code <= ABS_HAT3Y)
	{
		static constexpr std::array<std::array<uint8_t, 3>, 3> stateMap =
		{
			{
				{
					static_cast<uint8_t>(ControllerDPad::Centered),
					static_cast<uint8_t>(ControllerDPad::Up),
					static_cast<uint8_t>(ControllerDPad::Down)
				},
				{
					static_cast<uint8_t>(ControllerDPad::Left),
					static_cast<uint8_t>(ControllerDPad::Left_Up),
					static_cast<uint8_t>(ControllerDPad::Left_Down)
				},
				{
					static_cast<uint8_t>(ControllerDPad::Right),
					static_cast<uint8_t>(ControllerDPad::Right_Up),
					static_cast<uint8_t>(ControllerDPad::Right_Down)
				}
			}
		};

		const int32_t dpad = (code - ABS_HAT0X) / 2;
		const int32_t axis = (code - ABS_HAT0X) % 2;
		int32_t* const state = con->LinuxCon.DPads[dpad].data();

		//NOTE: Looking at several input drivers, it seems all DPad events use
		//-1 for left / up, 0 for centered and 1 for right / down
		if (value == 0)
			state[axis] = 0;
		else if (value < 0)
			state[axis] = 1;
		else if (value > 0)
			state[axis] = 2;

		InternalInputControllerDPad(con, index, stateMap[state[0]][state[1]]);
	}
	else
	{
		const input_absinfo* const info = &con->LinuxCon.ABSInfo[code];
		float normalized = static_cast<float>(value);

		const int range = info->maximum - info->minimum;
		if (range)
		{
			//Normalize to 0.0f -> 1.0f
			normalized = (normalized - static_cast<float>(info->minimum)) / static_cast<float>(range);
			//Normalize to -1.0f -> 1.0f
			normalized = normalized * 2.0f - 1.0f;
		}

		InternalInputControllerAxis(con, index, normalized);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::HandleKeyEventLinux(ControllerInternal* const con, int32_t code, int32_t value)
{
	if(code - BTN_MISC >= 0)
		InternalInputControllerButton(con, con->LinuxCon.KeyMap[code - BTN_MISC], value);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerGUID(std::string&)
{
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Input::GetKeyboardLayoutName()
{
	return TRAP::INTERNAL::WindowingAPI::GetX11KeyboardLayoutName();
}

#endif
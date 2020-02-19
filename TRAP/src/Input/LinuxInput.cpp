#include "TRAPPCH.h"
#include "Input/Input.h"

#ifdef TRAP_PLATFORM_LINUX

#include "Event/ControllerEvent.h"
#include "Utils/ControllerMappings.h"

TRAP::Input::ControllerLinuxLibrary TRAP::Input::s_linuxController{};

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Input::InitController()
{
	for (int32_t i = 0; i < TRAP::Embed::ControllerMappings.size(); i++)
		UpdateControllerMappings(TRAP::Embed::ControllerMappings[i]);

	const char* dirName = "/dev/input";

	s_linuxController.INotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
	if(s_linuxController.INotify > 0)
	{
		//HACK: Register for IN_ATTRIB to get notified when udev is done
		//This works well in practice but the true way is libudev
		s_linuxController.Watch = inotify_add_watch(s_linuxController.INotify, dirName, IN_CREATE | IN_ATTRIB | IN_DELETE);
	}

	//Continue without device connection notifications if inotify fails
	if(regcomp(&s_linuxController.Regex, "^event[0-9]\\+$", 0) != 0)
	{
		TP_ERROR("[Input][Controller][Linux] Could not compile regex!");
		return false;
	}

	int32_t count = 0;

	DIR* dir = opendir(dirName);
	if(dir)
	{
		struct dirent* entry;

		while((entry = readdir(dir)))
		{
			regmatch_t match;

			if (regexec(&s_linuxController.Regex, entry->d_name, 1, &match, 0) != 0)
				continue;

			std::string path = dirName;
			path += '/';
			path += entry->d_name;

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

void TRAP::Input::SetControllerVibrationInternal(Controller controller, float leftMotor, float rightMotor)
{
	if(!PollController(controller, 0))
		return;
		
	ControllerInternal* con = &s_controllerInternal[static_cast<uint8_t>(controller)];
	if(con->LinuxCon.VibrationSupported)
	{
		struct input_event play;
		//Delete any existing effect
		if(con->LinuxCon.CurrentVibration != -1)
		{
			//Stop the effect
			play.type = EV_FF;
			play.code = con->LinuxCon.CurrentVibration;
			play.value = 0;
		
			write(con->LinuxCon.FD, (const void*)&play, sizeof(play));
		
			//Delete the effect
			ioctl(con->LinuxCon.FD, EVIOCRMFF, con->LinuxCon.CurrentVibration);
		}
	
		//If VibrationSupported is true, start the new effect
		if(leftMotor != 0.0f || rightMotor != 0.0f)
		{
			struct ff_effect ff;
		
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
		
			write(con->LinuxCon.FD, (const void*)&play, sizeof(play));
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Attempt to open the specified controller device
bool TRAP::Input::OpenControllerDeviceLinux(const std::string& path)
{
	for(uint8_t cID = 0; cID <= static_cast<uint8_t>(Controller::Sixteen); cID++)
	{
		if (!s_controllerInternal[cID].Connected)
			continue;
		if (s_controllerInternal[cID].LinuxCon.Path == path)
			return false;
	}

	ControllerLinux LinuxCon = {};
	LinuxCon.FD = open(path.data(), O_RDWR | O_NONBLOCK); //O_RDWR is needed for vibrations
	if(LinuxCon.FD != -1)
		LinuxCon.VibrationSupported = true;
		
	if(errno == EACCES)
		LinuxCon.FD = open(path.data(), O_RDONLY | O_NONBLOCK);
	
	if (LinuxCon.FD == -1)
		return false;

	std::array<char, (EV_CNT + 7) / 8> EVBits{};
	std::array<char, (KEY_CNT + 7) / 8> keyBits{};
	std::array<char, (ABS_CNT + 7) / 8> ABSBits{};
	struct input_id ID;

	if (ioctl(LinuxCon.FD, EVIOCGBIT(0, EVBits.size()), EVBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGBIT(EV_KEY, keyBits.size()), keyBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGBIT(EV_ABS, ABSBits.size()), ABSBits.data()) < 0 ||
		ioctl(LinuxCon.FD, EVIOCGID, &ID) < 0)
	{
		TP_ERROR("[Input][Controller][Linux] Could not query input device: ", strerror(errno), "!");
		close(LinuxCon.FD);
		return false;
	}

	//Ensure this device supports the events expected of a controller
	if(!(EVBits[EV_KEY / 8] & (1 << (EV_KEY % 8))) || !(EVBits[EV_ABS / 8] & (1 << (EV_ABS % 8))))
	{
		close(LinuxCon.FD);
		return false;
	}

	std::string name;
	name.resize(256);
	if (ioctl(LinuxCon.FD, EVIOCGNAME(name.size()), name.data()) < 0)
		name = "Unknown";

	std::string guid;
	guid.resize(33);
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

	ControllerInternal* con = AddInternalController(name, guid, axisCount, buttonCount, dpadCount);
	if(!con)
	{
		close(LinuxCon.FD);
		return false;
	}

	LinuxCon.Path = path;
	std::memcpy(&con->LinuxCon, &LinuxCon, sizeof(LinuxCon));

	PollABSStateLinux(con);

	uint8_t cID;
	int8_t cIDUsable = -1;
	for (cID = 0; cID <= static_cast<uint8_t>(Controller::Sixteen); cID++)
		if (!s_controllerInternal[cID].Connected)
		{
			cIDUsable = cID;
			break;
		}

	if(cIDUsable != -1)
	{
		if (!s_eventCallback)
			return false;

		ControllerConnectEvent event(static_cast<Controller>(cIDUsable));
		s_eventCallback(event);
		
		return false;
	}
	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

//Frees all resources associated with the specified controller
void TRAP::Input::CloseController(Controller controller)
{
	ControllerInternal* con = &s_controllerInternal[static_cast<uint8_t>(controller)];
	
	close(con->LinuxCon.FD);

	bool connected = con->Connected;

	if(connected)
		TP_INFO("[Input][Controller] Controller: ",
		        (con->mapping
			        ? con->mapping->Name
			        : con->Name),
		        " (", static_cast<uint32_t>(controller), ") Disconnected!");

	*con = {};
	
	if (!s_eventCallback)
		return;

	if (connected)
	{
		ControllerDisconnectEvent event(static_cast<Controller>(controller));
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
		const struct inotify_event* e = (struct inotify_event*)(&buffer[offset]);

		offset += sizeof(struct inotify_event) + e->len;

		if (regexec(&s_linuxController.Regex, e->name, 1, &match, 0) != 0)
			continue;

		std::string path = "/dev/input/";
		path += e->name;

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

bool TRAP::Input::PollController(Controller controller, int32_t mode)
{
	if(s_controllerInternal[static_cast<uint8_t>(controller)].Connected)
	{
		ControllerInternal* con = &s_controllerInternal[static_cast<uint8_t>(controller)];
	
		//Read all queued events (non-blocking)
		for(;;)
		{
			struct input_event e;

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
void TRAP::Input::PollABSStateLinux(ControllerInternal* con)
{
	for (int32_t code = 0; code < ABS_CNT; code++)
	{
		if (con->LinuxCon.ABSMap[code] < 0)
			continue;

		struct input_absinfo* info = &con->LinuxCon.ABSInfo[code];

		if (ioctl(con->LinuxCon.FD, EVIOCGABS(code), info) < 0)
			continue;

		HandleABSEventLinux(con, code, info->value);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//Apply an EV_ABS event to the specified controller
void TRAP::Input::HandleABSEventLinux(ControllerInternal* con, int32_t code, int32_t value)
{
	const int32_t index = con->LinuxCon.ABSMap[code];

	if (code >= ABS_HAT0X && code <= ABS_HAT3Y)
	{
		static const std::array<std::array<uint8_t, 3>, 3> stateMap =
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
		int32_t* state = con->LinuxCon.DPads[dpad].data();

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
		const struct input_absinfo* info = &con->LinuxCon.ABSInfo[code];
		float normalized = value;

		const int range = info->maximum - info->minimum;
		if (range)
		{
			//Normalize to 0.0f -> 1.0f
			normalized = (normalized - info->minimum) / range;
			//Normalize to -1.0f -> 1.0f
			normalized = normalized * 2.0f - 1.0f;
		}

		InternalInputControllerAxis(con, index, normalized);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::HandleKeyEventLinux(ControllerInternal* con, int32_t code, int32_t value)
{
	if(code - BTN_MISC >= 0)
		InternalInputControllerButton(con, con->LinuxCon.KeyMap[code - BTN_MISC], value ? true : false);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Input::UpdateControllerGUID(std::string& guid)
{
}

#endif
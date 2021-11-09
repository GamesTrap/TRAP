#include "TRAPPCH.h"
#include "Utils.h"

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Application.h"

std::string TRAP::Utils::UUIDToString(const std::array<uint8_t, 16>& uuid)
{
	std::stringstream s;

	s << std::hex << std::setfill('0')
	  << std::setw(2) << static_cast<int32_t>(uuid[0])
	  << std::setw(2) << static_cast<int32_t>(uuid[1])
	  << std::setw(2) << static_cast<int32_t>(uuid[2])
	  << std::setw(2) << static_cast<int32_t>(uuid[3])
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(uuid[4])
	  << std::setw(2) << static_cast<int32_t>(uuid[5])
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(uuid[6])
	  << std::setw(2) << static_cast<int32_t>(uuid[7])
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(uuid[8])
	  << std::setw(2) << static_cast<int32_t>(uuid[9])
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(uuid[10])
	  << std::setw(2) << static_cast<int32_t>(uuid[11])
	  << std::setw(2) << static_cast<int32_t>(uuid[12])
	  << std::setw(2) << static_cast<int32_t>(uuid[13])
	  << std::setw(2) << static_cast<int32_t>(uuid[14])
	  << std::setw(2) << static_cast<int32_t>(uuid[15]);

	return s.str();
}

//-------------------------------------------------------------------------------------------------------------------//

std::array<uint8_t, 16> TRAP::Utils::UUIDFromString(const std::string_view uuid)
{
	uint8_t digit = 0;
	bool firstDigit = true;
	std::size_t index = 0;

	if (uuid.empty())
		return {};

	std::array<uint8_t, 16> result{};
	for (uint8_t i : uuid)
	{
		if (i == '-')
			continue;

		if (index >= 16 || !std::isxdigit(i))
			return {};

		if(firstDigit)
		{
			digit = i;
			firstDigit = false;
		}
		else
		{
			uint8_t charDigit;
			uint8_t uuidDigit;

			if (digit >= '0' && digit <= '9')
				charDigit = digit - '0';
			else if (digit >= 'a' && digit <= 'f')
				charDigit = 10 + digit - 'a';
			else if (digit >= 'A' && digit <= 'F')
				charDigit = 10 + digit - 'A';
			else
				charDigit = 0;

			if (i >= '0' && i <= '9')
				uuidDigit = i - '0';
			else if (i >= 'a' && i <= 'f')
				uuidDigit = 10 + i - 'a';
			else if (i >= 'A' && i <= 'F')
				uuidDigit = 10 + i - 'A';
			else
				uuidDigit = 0;
			result[index] = (static_cast<uint8_t>((charDigit << 4)) | uuidDigit);
			index++;
			firstDigit = true;
		}
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Endian TRAP::Utils::GetEndian()
{
	//Check if machine is using little-endian or big-endian
	int32_t intVal = 1;
	uint8_t* uVal = reinterpret_cast<uint8_t*>(&intVal);
#if __cplusplus > 201703L
	static Endian endian = static_cast<Endian>(std::endian::native == std::endian::little);
#else
	static Endian endian = static_cast<Endian>(uVal[0] == 1);
#endif

	return endian;
}

//-------------------------------------------------------------------------------------------------------------------//

const TRAP::Utils::CPUInfo& TRAP::Utils::GetCPUInfo()
{
	static CPUInfo cpu{};

	if(!cpu.Model.empty())
		return cpu;

	auto CPUID = [](const uint32_t funcID, const uint32_t subFuncID)->std::array <uint32_t, 4>
	{
		std::array<uint32_t, 4> regs{};
	#ifdef TRAP_PLATFORM_WINDOWS
		__cpuidex(reinterpret_cast<int32_t*>(regs.data()), static_cast<int32_t>(funcID),
		          static_cast<int32_t>(subFuncID));
	#else
		asm volatile
			("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
				: "a" (funcID), "c" (subFuncID));
	#endif

		return regs;
	};

	std::array<uint32_t, 4> regs = CPUID(0, 0);
	const uint32_t HFS = regs[0];
	//Get Vendor
	const std::string vendorID = std::string(reinterpret_cast<const char*>(&regs[1]), 4) +
		                         std::string(reinterpret_cast<const char*>(&regs[3]), 4) +
		                         std::string(reinterpret_cast<const char*>(&regs[2]), 4);
	regs = CPUID(1, 0);
	cpu.HyperThreaded = regs[3] & 0x10000000; //Get Hyper-threading

	const std::string upVendorID = Utils::String::ToUpper(vendorID);
	//Get Number of cores
	constexpr int32_t MAX_INTEL_TOP_LVL = 4;
	constexpr uint32_t LVL_TYPE = 0x0000FF00;
	constexpr uint32_t LVL_CORES = 0x0000FFFF;
	if (upVendorID.find("INTEL") != std::string::npos)
	{
		if (HFS >= 11)
		{
			int32_t numSMT = 0;
			for (int32_t lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl)
			{
				std::array<uint32_t, 4> regs1 = CPUID(0x0B, lvl);
				const uint32_t currentLevel = (LVL_TYPE & regs1[2]) >> 8;
				switch (currentLevel)
				{
				case BIT(0):
					numSMT = LVL_CORES & regs1[1];
					break;

				case BIT(1):
					cpu.LogicalCores = LVL_CORES & regs1[1];
					break;

				default:
					break;
				}
			}
			cpu.Cores = cpu.LogicalCores / numSMT;
		}
		else
		{
			if (HFS >= 1)
			{
				cpu.LogicalCores = (regs[1] >> 16) & 0xFF;
				if (HFS >= 4)
				{
					std::array<uint32_t, 4> regs1 = CPUID(4, 0);
					cpu.Cores = (1 + (regs1[0] >> 26)) & 0x3F;
				}
			}
			if (cpu.HyperThreaded)
			{
				if (!(cpu.Cores > 1))
				{
					cpu.Cores = 1;
					cpu.LogicalCores = (cpu.LogicalCores >= 2 ? cpu.LogicalCores : 2);
				}
			}
			else
				cpu.Cores = cpu.LogicalCores = 1;
		}
	}
	else if (upVendorID.find("AMD") != std::string::npos)
	{
		uint32_t extFamily;
		if (((regs[0] >> 8) & 0xF) < 0xF)
			extFamily = (regs[0] >> 8) & 0xF;
		else
			extFamily = ((regs[0] >> 8) & 0xF) + ((regs[0] >> 20) & 0xFF);

		if (HFS >= 1)
		{
			cpu.LogicalCores = (regs[1] >> 16) & 0xFF;
			std::array<uint32_t, 4> regs1 = CPUID(0x80000000, 0);
			if (regs1[0] >= 8)
			{
				regs1 = CPUID(0x80000008, 0);
				cpu.Cores = 1 + (regs1[2] & 0xFF);
			}
		}
		if (cpu.HyperThreaded)
		{
			if (!(cpu.Cores > 1))
			{
				cpu.Cores = 1;
				cpu.LogicalCores = (cpu.LogicalCores >= 2 ? cpu.LogicalCores : 2);
			}
			else if (cpu.Cores > 1)
			{
				//Ryzen 3 has SMT flag, but in fact cores count is equal to threads count.
				//Ryzen 5/7 reports twice as many "real" cores (e.g. 16 cores instead of 8) because of SMT.
				//On PPR 17h, page 82:
				//CPUID_Fn8000001E_EBX [Core Identifiers][15:8] is ThreadsPerCore
				//ThreadsPerCore: [...] The number of threads per core is ThreadsPerCore + 1
				std::array<uint32_t, 4> regs1 = CPUID(0x80000000, 0);
				if ((extFamily >= 23) && (regs1[0] >= 30))
				{
					regs1 = CPUID(0x8000001E, 0);
					cpu.Cores /= ((regs1[1] >> 8) & 0xFF) + 1;
				}
			}
		}
		else
			cpu.Cores = cpu.LogicalCores = 1;
	}

	//Get CPU brand string
	for (uint32_t i = 0x80000002; i < 0x80000005; ++i)
	{
		std::array<uint32_t, 4> regs1 = CPUID(i, 0);
		cpu.Model += std::string(reinterpret_cast<const char*>(&regs1[0]), 4);
		cpu.Model += std::string(reinterpret_cast<const char*>(&regs1[1]), 4);
		cpu.Model += std::string(reinterpret_cast<const char*>(&regs1[2]), 4);
		cpu.Model += std::string(reinterpret_cast<const char*>(&regs1[3]), 4);
	}

	return cpu;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::LinuxWindowManager TRAP::Utils::GetLinuxWindowManager()
{
	static LinuxWindowManager windowManager{};

	#ifdef TRAP_PLATFORM_LINUX
	if(windowManager != LinuxWindowManager::Unknown)
		return windowManager;

	std::string wl = "wayland";
	std::string x = "x11";
	std::string session;
	if(std::getenv("XDG_SESSION_TYPE"))
		session = std::getenv("XDG_SESSION_TYPE");
	if (std::getenv("WAYLAND_DISPLAY") || session == wl)
		windowManager = LinuxWindowManager::Wayland;
	else if (std::getenv("DISPLAY") || session == x)
		windowManager = LinuxWindowManager::X11;
	else
	{
		Utils::Dialogs::ShowMsgBox("Unsupported window manager", "Window manager is unsupported!\n"
									u8"TRAP™ currently only supports X11\n"
									//u8"TRAP™ uses X11 or Wayland\n"
									"Make sure the appropriate environment variable(s) is/are set!\n"
									"Error code: 0x0008",
									Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::EngineLinuxPrefix, "Unsupported window manager!");
		TRAP::Application::Shutdown();
		exit(-1);
	}
#endif

	return windowManager;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::LinuxWindowManagerToString(const TRAP::Utils::LinuxWindowManager lwm)
{
	switch(lwm)
	{
	case LinuxWindowManager::X11:
		return "X11";

	case LinuxWindowManager::Wayland:
		return "Wayland";

	default:
		return "Unknown";
	}
}
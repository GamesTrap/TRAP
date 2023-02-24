#include "TRAPPCH.h"
#include "Utils.h"

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Application.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

[[nodiscard]] std::string TRAP::Utils::UUIDToString(const std::array<uint8_t, 16>& uuid)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

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

[[nodiscard]] std::array<uint8_t, 16> TRAP::Utils::UUIDFromString(const std::string_view uuid)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	uint8_t digit = 0;
	bool firstDigit = true;
	std::size_t index = 0;

	if (uuid.empty())
		return {};

	std::array<uint8_t, 16> result{};
	for (const uint8_t i : uuid)
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
			uint8_t charDigit = 0;
			uint8_t uuidDigit = 0;

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

[[nodiscard]] TRAP::Utils::Endian TRAP::Utils::GetEndian()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//Check if machine is using little-endian or big-endian
	const int32_t intVal = 1;
	const uint8_t* const uVal = reinterpret_cast<const uint8_t*>(&intVal);
#if __cpp_lib_endian
	static Endian endian = static_cast<Endian>(std::endian::native == std::endian::little);
#else
	static Endian endian = static_cast<Endian>(uVal[0] == 1);
#endif

	return endian;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Utils::CPUInfo& TRAP::Utils::GetCPUInfo()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	static CPUInfo cpu{};

	if(!cpu.Model.empty())
		return cpu;

	static constexpr auto CPUID = [](const uint32_t funcID, const uint32_t subFuncID)
	{
		std::array<int32_t, 4> regs{};
	#ifdef TRAP_PLATFORM_WINDOWS
		__cpuidex(regs.data(), static_cast<int32_t>(funcID), static_cast<int32_t>(subFuncID));
	#else
		asm volatile
			("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
				: "a" (funcID), "c" (subFuncID));
	#endif

		return Utils::BitCast<std::array<int32_t, 4>, std::array<uint32_t, 4>>(regs);
	};

	std::array<uint32_t, 4> regs = CPUID(0, 0);
	const uint32_t HFS = regs[0];
	//Get Vendor
	const std::string vendorID = std::string(reinterpret_cast<char*>(&regs[1]), sizeof(uint32_t)) +
		                         std::string(reinterpret_cast<char*>(&regs[3]), sizeof(uint32_t)) +
		                         std::string(reinterpret_cast<char*>(&regs[2]), sizeof(uint32_t));
	regs = CPUID(1, 0);
	cpu.HyperThreaded = regs[3] & 0x10000000; //Get Hyper-threading

	const std::string upVendorID = Utils::String::ToUpper(vendorID);
	//Get Number of cores
	static constexpr int32_t MAX_INTEL_TOP_LVL = 4;
	static constexpr uint32_t LVL_TYPE = 0x0000FF00;
	static constexpr uint32_t LVL_CORES = 0x0000FFFF;
	if (upVendorID.find("INTEL") != std::string::npos)
	{
		if (HFS >= 11)
		{
			int32_t numSMT = 0;
			for (int32_t lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl)
			{
				const std::array<uint32_t, 4> regs1 = CPUID(0x0B, lvl);
				const uint32_t currentLevel = (LVL_TYPE & regs1[2]) >> 8;
				switch (currentLevel)
				{
				case BIT(0):
					numSMT = static_cast<int32_t>(LVL_CORES & regs1[1]);
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
					const std::array<uint32_t, 4> regs1 = CPUID(4, 0);
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
		uint32_t extFamily = 0;
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
		cpu.Model += std::string(reinterpret_cast<char*>(&regs1[0]), sizeof(uint32_t));
		cpu.Model += std::string(reinterpret_cast<char*>(&regs1[1]), sizeof(uint32_t));
		cpu.Model += std::string(reinterpret_cast<char*>(&regs1[2]), sizeof(uint32_t));
		cpu.Model += std::string(reinterpret_cast<char*>(&regs1[3]), sizeof(uint32_t));
	}

	std::size_t lastAlphaChar = 0;
	for(auto it = cpu.Model.rbegin(); it != cpu.Model.rend(); ++it)
	{
		if (isalnum(*it))
		{
			lastAlphaChar = it - cpu.Model.rbegin();
			break;
		}
	}
	cpu.Model.erase(cpu.Model.end() - lastAlphaChar, cpu.Model.end());

	return cpu;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::LinuxWindowManager TRAP::Utils::GetLinuxWindowManager()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	static LinuxWindowManager windowManager{};

#ifdef TRAP_PLATFORM_LINUX
	if(windowManager != LinuxWindowManager::Unknown)
		return windowManager;

	const std::string wl = "wayland";
	const std::string x = "x11";
	std::string session;
	if(getenv("XDG_SESSION_TYPE"))
		session = getenv("XDG_SESSION_TYPE");
	if (getenv("WAYLAND_DISPLAY") || session == wl)
		windowManager = LinuxWindowManager::Wayland;
	else if (getenv("DISPLAY") || session == x)
		windowManager = LinuxWindowManager::X11;
	else
	{
#ifndef TRAP_HEADLESS_MODE
		Utils::Dialogs::ShowMsgBox("Unsupported window manager", "Window manager is unsupported!\n"
									"TRAP™ currently only supports X11/Xwayland\n"
									//"TRAP™ uses X11 or Wayland\n"
									"Make sure the appropriate environment variable(s) is/are set!\n"
									"Error code: 0x0008",
									Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::EngineLinuxPrefix, "Unsupported window manager!");
		TRAP::Application::Shutdown();
		exit(0x0008);
#else
		return LinuxWindowManager::Unknown;
#endif
	}

#ifndef ENABLE_WAYLAND_SUPPORT
	//Replace Wayland with X11
	using namespace std::string_view_literals;
	if(windowManager == LinuxWindowManager::Wayland)
	{
		if(getenv("DISPLAY") || getenv("XDG_SESSION_TYPE") == "x11"sv)
			windowManager = LinuxWindowManager::X11;
		else
			windowManager = LinuxWindowManager::Unknown;
	}
#endif
#endif

	return windowManager;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS

static TRAP::Utils::NTDLL s_ntdll;

[[nodiscard]] BOOL TRAP::Utils::IsWindowsVersionOrGreaterWin32(const WORD major, const WORD minor, const WORD sp)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	if(!s_ntdll.Instance || !s_ntdll.RtlVerifyVersionInfo) //Init s_ntdll if not already done
	{
		s_ntdll.Instance = static_cast<HINSTANCE>(DynamicLoading::LoadLibrary("ntdll.dll"));
		if (s_ntdll.Instance)
		{
			s_ntdll.RtlVerifyVersionInfo = DynamicLoading::GetLibrarySymbol<PFN_RtlVerifyVersionInfo>(s_ntdll.Instance,
																									  "RtlVerifyVersionInfo");
		}

		TRAP_ASSERT(s_ntdll.Instance && s_ntdll.RtlVerifyVersionInfo, "Utils::IsWindowsVersionOrGreaterWin32(): Failed to load ntdll.dll");
	}

	OSVERSIONINFOEXW osvi = { sizeof(osvi), major, minor, 0, 0, {0}, sp };
	const DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR;
	ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
	//HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
	//      latter lies unless the user knew to embed a non-default manifest
	//      announcing support for Windows 10 via supportedOS GUID
	return s_ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows10BuildOrGreaterWin32(const WORD build)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	if(!s_ntdll.Instance || !s_ntdll.RtlVerifyVersionInfo) //Init s_ntdll if not already done
	{
		s_ntdll.Instance = static_cast<HINSTANCE>(DynamicLoading::LoadLibrary("ntdll.dll"));
		if (s_ntdll.Instance)
		{
			s_ntdll.RtlVerifyVersionInfo = DynamicLoading::GetLibrarySymbol<PFN_RtlVerifyVersionInfo>(s_ntdll.Instance,
																									  "RtlVerifyVersionInfo");
		}

		TRAP_ASSERT(s_ntdll.Instance && s_ntdll.RtlVerifyVersionInfo, "Utils::IsWindows10BuildOrGreaterWin32(): Failed to load ntdll.dll");
	}

	OSVERSIONINFOEXW osvi = { sizeof(osvi), 10, 0, build };
	const DWORD mask = VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER;
	ULONGLONG cond = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_MINORVERSION, VER_GREATER_EQUAL);
	cond = VerSetConditionMask(cond, VER_BUILDNUMBER, VER_GREATER_EQUAL);
	//HACK: Use RtlVerifyVersionInfo instead of VerifyVersionInfoW as the
	//      latter lies unless the user knew to embed a non-default manifest
	//      announcing support for Windows 10 via supportedOS GUID
	return s_ntdll.RtlVerifyVersionInfo(&osvi, mask, cond) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows11BuildOrGreaterWin32(const WORD build)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return IsWindows10BuildOrGreaterWin32(build); //Windows 11 shares 10.0.XXXXX format with Windows 10
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows10Version1607OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return IsWindows10BuildOrGreaterWin32(14393);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows10Version1703OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return IsWindows10BuildOrGreaterWin32(15063);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows8Point1OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return IsWindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WINBLUE),
		                                  LOBYTE(_WIN32_WINNT_WINBLUE), 0);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows8OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return IsWindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WIN8),
		                                  LOBYTE(_WIN32_WINNT_WIN8), 0);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows7OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return IsWindowsVersionOrGreaterWin32(HIBYTE(_WIN32_WINNT_WIN7),
		                                  LOBYTE(_WIN32_WINNT_WIN7), 0);
}

#endif
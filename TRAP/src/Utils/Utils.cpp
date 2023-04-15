#include "TRAPPCH.h"
#include "Utils.h"

#include "Core/PlatformDetection.h"
#include "Utils/Memory.h"
#include "Utils/String/String.h"
#include "Utils/Dialogs/Dialogs.h"
#include "Application.h"
#include "Utils/DynamicLoading/DynamicLoading.h"

[[nodiscard]] std::string TRAP::Utils::UUIDToString(const std::array<uint8_t, 16>& uuid)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	std::stringstream s;

	s << std::hex << std::setfill('0')
	  << std::setw(2) << static_cast<int32_t>(std::get<0>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<1>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<2>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<3>(uuid))
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(std::get<4>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<5>(uuid))
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(std::get<6>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<7>(uuid))
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(std::get<8>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<9>(uuid))
	  << '-'
	  << std::setw(2) << static_cast<int32_t>(std::get<10>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<11>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<12>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<13>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<14>(uuid))
	  << std::setw(2) << static_cast<int32_t>(std::get<15>(uuid));

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

		if (index >= 16u || (std::isxdigit(i) == 0))
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
			result[index] = (static_cast<uint8_t>((charDigit << 4u)) | uuidDigit);
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
			("cpuid" : "=a" (std::get<0>(regs)), "=b" (std::get<1>(regs)), "=c" (std::get<2>(regs)), "=d" (std::get<3>(regs))
				: "a" (funcID), "c" (subFuncID));
	#endif

		return Utils::BitCast<std::array<uint32_t, 4>>(regs);
	};

	std::array<uint32_t, 4> regs = CPUID(0, 0);
	const uint32_t HFS = std::get<0>(regs);
	//Get Vendor
	const std::string vendorID = std::string(reinterpret_cast<char*>(&std::get<1>(regs)), sizeof(uint32_t)) +
		                         std::string(reinterpret_cast<char*>(&std::get<2>(regs)), sizeof(uint32_t)) +
		                         std::string(reinterpret_cast<char*>(&std::get<3>(regs)), sizeof(uint32_t));
	regs = CPUID(1, 0);
	cpu.HyperThreaded = ((std::get<3>(regs) & 0x10000000u) != 0u); //Get Hyper-threading

	const std::string upVendorID = Utils::String::ToUpper(vendorID);
	//Get Number of cores
	static constexpr int32_t MAX_INTEL_TOP_LVL = 4;
	static constexpr uint32_t LVL_TYPE = 0x0000FF00;
	static constexpr uint32_t LVL_CORES = 0x0000FFFF;
	if (upVendorID.find("INTEL") != std::string::npos)
	{
		if (HFS >= 11u)
		{
			int32_t numSMT = 0;
			for (int32_t lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl)
			{
				const std::array<uint32_t, 4> regs1 = CPUID(0x0B, lvl);
				const uint32_t currentLevel = (LVL_TYPE & std::get<2>(regs1)) >> 8u;
				switch (currentLevel)
				{
				case BIT(0):
					numSMT = static_cast<int32_t>(LVL_CORES & std::get<1>(regs1));
					break;

				case BIT(1):
					cpu.LogicalCores = LVL_CORES & std::get<1>(regs1);
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
				cpu.LogicalCores = (std::get<1>(regs) >> 16u) & 0xFFu;
				if (HFS >= 4)
				{
					const std::array<uint32_t, 4> regs1 = CPUID(4, 0);
					cpu.Cores = (1 + (std::get<0>(regs1) >> 26u)) & 0x3Fu;
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
		if (((std::get<0>(regs) >> 8u) & 0xFu) < 0xFu)
			extFamily = (std::get<0>(regs) >> 8u) & 0xFu;
		else
			extFamily = ((std::get<0>(regs) >> 8u) & 0xFu) + ((std::get<0>(regs) >> 20u) & 0xFFu);

		if (HFS >= 1)
		{
			cpu.LogicalCores = (std::get<1>(regs) >> 16u) & 0xFFu;
			std::array<uint32_t, 4> regs1 = CPUID(0x80000000u, 0u);
			if (std::get<0>(regs1) >= 8u)
			{
				regs1 = CPUID(0x80000008u, 0u);
				cpu.Cores = 1 + (std::get<2>(regs1) & 0xFFu);
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
				std::array<uint32_t, 4> regs1 = CPUID(0x80000000u, 0u);
				if ((extFamily >= 23u) && (std::get<0>(regs1) >= 30u))
				{
					regs1 = CPUID(0x8000001Eu, 0u);
					cpu.Cores /= ((std::get<1>(regs1) >> 8u) & 0xFFu) + 1u;
				}
			}
		}
		else
			cpu.Cores = cpu.LogicalCores = 1;
	}

	//Get CPU brand string
	for (uint32_t i = 0x80000002u; i < 0x80000005u; ++i)
	{
		std::array<uint32_t, 4> regs1 = CPUID(i, 0);
		cpu.Model += std::string(reinterpret_cast<char*>(&std::get<0>(regs1)), sizeof(uint32_t));
		cpu.Model += std::string(reinterpret_cast<char*>(&std::get<1>(regs1)), sizeof(uint32_t));
		cpu.Model += std::string(reinterpret_cast<char*>(&std::get<2>(regs1)), sizeof(uint32_t));
		cpu.Model += std::string(reinterpret_cast<char*>(&std::get<3>(regs1)), sizeof(uint32_t));
	}

	std::size_t lastAlphaChar = 0;
	for(auto it = cpu.Model.rbegin(); it != cpu.Model.rend(); ++it)
	{
		if (isalnum(*it) != 0)
		{
			lastAlphaChar = it - cpu.Model.rbegin();
			break;
		}
	}
	cpu.Model.erase(cpu.Model.end() - lastAlphaChar, cpu.Model.end());

	return cpu;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::LinuxWindowManager TRAP::Utils::GetLinuxWindowManager()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	static LinuxWindowManager windowManager = LinuxWindowManager::Unknown;

#ifdef TRAP_PLATFORM_LINUX
	if(windowManager != LinuxWindowManager::Unknown)
		return windowManager;

	const std::string wl = "wayland";
	const std::string x11 = "x11";
	std::string session;

	//TRAP_WM env var allows the user to override the window manager detection
	if(getenv("TRAP_WM"))
	{
		session = getenv("TRAP_WM");

		if(session == wl)
			windowManager = LinuxWindowManager::Wayland;
		else if(session == x11)
			windowManager = LinuxWindowManager::X11;


		//Return if a valid window manager was set via TRAP_WM
		if(windowManager != LinuxWindowManager::Unknown)
		{
			TP_INFO(Log::EngineLinuxPrefix, "TRAP_WM env var detected, now using ",
			                                Utils::String::ConvertToString(windowManager));
			return windowManager;
		}
	}

	//Proceed with normal detection
	session = "";

	if(getenv("XDG_SESSION_TYPE"))
		session = getenv("XDG_SESSION_TYPE");

	if (getenv("WAYLAND_DISPLAY") || session == wl)
		windowManager = LinuxWindowManager::Wayland;
	else if (getenv("DISPLAY") || session == x11)
		windowManager = LinuxWindowManager::X11;
	else
	{
#ifndef TRAP_HEADLESS_MODE
		Utils::Dialogs::ShowMsgBox("Unsupported window manager", "Window manager is unsupported!\n"
									"TRAP™ only supports X11 and Wayland\n"
									"Make sure the appropriate environment variable(s) is/are set!\n"
									"Error code: 0x0008",
									Utils::Dialogs::Style::Error, Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(Log::EngineLinuxPrefix, "Window manager is unsupported! (0x0008)");
		exit(0x0008);
#else
		return LinuxWindowManager::Unknown;
#endif /*TRAP_HEADLESS_MODE*/
	}

#ifndef ENABLE_WAYLAND_SUPPORT
	//Replace Wayland with X11 (thus forcing to use Xwayland)
	using namespace std::string_view_literals;
	if(windowManager == LinuxWindowManager::Wayland)
	{
		if(getenv("DISPLAY") || getenv("XDG_SESSION_TYPE") == "x11"sv)
			windowManager = LinuxWindowManager::X11;
		else
			windowManager = LinuxWindowManager::Unknown;
	}
#endif /*ENABLE_WAYLAND_SUPPORT*/
#endif /*TRAP_PLATFORM_LINUX*/

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

//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_WINDOWS)
[[nodiscard]] static bool CheckSingleProcessWindows()
{
	ZoneScoped;

	const HANDLE hMutex = CreateMutex(0, 0, L"TRAP-Engine");
	if(!hMutex) //Error creating mutex
	{
		TP_ERROR(TRAP::Log::ApplicationPrefix, "Failed to create mutex!");
		TP_ERROR(TRAP::Log::ApplicationPrefix, TRAP::Utils::String::GetStrError());
		return false;
	}
	if(hMutex && GetLastError() == ERROR_ALREADY_EXISTS)
		return false;

	return true;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_LINUX)
[[nodiscard]] static bool CheckSingleProcessLinux()
{
	ZoneScoped;

	static int32_t socketFD = -1;
	static int32_t rc = 1;
	static constexpr uint16_t port = 49420; //Just a free (hopefully) random port

	if(socketFD == -1 || (rc != 0))
	{
		socketFD = -1;
		rc = 1;

		socketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if(socketFD < 0)
		{
			TP_ERROR(TRAP::Log::ApplicationPrefix, "Failed to create socket!");
			TP_ERROR(TRAP::Log::ApplicationPrefix, TRAP::Utils::String::GetStrError());
			return false;
		}

		sockaddr_in name{};
		name.sin_family = AF_INET;
		name.sin_port = port;
		name.sin_addr.s_addr = INADDR_ANY;

		if(TRAP::Utils::GetEndian() != TRAP::Utils::Endian::Big)
		{
			TRAP::Utils::Memory::SwapBytes(name.sin_port);
			TRAP::Utils::Memory::SwapBytes(name.sin_addr.s_addr);
		}

		sockaddr convertedSock = TRAP::Utils::BitCast<sockaddr>(name); //Prevent usage of reinterpret_cast
		rc = bind(socketFD, &convertedSock, sizeof(name));
		if(rc < 0)
		{
			TP_ERROR(TRAP::Log::ApplicationPrefix, "Failed to bind socket!");
			TP_ERROR(TRAP::Log::ApplicationPrefix, TRAP::Utils::String::GetStrError());
		}
	}

	return (socketFD != -1 && rc == 0);
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::CheckSingleProcess()
{
	//Single process mode
#ifdef ENABLE_SINGLE_PROCESS_ONLY
	bool singleProcess = true;
	#ifdef TRAP_PLATFORM_LINUX
		singleProcess = CheckSingleProcessLinux();
	#elif defined(TRAP_PLATFORM_WINDOWS)
		singleProcess = CheckSingleProcessWindows();
	#endif

	if(!singleProcess)
	{
		TRAP::Utils::Dialogs::ShowMsgBox("TRAP™ is already running", "A TRAP™ Application is already running!\n"
		                                 "Error code: 0x0012", TRAP::Utils::Dialogs::Style::Error,
								         TRAP::Utils::Dialogs::Buttons::Quit);
		TP_CRITICAL(TRAP::Log::ApplicationPrefix, "A TRAP™ Application is already running! (0x0012)");
		exit(0x0012);
	}
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_PLATFORM_WINDOWS) && defined(TRAP_HEADLESS_MODE)
static BOOL WINAPI SIGINTHandlerRoutine(_In_ DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_C_EVENT)
	{
		TRAP::Application::Shutdown();
		return TRUE;
	}
	else if (dwCtrlType == CTRL_CLOSE_EVENT)
	{
		TRAP::Application::Shutdown();
		Sleep(10000);
		return TRUE;
	}

	return FALSE;
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Utils::RegisterSIGINTCallback()
{
#ifdef TRAP_PLATFORM_LINUX
	if(signal(SIGINT, [](int) {TRAP::Application::Shutdown(); }) == SIG_ERR)
#elif defined(TRAP_PLATFORM_WINDOWS)
	if(!SetConsoleCtrlHandler(SIGINTHandlerRoutine, TRUE))
#endif
	{
		TP_ERROR(TRAP::Log::ApplicationPrefix, "Failed to register SIGINT callback!");
		TP_ERROR(TRAP::Log::ApplicationPrefix, TRAP::Utils::String::GetStrError());
	}
}
#endif /*TRAP_HEADLESS_MODE*/

#include "TRAPPCH.h"
#include "Utils.h"

#include "Core/PlatformDetection.h"
#include "Utils/String/String.h"
#include "Utils/ErrorCodes/ErrorCodes.h"
#include "Utils/DynamicLoading/DynamicLoading.h"
#include "Application.h"

namespace
{
#ifdef TRAP_PLATFORM_WINDOWS
	using PFN_RtlVerifyVersionInfo = LONG(WINAPI*)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);
	struct NTDLL
	{
		HINSTANCE Instance = nullptr;
		PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo = nullptr;
	} s_ntdll;

	[[nodiscard]] bool InitNTDLL()
	{
		if(!s_ntdll.Instance || !s_ntdll.RtlVerifyVersionInfo) //Init s_ntdll if not already done
		{
			s_ntdll.Instance = static_cast<HINSTANCE>(TRAP::Utils::DynamicLoading::LoadLibrary("ntdll.dll"));
			if (s_ntdll.Instance)
			{
				s_ntdll.RtlVerifyVersionInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<PFN_RtlVerifyVersionInfo>(s_ntdll.Instance,
																										               "RtlVerifyVersionInfo");
			}

			TRAP_ASSERT(s_ntdll.Instance && s_ntdll.RtlVerifyVersionInfo, "Utils::InitNTDLL(): Failed to load ntdll.dll");
		}

		return s_ntdll.Instance && s_ntdll.RtlVerifyVersionInfo;
	}
#endif /*TRAP_PLATFORM_WINDOWS*/

	//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_WINDOWS)
	[[nodiscard]] bool CheckSingleProcessWindows()
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
#endif /*ENABLE_SINGLE_PROCESS_ONLY && TRAP_PLATFORM_WINDOWS*/

	//-------------------------------------------------------------------------------------------------------------------//

#if defined(ENABLE_SINGLE_PROCESS_ONLY) && defined(TRAP_PLATFORM_LINUX)
	[[nodiscard]] bool CheckSingleProcessLinux()
	{
		ZoneScoped;

		constinit static i32 socketFD = -1;
		constinit static i32 rc = 1;
		static constexpr u16 port = 49420; //Just a free (hopefully) random port

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

			if constexpr (Utils::GetEndian() != Utils::Endian::Big)
			{
				TRAP::Utils::Memory::SwapBytes(name.sin_port);
				TRAP::Utils::Memory::SwapBytes(name.sin_addr.s_addr);
			}

			sockaddr convertedSock = std::bit_cast<sockaddr>(name); //Prevent usage of reinterpret_cast
			rc = bind(socketFD, &convertedSock, sizeof(name));
			if(rc < 0)
			{
				TP_ERROR(TRAP::Log::ApplicationPrefix, "Failed to bind socket!");
				TP_ERROR(TRAP::Log::ApplicationPrefix, TRAP::Utils::String::GetStrError());
			}
		}

		return (socketFD != -1 && rc == 0);
	}
#endif /*ENABLE_SINGLE_PROCESS_ONLY && TRAP_PLATFORM_LINUX*/

	//-------------------------------------------------------------------------------------------------------------------//

#if defined(TRAP_PLATFORM_WINDOWS) && defined(TRAP_HEADLESS_MODE)
	[[nodiscard]] BOOL WINAPI SIGINTHandlerRoutine(_In_ DWORD dwCtrlType)
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
#endif /*TRAP_PLATFORM_WINDOWS && TRAP_HEADLESS_MODE*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Utils::UUIDToString(const TRAP::Utils::UUID& uuid)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	return fmt::format("{:02x}{:02x}{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
					   std::get<0>(uuid), std::get<1>(uuid), std::get<2>(uuid), std::get<3>(uuid),
					   std::get<4>(uuid), std::get<5>(uuid), std::get<6>(uuid), std::get<7>(uuid),
					   std::get<8>(uuid), std::get<9>(uuid), std::get<10>(uuid), std::get<11>(uuid),
					   std::get<12>(uuid), std::get<13>(uuid), std::get<14>(uuid), std::get<15>(uuid));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::UUID TRAP::Utils::UUIDFromString(const std::string_view uuid)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	TRAP::Utils::UUID result{};

	if(uuid.empty())
		return {};

	usize index = 0;
	for(const char c : uuid)
	{
		if(!String::IsHexDigit(c)) //Ignore non hex characters
			continue;

		if(index >= 32) //Out of bounds
			return {};

		const u8 convertedCharacter = String::IsDigit(c) ? NumericCast<u8>(c - '0') :
		                                                        NumericCast<u8>(String::ToLower(c) - 'a' + 10);
		if(index % 2 == 0)
			result[index / 2] = NumericCast<u8>(convertedCharacter << 4u);
		else
			result[index / 2] |= convertedCharacter;

		++index;
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Utils::CPUInfo& TRAP::Utils::GetCPUInfo()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	//Note this will only store info of the CPU we are currently running on.
	//Its likely that this is broken on multi NUMA systems... //TODO

	static CPUInfo cpu{};

	if(!cpu.Model.empty())
		return cpu;

	static const auto CPUID = [](const u32 funcID, const u32 subFuncID)
	{
	#ifdef TRAP_PLATFORM_WINDOWS
		std::array<i32, 4> regs{};
		__cpuidex(regs.data(), static_cast<i32>(funcID), static_cast<i32>(subFuncID));
	#else
		std::array<u32, 4> regs{};
		__get_cpuid_count(funcID, subFuncID, &std::get<0>(regs), &std::get<1>(regs), &std::get<2>(regs), &std::get<3>(regs));
	#endif

		return std::bit_cast<std::array<u32, 4>>(regs);
	};

	std::array<u32, 4> regs = CPUID(0, 0);
	const u32 HFS = std::get<0>(regs);
	//Get Vendor
	const std::string vendorID = std::string(reinterpret_cast<char*>(&std::get<1>(regs)), sizeof(u32)) +
		                         std::string(reinterpret_cast<char*>(&std::get<2>(regs)), sizeof(u32)) +
		                         std::string(reinterpret_cast<char*>(&std::get<3>(regs)), sizeof(u32));
	regs = CPUID(1, 0);
	cpu.HyperThreaded = ((std::get<3>(regs) & 0x10000000u) != 0u); //Get Hyper-threading

	const std::string upVendorID = Utils::String::ToUpper(vendorID);
	//Get Number of cores
	static constexpr i32 MAX_INTEL_TOP_LVL = 4;
	static constexpr u32 LVL_TYPE = 0x0000FF00;
	static constexpr u32 LVL_CORES = 0x0000FFFF;
	if (Utils::String::Contains(upVendorID, "INTEL"))
	{
		if (HFS >= 11u)
		{
			u32 numSMT = 0;
			for (u32 lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl)
			{
				const std::array<u32, 4> regs1 = CPUID(0x0Bu, lvl);
				const u32 currentLevel = (LVL_TYPE & std::get<2>(regs1)) >> 8u;
				switch (currentLevel)
				{
				case BIT(0u):
					numSMT = LVL_CORES & std::get<1>(regs1);
					break;

				case BIT(1u):
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
					const std::array<u32, 4> regs1 = CPUID(4, 0);
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
	else if (Utils::String::Contains(upVendorID, "AMD"))
	{
		u32 extFamily = 0;
		if (((std::get<0>(regs) >> 8u) & 0xFu) < 0xFu)
			extFamily = (std::get<0>(regs) >> 8u) & 0xFu;
		else
			extFamily = ((std::get<0>(regs) >> 8u) & 0xFu) + ((std::get<0>(regs) >> 20u) & 0xFFu);

		if (HFS >= 1)
		{
			cpu.LogicalCores = (std::get<1>(regs) >> 16u) & 0xFFu;
			std::array<u32, 4> regs1 = CPUID(0x80000000u, 0u);
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
				std::array<u32, 4> regs1 = CPUID(0x80000000u, 0u);
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
	for (u32 i = 0x80000002u; i < 0x80000005u; ++i)
	{
		std::array<u32, 4> regs1 = CPUID(i, 0);
		cpu.Model += fmt::format("{}{}{}{}",
		                         std::string(reinterpret_cast<char*>(&std::get<0>(regs1)), sizeof(u32)),
		                         std::string(reinterpret_cast<char*>(&std::get<1>(regs1)), sizeof(u32)),
		                         std::string(reinterpret_cast<char*>(&std::get<2>(regs1)), sizeof(u32)),
		                         std::string(reinterpret_cast<char*>(&std::get<3>(regs1)), sizeof(u32)));
	}

	usize lastAlphaChar = 0;
	const auto it = std::ranges::find_if(std::ranges::reverse_view(cpu.Model), Utils::String::IsAlphaNumeric);
	if(it != cpu.Model.rend())
		lastAlphaChar = NumericCast<usize>(it - cpu.Model.rbegin());

	cpu.Model.erase(cpu.Model.end() - NumericCast<std::string::difference_type>(lastAlphaChar), cpu.Model.end());

	return cpu;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Utils::LinuxWindowManager TRAP::Utils::GetLinuxWindowManager()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	constinit static LinuxWindowManager windowManager = LinuxWindowManager::Unknown;

#ifdef TRAP_PLATFORM_LINUX
	if(windowManager != LinuxWindowManager::Unknown)
		return windowManager;

	static constexpr std::string wl = "wayland";
	static constexpr std::string x11 = "x11";
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
			TP_INFO(Log::EngineLinuxPrefix, "TRAP_WM env var detected, now using ", windowManager);
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
		Utils::DisplayError(Utils::ErrorCode::LinuxUnsupportedWindowManager);
#else
		return LinuxWindowManager::Unknown;
#endif /*TRAP_HEADLESS_MODE*/
	}
#endif /*TRAP_PLATFORM_LINUX*/

	return windowManager;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS

[[nodiscard]] BOOL TRAP::Utils::IsWindowsVersionOrGreaterWin32(const WORD major, const WORD minor, const WORD sp)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	if(!InitNTDLL())
		return false;

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
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	if(!InitNTDLL())
		return false;

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
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return IsWindows10BuildOrGreaterWin32(build); //Windows 11 shares 10.0.XXXXX format with Windows 10
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows11OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return IsWindows10BuildOrGreaterWin32(22000); //Windows 11 shares 10.0.XXXXX format with Windows 10
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows10Version1607OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return IsWindows10BuildOrGreaterWin32(14393);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows10Version1703OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return IsWindows10BuildOrGreaterWin32(15063);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] BOOL TRAP::Utils::IsWindows10OrGreaterWin32()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None &&
	                                          (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return IsWindows10BuildOrGreaterWin32(10240);
}

#endif /*TRAP_PLATFORM_WINDOWS*/

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
		Utils::DisplayError(Utils::ErrorCode::ApplicationIsAlreadyRunning);
#endif /*ENABLE_SINGLE_PROCESS_ONLY*/
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_HEADLESS_MODE
void TRAP::Utils::RegisterSIGINTCallback()
{
#ifdef TRAP_PLATFORM_LINUX
	if(signal(SIGINT, [](i32) {TRAP::Application::Shutdown(); }) == SIG_ERR)
#elif defined(TRAP_PLATFORM_WINDOWS)
	if(!SetConsoleCtrlHandler(SIGINTHandlerRoutine, TRUE))
#endif
	{
		TP_ERROR(TRAP::Log::ApplicationPrefix, "Failed to register SIGINT callback!");
		TP_ERROR(TRAP::Log::ApplicationPrefix, TRAP::Utils::String::GetStrError());
	}
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
void TRAP::Utils::SetThreadName(const std::string_view name)
{
	tracy::SetThreadName(name.data());
}
#endif /*TRACY_ENABLE*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRACY_ENABLE
void TRAP::Utils::SetThreadName(const std::string_view name, const ThreadGroup group)
{
	tracy::SetThreadNameWithHint(name.data(), std::to_underlying(group));
}
#endif /*TRACY_ENABLE*/

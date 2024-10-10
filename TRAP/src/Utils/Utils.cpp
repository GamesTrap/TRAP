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
	class NTDLL final
	{
	public:
		using PFN_RtlVerifyVersionInfo = LONG(WINAPI*)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);

		NTDLL()
		{
			m_instance = TRAP::Utils::DynamicLoading::LoadLibrary("ntdll.dll");
			if (!m_instance)
				TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::FailedToLoadLibrary);

			RtlVerifyVersionInfo = TRAP::Utils::DynamicLoading::GetLibrarySymbol<NTDLL::PFN_RtlVerifyVersionInfo>(m_instance, "RtlVerifyVersionInfo");
			if (!RtlVerifyVersionInfo)
				TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::FailedToLoadFunctionFromLibrary);
		}

		~NTDLL()
		{
			TRAP::Utils::DynamicLoading::FreeLibrary(m_instance);
		}

		NTDLL(const NTDLL&) = delete;
		NTDLL(NTDLL&&) = delete;

		NTDLL& operator=(const NTDLL&) = delete;
		NTDLL& operator=(NTDLL&&) = delete;

		PFN_RtlVerifyVersionInfo RtlVerifyVersionInfo = nullptr;
	private:
		void* m_instance = nullptr;
	} s_ntdll{};
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

		static const auto TryAcquireLockfile = []()
		{
			static constexpr std::string_view lockfile = "/tmp/trap_engine.lock";

			const i32 fd = open(lockfile.data(), O_CREAT | O_RDWR, 0666);
			if(fd == -1)
			{
				TP_ERROR(TRAP::Log::UtilsPrefix, "Failed to open lockfile!");
				TP_ERROR(TRAP::Log::UtilsPrefix, TRAP::Utils::String::GetStrError());
				return false;
			}

			const i32 lockResult = flock(fd, LOCK_EX | LOCK_NB);
			if(lockResult != 0)
			{
				if(errno != EWOULDBLOCK)
				{
					TP_ERROR(TRAP::Log::UtilsPrefix, "Failed to acquire lockfile!");
					TP_ERROR(TRAP::Log::UtilsPrefix, TRAP::Utils::String::GetStrError());
				}
				close(fd);
				return false; //Another process holds the lock
			}

			return true;
		};

		static const bool isSingleProcess = TryAcquireLockfile();

		return isSingleProcess;
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

[[nodiscard]] const TRAP::Utils::CPUInfo& TRAP::Utils::GetCPUInfo()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	static const auto LoadCPUInfo = []()
	{
		CPUInfo cpu{};

#ifdef TRAP_PLATFORM_WINDOWS
		//Retrieve physical core count
		DWORD length = 0u;
		GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length);
		std::vector<u8> buffer(length);
		if(GetLogicalProcessorInformationEx(RelationProcessorCore, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()), &length))
		{
			auto* logicalProcInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());
			for(DWORD i = 0u; i < buffer.size(); i += logicalProcInfo->Size)
			{
				if(logicalProcInfo->Relationship == RelationProcessorCore)
					++cpu.Cores;

				logicalProcInfo = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(reinterpret_cast<u8*>(logicalProcInfo) + logicalProcInfo->Size);
			}
		}

		//Retrieve thread count
		SYSTEM_INFO sysInfo{};
		GetSystemInfo(&sysInfo);
		cpu.LogicalCores = sysInfo.dwNumberOfProcessors;

		//Retrieve CPU model name
		HKEY hKey{};
		static constexpr std::wstring_view subKey = L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
		static constexpr std::wstring_view valueName = L"ProcessorNameString";
		if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, subKey.data(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD bufferSize = 0u;
			std::wstring temp{};
			if(RegGetValueW(hKey, nullptr, valueName.data(), RRF_RT_REG_SZ, nullptr, nullptr, &bufferSize) == ERROR_SUCCESS)
			{
				temp.resize(bufferSize / sizeof(wchar_t));

				if(RegGetValueW(hKey, nullptr, valueName.data(), RRF_RT_REG_SZ, nullptr, temp.data(), &bufferSize) == ERROR_SUCCESS)
					cpu.Model = TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(temp);
			}

			RegCloseKey(hKey);
		}
#elif defined(TRAP_PLATFORM_LINUX)
		std::ifstream cpuInfo("/proc/cpuinfo");
		std::set<i32> coreIDs{};

		std::string temp{};
		while(std::getline(cpuInfo, temp))
		{
			if(temp.find("core id") != std::string::npos)
			{
				try
				{
					const i32 coreID = std::stoi(temp.substr(temp.find(':') + 1u));
					coreIDs.insert(coreID);
				}
				catch(...)
				{
					TP_ERROR(Log::UtilsPrefix, "Failed to parse /proc/cpuinfo for physical core count!");
				}
			}

			if(cpu.Model.empty() && temp.find("model name") != std::string::npos)
			{
				if(const usize pos = temp.find(':'); pos != std::string::npos)
					cpu.Model = temp.substr(pos + 2u);
			}
		}
		cpu.Cores = NumericCast<u32>(coreIDs.size());
		cpu.LogicalCores = NumericCast<u32>(sysconf(_SC_NPROCESSORS_ONLN));
#endif

		if (cpu.Cores == 0u)
			cpu.Cores = 1u;
		if (cpu.LogicalCores == 0u)
			cpu.LogicalCores = cpu.Cores;
		cpu.HyperThreaded = cpu.LogicalCores > cpu.Cores;
		if(cpu.Model.empty())
			cpu.Model = "Unknown CPU";

		usize lastAlphaChar = 0u;
		if(const auto it = std::ranges::find_if(std::ranges::reverse_view(cpu.Model), Utils::String::IsAlphaNumeric);it != cpu.Model.rend())
			lastAlphaChar = NumericCast<usize>(it - cpu.Model.rbegin());
		cpu.Model.erase(cpu.Model.end() - NumericCast<std::string::difference_type>(lastAlphaChar), cpu.Model.end());

		return cpu;
	};

	static const CPUInfo cpu{LoadCPUInfo()};

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

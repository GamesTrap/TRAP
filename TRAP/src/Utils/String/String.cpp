#include "TRAPPCH.h"
#include "String.h"

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Utils::String::GetTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timePoint) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	const std::time_t time = std::chrono::system_clock::to_time_t(timePoint);

	std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
	localtime_s(&tm, &time);
#elif defined(TRAP_PLATFORM_LINUX)
	localtime_r(&time, &tm);
#endif

	std::array<char, 9> buffer{};
	if(strftime(buffer.data(), buffer.size(), "%T", &tm) == 0)
		return "";

	return std::string(buffer.data(), buffer.size() - 1); //Copy data except the null terminator
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] std::string TRAP::Utils::String::GetDateTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& dateTimePoint) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	const std::time_t time = std::chrono::system_clock::to_time_t(dateTimePoint);

	std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
	localtime_s(&tm, &time);
#elif defined(TRAP_PLATFORM_LINUX)
	localtime_r(&time, &tm);
#endif

	std::array<char, 20> buffer{};
	if(strftime(buffer.data(), buffer.size(), "%F %T", &tm) == 0)
		return "";

	return std::string(buffer.data(), buffer.size() - 1); //Copy data except the null terminator
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_LINUX
[[nodiscard]] std::string TRAP::Utils::String::GetStrError()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

    std::string error(1024, '\0');
    #if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
        strerror_r(errno, error.data(), error.size());
        //Remove trailing terminating null characters
        error.resize(error.find('\0'));
        return error;
    #else
        const char* const errorCStr = strerror_r(errno, error.data(), error.size());
        return std::string(errorCStr);
    #endif

    return "";
}
#elif defined(TRAP_PLATFORM_WINDOWS)
[[nodiscard]] std::string TRAP::Utils::String::GetStrError()
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	DWORD error = GetLastError();
	if(!error)
		return "";

	LPWSTR lpMsgBuf = nullptr;
	DWORD bufLen = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
	                              FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error,
								  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&lpMsgBuf), 0, nullptr);
	if(!bufLen)
		return "";

	std::string errorStr = CreateUTF8StringFromWideStringWin32(lpMsgBuf);

	if (errorStr.empty())
		return "";

	errorStr.pop_back();
	errorStr.pop_back();
	errorStr.pop_back();

	LocalFree(lpMsgBuf);

	SetLastError(0); //Clear

	return fmt::format("{} ({})", errorStr, error);
}
#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
[[nodiscard]] std::string TRAP::Utils::String::CreateUTF8StringFromWideStringWin32(const std::wstring_view wStr)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	std::string result{};

	const i32 size = WideCharToMultiByte(CP_UTF8, 0, wStr.data(), -1, nullptr, 0, nullptr, nullptr);
	if (!size)
	{
		TP_ERROR(TRAP::Log::EngineWindowsPrefix, "Failed to convert string to UTF-8");
		return {};
	}

	result.resize(size);
	if (!WideCharToMultiByte(CP_UTF8, 0, wStr.data(), -1, result.data(), size, nullptr, nullptr))
	{
		TP_ERROR(TRAP::Log::EngineWindowsPrefix, "Failed to convert string to UTF-8");
		return {};
	}

	return result;
}
#endif /*TRAP_PLATFORM_WINDOWS*/

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
[[nodiscard]] std::wstring TRAP::Utils::String::CreateWideStringFromUTF8StringWin32(const std::string_view str)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	std::wstring result{};

	const i32 count = MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, nullptr, 0);
	if (!count)
	{
		TP_ERROR(TRAP::Log::EngineWindowsPrefix, "Failed to convert string from UTF-8");
		return {};
	}

	result.resize(count);

	if (!MultiByteToWideChar(CP_UTF8, 0, str.data(), -1, result.data(), count))
	{
		TP_ERROR(TRAP::Log::EngineWindowsPrefix, "Failed to convert string from UTF-8");
		return {};
	}

	return result;
}
#endif /*TRAP_PLATFORM_WINDOWS*/

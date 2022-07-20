#include "TRAPPCH.h"
#include "String.h"

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view str,
                                                                   const std::string_view delimiters)
{
	std::size_t start = 0;
	std::size_t end = str.find_first_of(delimiters);

	std::vector<std::string_view> result;

	while (end <= std::string_view::npos && start < str.size())
	{
		std::string_view token;
		if (end != std::string_view::npos)
			token = std::string_view(str.data() + start, end - start);
		else
			token = std::string_view(str.data() + start, str.size() - start);

		if (!token.empty())
			result.push_back(token);

		if (end == std::string_view::npos)
			break;

		start = end + 1;
		end = str.find_first_of(delimiters, start);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view str,
                                                                   const char delimiter)
{
	return SplitStringView(str, std::string_view(&delimiter, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::SplitString(const std::string& str,
                                                          const std::string_view delimiters)
{
	std::size_t start = 0;
	std::size_t end = str.find_first_of(delimiters);

	std::vector<std::string> result;

	while (end <= std::string::npos)
	{
		const std::string token = str.substr(start, end - start);

		if (!token.empty())
			result.push_back(token);

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = str.find_first_of(delimiters, start);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::SplitString(const std::string& str, const char delimiter)
{
	return SplitString(str, std::string_view(&delimiter, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::GetLinesStringView(const std::string_view str)
{
	return SplitStringView(str, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::GetLines(const std::string& str)
{
	return SplitString(str, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::String::StartsWith(const std::string_view str, const std::string_view start)
{
	return str.find(start) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Utils::String::GetSuffixStringView(const std::string_view name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? std::string_view() : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetSuffix(const std::string& name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::ToLower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](const int32_t c) -> char
	{
		return static_cast<char>(::tolower(c));
	});

	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::ToUpper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](const int32_t c) -> char
	{
		return static_cast<char>(::toupper(c));
	});

	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

int64_t TRAP::Utils::String::GetCount(const std::string_view str, const char delimiter)
{
	return std::count(str.begin(), str.end(), delimiter);
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::String::CompareAnyCase(const std::string_view left, const std::string_view right)
{
	if (left.size() != right.size())
		return false;

	for (auto c1 = left.cbegin(), c2 = right.cbegin(); c1 != left.cend(); ++c1, ++c2)
	{
		if (::tolower(*c1) != ::tolower(*c2))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timePoint)
{
	const std::time_t time = std::chrono::system_clock::to_time_t(timePoint);

	std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
	localtime_s(&tm, &time);
#elif defined(TRAP_PLATFORM_LINUX)
	localtime_r(&time, &tm);
#endif

	std::array<char, 9> buffer{};
	strftime(buffer.data(), buffer.size(), "%T", &tm);

	return std::string(buffer.begin(), buffer.end());
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetDateTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& dateTimePoint)
{
	const std::time_t time = std::chrono::system_clock::to_time_t(dateTimePoint);

	std::tm tm{};
#ifdef TRAP_PLATFORM_WINDOWS
	localtime_s(&tm, &time);
#elif defined(TRAP_PLATFORM_LINUX)
	localtime_r(&time, &tm);
#endif

	std::array<char, 20> buffer{};
	strftime(buffer.data(), buffer.size(), "%F %T", &tm);

	return std::string(buffer.begin(), buffer.end());
}

//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_LINUX
std::string TRAP::Utils::String::GetStrError()
{
    std::string error(1024, '\0');
    #if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
        strerror_r(errno, error.data(), error.size());
        //Remove trailing terminating null characters
        error.resize(error.find('\0'));
        return error;
    #else
        char* errorCStr = strerror_r(errno, error.data(), error.size());
        return std::string(errorCStr);
    #endif

    return "";
}
#elif defined(TRAP_PLATFORM_WINDOWS)
std::string TRAP::Utils::String::GetStrError()
{
	//TODO Use GetLastError?
	return "";
}
#endif
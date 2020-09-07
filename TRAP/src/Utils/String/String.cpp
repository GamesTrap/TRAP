#include "TRAPPCH.h"
#include "String.h"

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view string, const std::string_view delimiters)
{
	std::size_t start = 0;
	std::size_t end = string.find_first_of(delimiters);

	std::vector<std::string_view> result;

	while (end < std::string::npos)
	{
		std::string_view token(string.data() + start, end - start);

		if (!token.empty())
			result.push_back(token);

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view string, const char delimiter)
{
	return SplitStringView(string, std::string_view(&delimiter, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::SplitString(const std::string_view string, const std::string_view delimiters)
{
	std::size_t start = 0;
	std::size_t end = string.find_first_of(delimiters);

	std::vector<std::string> result;

	while (end <= std::string::npos)
	{
		std::string token = std::string(string.substr(start, end - start));

		if (!token.empty())
			result.push_back(token);

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::SplitString(const std::string_view string, const char delimiter)
{
	return SplitString(string, std::string_view(&delimiter, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::GetLinesStringView(const std::string_view string)
{
	return SplitStringView(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::GetLines(const std::string_view string)
{
	return SplitString(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Utils::String::FindToken(const char* str, const std::string_view token)
{
	const char* t = str;
	while ((t = strstr(t, token.data())))
	{
		const bool left = str == t || isspace(t[-1]);
		const bool right = !t[token.size()] || isspace(t[token.size()]);
		if (left && right)
			return t;

		t += token.size();
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Utils::String::FindToken(const std::string_view string, const std::string_view token)
{
	return FindToken(string.data(), token);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Utils::String::GetBlockStringView(const char* str, const char** outPosition)
{
	const char* end = strstr(str, "}");
	if (!end)
		return std::string_view(str);

	if (outPosition)
		*outPosition = end;
	const auto length = static_cast<uint32_t>(end - str + 1);

	return std::string_view(str, length);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Utils::String::GetBlockStringView(const std::string_view string, const uint32_t offset)
{
	const char* str = string.data() + offset;

	return GetBlockStringView(str);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetBlock(const char* str, const char** outPosition)
{
	const char* end = strstr(str, "}");
	if (!end)
		return std::string(str);

	if (outPosition)
		* outPosition = end;
	const auto length = static_cast<uint32_t>(end - str + 1);

	return std::string(str, length);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetBlock(const std::string_view string, const uint32_t offset)
{
	const char* str = string.data() + offset;

	return GetBlock(str);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Utils::String::GetStatementStringView(const char* str, const char** outPosition)
{
	const char* end = strstr(str, ";");
	if (!end)
		return std::string_view(str);

	if (outPosition)
		*outPosition = end;
	const auto length = static_cast<uint32_t>(end - str + 1);

	return std::string_view(str, length);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetStatement(const char* str, const char** outPosition)
{
	const char* end = strstr(str, ";");
	if (!end)
		return std::string(str);

	if (outPosition)
		* outPosition = end;
	const auto length = static_cast<uint32_t>(end - str + 1);

	return std::string(str, length);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string_view> TRAP::Utils::String::TokenizeStringView(const std::string_view string)
{
	return SplitStringView(string, " \t\n");
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::Tokenize(const std::string_view string)
{
	return SplitString(string, " \t\n");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::String::StartsWith(const std::string_view string, const std::string_view start)
{
	return string.find(start) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string_view TRAP::Utils::String::GetSuffixStringView(const std::string_view name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : std::string_view(name.substr(pos + 1));
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetSuffix(const std::string_view name)
{
	const std::size_t pos = name.rfind('.');
	
	return (pos == std::string::npos) ? "" : std::string(name.substr(pos + 1));
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::ToLower(const std::string& string)
{
	std::string result(string);
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::ToUpper(const std::string& string)
{
	std::string result(string);
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Utils::String::GetCount(const std::string_view string, const char delimiter)
{
	return static_cast<uint32_t>(std::count(string.begin(), string.end(), delimiter));
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::String::CompareAnyCase(const std::string_view left, const std::string_view right)
{
	if (left.size() != right.size())
		return false;

	for (std::string_view::const_iterator c1 = left.begin(), c2 = right.begin(); c1 != left.end(); ++c1, ++c2)
	{
		if (::tolower(*c1) != ::tolower(*c2))
			return false;
	}

	return true;
}
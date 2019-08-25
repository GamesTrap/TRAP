#include "TRAPPCH.h"
#include "String.h"

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::SplitString(const std::string& string, const std::string& delimiters)
{
	std::size_t start = 0;
	std::size_t end = string.find_first_of(delimiters);

	std::vector<std::string> result;

	while (end <= std::string::npos)
	{
		std::string token = string.substr(start, end - start);

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

std::vector<std::string> TRAP::Utils::String::SplitString(const std::string& string, const char delimiter)
{
	return SplitString(string, std::string(1, delimiter));
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::GetLines(const std::string& string)
{
	return SplitString(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

const char* TRAP::Utils::String::FindToken(const char* str, const std::string& token)
{
	const char* t = str;
	while ((t = strstr(t, token.c_str())))
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

const char* TRAP::Utils::String::FindToken(const std::string& string, const std::string& token)
{
	return FindToken(string.c_str(), token);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetBlock(const char* str, const char** outPosition)
{
	const char* end = strstr(str, "}");
	if (!end)
		return std::string(str);

	if (outPosition)
		* outPosition = end;
	const auto length = static_cast<unsigned int>(end - str + 1);

	return std::string(str, length);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetBlock(const std::string& string, const unsigned int offset)
{
	const char* str = string.c_str() + offset;

	return GetBlock(str);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetStatement(const char* str, const char** outPosition)
{
	const char* end = strstr(str, ";");
	if (!end)
		return std::string(str);

	if (outPosition)
		* outPosition = end;
	const auto length = static_cast<unsigned int>(end - str + 1);

	return std::string(str, length);
}

//-------------------------------------------------------------------------------------------------------------------//

std::vector<std::string> TRAP::Utils::String::Tokenize(const std::string& string)
{
	return SplitString(string, " \t\n");
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::String::StartsWith(const std::string& string, const std::string& start)
{
	return string.find(start) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::GetSuffix(const std::string& name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

std::string TRAP::Utils::String::ToLower(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);

	return string;
}

//-------------------------------------------------------------------------------------------------------------------//

unsigned int TRAP::Utils::String::GetCount(const std::string& string, const char delimiter)
{
	return static_cast<unsigned int>(std::count(string.begin(), string.end(), delimiter));
}
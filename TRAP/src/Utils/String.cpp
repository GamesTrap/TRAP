#include "TRAPPCH.h"
#include "String.h"

namespace TRAP::Utils
{
	std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
	{
		std::size_t start = 0;
		std::size_t end = string.find_first_of(delimiters);

		std::vector<std::string> result;

		while(end <= std::string::npos)
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

	std::vector<std::string> SplitString(const std::string& string, const char delimiter)
	{
		return SplitString(string, std::string(1, delimiter));
	}

	std::vector<std::string> GetLines(const std::string& string)
	{
		return SplitString(string, "\n");
	}

	const char* FindToken(const char* str, const std::string& token)
	{
		const char* t = str;
		while((t = strstr(t, token.c_str())))
		{
			const bool left = str == t || isspace(t[-1]);
			const bool right = !t[token.size()] || isspace(t[token.size()]);
			if (left && right)
				return t;

			t += token.size();
		}
		return nullptr;
	}

	const char* FindToken(const std::string& string, const std::string& token)
	{
		return FindToken(string.c_str(), token);
	}

	std::string GetBlock(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, "}");
		if (!end)
			return std::string(str);

		if (outPosition)
			* outPosition = end;
		const auto length = static_cast<unsigned int>(end - str + 1);

		return std::string(str, length);
	}

	std::string GetBlock(const std::string& string, const unsigned int offset)
	{
		const char* str = string.c_str() + offset;

		return GetBlock(str);
	}

	std::string GetStatement(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, ";");
		if (!end)
			return std::string(str);

		if (outPosition)
			* outPosition = end;
		const auto length = static_cast<unsigned int>(end - str + 1);

		return std::string(str, length);
	}

	std::vector<std::string> Tokenize(const std::string& string)
	{
		return SplitString(string, " \t\n");
	}

	bool StartsWith(const std::string& string, const std::string& start)
	{
		return string.find(start) == 0;
	}

	std::string GetSuffix(const std::string& name)
	{
		const std::size_t pos = name.rfind('.');

		return (pos == std::string::npos) ? "" : name.substr(name.rfind('.') + 1);
	}

	std::string ToLower(std::string string)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);

		return string;
	}
}

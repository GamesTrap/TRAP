#pragma once

namespace TRAP::Utils
{
	std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
	std::vector<std::string> SplitString(const std::string& string, char delimiter);

	std::vector<std::string> GetLines(const std::string& string);

	const char* FindToken(const char* str, const std::string& token);
	const char* FindToken(const std::string& string, const std::string& token);

	std::string GetBlock(const char* str, const char** outPosition = nullptr);
	std::string GetBlock(const std::string& string, unsigned int offset = 0);

	std::string GetStatement(const char* str, const char** outPosition = nullptr);

	std::vector<std::string> Tokenize(const std::string& string);

	bool StartsWith(const std::string& string, const std::string& start);

	std::string GetSuffix(const std::string& name);

	std::string ToLower(std::string string);
}

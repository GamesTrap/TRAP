#ifndef _TRAP_STRING_H_
#define _TRAP_STRING_H_

namespace TRAP::Utils::String
{
	std::vector<std::string_view> SplitStringView(std::string_view string, std::string_view delimiters);
	std::vector<std::string_view> SplitStringView(std::string_view string, char delimiter);
	std::vector<std::string> SplitString(std::string_view string, std::string_view delimiters);
	std::vector<std::string> SplitString(std::string_view string, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	std::vector<std::string_view> GetLinesStringView(std::string_view string);
	std::vector<std::string> GetLines(std::string_view string);

	//-------------------------------------------------------------------------------------------------------------------//

	const char* FindToken(const char* str, std::string_view token);
	const char* FindToken(std::string_view string, std::string_view token);
	
	//-------------------------------------------------------------------------------------------------------------------//

	std::string_view GetBlockStringView(const char* str, const char** outPosition = nullptr);
	std::string_view GetBlockStringView(std::string_view string, uint32_t offset = 0);
	std::string GetBlock(const char* str, const char** outPosition = nullptr);
	std::string GetBlock(std::string_view string, uint32_t offset = 0);

	//-------------------------------------------------------------------------------------------------------------------//

	std::string_view GetStatementStringView(const char* str, const char** outPosition = nullptr);
	std::string GetStatement(const char* str, const char** outPosition = nullptr);

	//-------------------------------------------------------------------------------------------------------------------//

	std::vector<std::string_view> TokenizeStringView(std::string_view string);
	std::vector<std::string> Tokenize(std::string_view string);

	//-------------------------------------------------------------------------------------------------------------------//

	bool StartsWith(std::string_view string, std::string_view start);

	//-------------------------------------------------------------------------------------------------------------------//

	std::string_view GetSuffixStringView(std::string_view name);
	std::string GetSuffix(std::string_view name);

	//-------------------------------------------------------------------------------------------------------------------//

	std::string ToLower(std::string string);

	//-------------------------------------------------------------------------------------------------------------------//

	std::string ToUpper(std::string string);
	
	//-------------------------------------------------------------------------------------------------------------------//

	uint32_t GetCount(std::string_view string, char delimiter);
	
	//-------------------------------------------------------------------------------------------------------------------//

	bool CompareAnyCase(std::string_view left, std::string_view right);
}

#endif /*_TRAP_STRING_H_*/
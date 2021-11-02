#ifndef TRAP_STRING_H
#define TRAP_STRING_H

namespace TRAP::Utils::String
{
	/// <summary>
	/// Split a string by the delimiter(s).
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiters">Delimiter(s) to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter(s).</returns>
	std::vector<std::string_view> SplitStringView(std::string_view string, std::string_view delimiters);
	/// <summary>
	/// Split a string by the delimiter.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiter">Delimiter to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter.</returns>
	std::vector<std::string_view> SplitStringView(std::string_view string, char delimiter);
	/// <summary>
	/// Split a string by the delimiter(s).
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiters">Delimiter(s) to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter(s).</returns>
	std::vector<std::string> SplitString(std::string_view string, std::string_view delimiters);
	/// <summary>
	/// Split a string by the delimiter.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiter">Delimiter to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter.</returns>
	std::vector<std::string> SplitString(std::string_view string, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Split a string on new line.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <returns>Vector of strings splitted by new lines.</returns>
	std::vector<std::string_view> GetLinesStringView(std::string_view string);
	/// <summary>
	/// Split a string on new line.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <returns>Vector of strings splitted by new lines.</returns>
	std::vector<std::string> GetLines(std::string_view string);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Find a token in a string.
	/// </summary>
	/// <param name="str">String to query.</param>
	/// <param name="token">Token to find in string.</param>
	/// <returns>If found string starting on first occurrence of the token, nullptr otherwise.</returns>
	const char* FindToken(const char* str, std::string_view token);
	/// <summary>
	/// Find a token in a string.
	/// </summary>
	/// <param name="string">String to query.</param>
	/// <param name="token">Token to find in string.</param>
	/// <returns>If found string starting on first occurrence of the token, nullptr otherwise.</returns>
	const char* FindToken(std::string_view string, std::string_view token);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Get a block encapsulated with a '}' from a string.
	/// </summary>
	/// <param name="str">String to get block from.</param>
	/// <param name="outPosition">Override the '}' with own char/string.</param>
	/// <returns>If closing '}' was found returns the block, unmodified input string otherwise.</returns>
	std::string_view GetBlockStringView(const char* str, const char** outPosition = nullptr);
	/// <summary>
	/// Get a block encapsulated with a "}" from a string.
	/// </summary>
	/// <param name="string">String to get block from.</param>
	/// <param name="offset">Offset into the given string to use.</param>
	/// <returns>On success block, unmodified input string otherwise.</returns>
	std::string_view GetBlockStringView(std::string_view string, uint32_t offset = 0);
	/// <summary>
	/// Get a block encapsulated with a '}' from a string.
	/// </summary>
	/// <param name="str">String to get block from.</param>
	/// <param name="outPosition">Override the '}' with own char/string.</param>
	/// <returns>If closing '}' was found returns the block, unmodified input string otherwise.</returns>
	std::string GetBlock(const char* str, const char** outPosition = nullptr);
	/// <summary>
	/// Get a block encapsulated with a '}' from a string.
	/// </summary>
	/// <param name="string">String to get block from.</param>
	/// <param name="offset">Offset into the given string to use.</param>
	/// <returns>On success block, unmodified input string otherwise.</returns>
	std::string GetBlock(std::string_view string, uint32_t offset = 0);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Get a statement ending with a ';' from a string.
	/// </summary>
	/// <param name="str">String to get statement from.</param>
	/// <param name="outPosition">Override the ';' with own char/string.</param>
	/// <returns>If ending ';' was found return the statement, unmodified input string otherwise.</returns>
	std::string_view GetStatementStringView(const char* str, const char** outPosition = nullptr);
	/// <summary>
	/// Get a statement ending with a ';' from a string.
	/// </summary>
	/// <param name="str">String to get statement from.</param>
	/// <param name="outPosition">Override the ';' with own char/string.</param>
	/// <returns>If ending ';' was found return the statement, unmodified input string otherwise.</returns>
	std::string GetStatement(const char* str, const char** outPosition = nullptr);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Tokenize a string.
	/// </summary>
	/// <param name="string">String to tokenize.</param>
	/// <returns>Vector of strings splitted by " \t\n".</returns>
	std::vector<std::string_view> TokenizeStringView(std::string_view string);
	/// <summary>
	/// Tokenize string.
	/// </summary>
	/// <param name="string">String to tokenize.</param>
	/// <returns>Vector of strings splitted by " \t\n".</returns>
	std::vector<std::string> Tokenize(std::string_view string);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a string starts with contents another string or char.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="start">Content to check.</param>
	/// <returns>If content is inside the string to check against true, false otherwise.</returns>
	bool StartsWith(std::string_view string, std::string_view start);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Get the suffix of a string(everything after the last '.' in the string).
	/// </summary>
	/// <param name="name">String to get suffix from.</param>
	/// <returns>Suffix if found, nullptr otherwise.</returns>
	std::string_view GetSuffixStringView(std::string_view name);
	/// <summary>
	/// Get the suffix of a string(everything after the last '.' in the string).
	/// </summary>
	/// <param name="name">String to get suffix from.</param>
	/// <returns>Suffix if found, empty string otherwise.</returns>
	std::string GetSuffix(std::string_view name);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to lower case.
	/// </summary>
	/// <param name="string">String to convert.</param>
	/// <returns>Lower case version of the given string.</returns>
	std::string ToLower(const std::string& string);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to upper case.
	/// </summary>
	/// <param name="string">String to convert.</param>
	/// <returns>Upper case version of the given string.</returns>
	std::string ToUpper(const std::string& string);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Count the occurrences of the given delimiter in a string.
	/// </summary>
	/// <param name="string">String to count occurrences.</param>
	/// <param name="delimiter">Delimiter to find.</param>
	/// <returns>Number of occurrences of the delimiter inside the given string.</returns>
	uint32_t GetCount(std::string_view string, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if two strings are equal without case sensitivity.
	/// </summary>
	/// <param name="left">String to check.</param>
	/// <param name="right">String to check.</param>
	/// <returns>True if both strings are equal, false otherwise</returns>
	bool CompareAnyCase(std::string_view left, std::string_view right);
}

#endif /*TRAP_STRING_H*/
#ifndef TRAP_STRING_H
#define TRAP_STRING_H

#include "Window/Window.h"
#include "Graphics/API/RendererAPI.h"
#include "FileSystem/FileWatcher.h"
#include "Scene/Components.h"

namespace TRAP::Utils::String
{
	/// <summary>
	/// Split a string by the delimiter(s).
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiters">Delimiter(s) to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter(s).</returns>
	[[nodiscard]] constexpr std::vector<std::string_view> SplitStringView(std::string_view str, std::string_view delimiters);
	/// <summary>
	/// Split a string by the delimiter.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiter">Delimiter to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter.</returns>
	[[nodiscard]] constexpr std::vector<std::string_view> SplitStringView(std::string_view str, char delimiter);
	/// <summary>
	/// Split a string by the delimiter(s).
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiters">Delimiter(s) to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter(s).</returns>
	[[nodiscard]] constexpr std::vector<std::string> SplitString(const std::string& str, std::string_view delimiters);
	/// <summary>
	/// Split a string by the delimiter.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiter">Delimiter to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter.</returns>
	[[nodiscard]] constexpr std::vector<std::string> SplitString(const std::string& str, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Split a string on new line.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <returns>Vector of strings splitted by new lines.</returns>
	[[nodiscard]] constexpr std::vector<std::string_view> GetLinesStringView(std::string_view str);
	/// <summary>
	/// Split a string on new line.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <returns>Vector of strings splitted by new lines.</returns>
	[[nodiscard]] constexpr std::vector<std::string> GetLines(const std::string& str);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a string begins with the given prefix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="start">Prefix to check.</param>
	/// <returns>True if the string begins with the provided prefix, false otherwise.</returns>
	[[nodiscard]] constexpr bool StartsWith(std::string_view str, std::string_view start);

	/// <summary>
	/// Check if a string begins with the given prefix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="c">Prefix to check.</param>
	/// <returns>True if the string begins with the provided prefix, false otherwise.</returns>
	[[nodiscard]] constexpr bool StartsWith(std::string_view str, char c);

	/// <summary>
	/// Check if a string begins with the given prefix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="cstr">Prefix to check.</param>
	/// <returns>True if the string begins with the provided prefix, false otherwise.</returns>
	[[nodiscard]] constexpr bool StartsWith(std::string_view str, const char* cstr);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a string ends with the given suffix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="end">Suffix to check.</param>
	/// <returns>True if the string ends with the provided suffix, false otherwise.</returns>
	[[nodiscard]] constexpr bool EndsWith(std::string_view str, std::string_view end);

	/// <summary>
	/// Check if a string ends with the given suffix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="c">Suffix to check.</param>
	/// <returns>True if the string ends with the provided suffix, false otherwise.</returns>
	[[nodiscard]] constexpr bool EndsWith(std::string_view str, char c);

	/// <summary>
	/// Check if a string ends with the given suffix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="cstr">Suffix to check.</param>
	/// <returns>True if the string ends with the provided suffix, false otherwise.</returns>
	[[nodiscard]] constexpr bool EndsWith(std::string_view str, const char* cstr);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Checks if the string contains the given substring.
	/// </summary>
	/// <param name="str">String to check.</param>
	/// <param name="substr">Substring to check if inside string.</param>
	/// <returns>True if the string contains the provided substring, false otherwise.</returns>
	[[nodiscard]] constexpr bool Contains(std::string_view str, std::string_view substr) noexcept;

	/// <summary>
	/// Checks if the string contains the given substring.
	/// </summary>
	/// <param name="str">String to check.</param>
	/// <param name="c">Substring to check if inside string.</param>
	/// <returns>True if the string contains the provided substring, false otherwise.</returns>
	[[nodiscard]] constexpr bool Contains(std::string_view str, char c) noexcept;

	/// <summary>
	/// Checks if the string contains the given substring.
	/// </summary>
	/// <param name="str">String to check.</param>
	/// <param name="substr">Substring to check if inside string.</param>
	/// <returns>True if the string contains the provided substring, false otherwise.</returns>
	[[nodiscard]] constexpr bool Contains(std::string_view str, const char* substr);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Get the suffix of a string(everything after the last '.' in the string).
	/// </summary>
	/// <param name="name">String to get suffix from.</param>
	/// <returns>Suffix if found, nullptr otherwise.</returns>
	[[nodiscard]] constexpr std::string_view GetSuffixStringView(std::string_view name);
	/// <summary>
	/// Get the suffix of a string(everything after the last '.' in the string).
	/// </summary>
	/// <param name="name">String to get suffix from.</param>
	/// <returns>Suffix if found, empty string otherwise.</returns>
	[[nodiscard]] constexpr std::string GetSuffix(const std::string& name);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to lower case.
	/// </summary>
	/// <param name="string">String to convert.</param>
	/// <returns>Lower case version of the given string.</returns>
	[[nodiscard]] constexpr std::string ToLower(std::string str);

	/// <summary>
	/// Convert a character to lower case.
	/// </summary>
	/// <param name="c">Character to convert.</param>
	/// <returns>Lower case version of the given character.</returns>
	[[nodiscard]] constexpr char ToLower(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to upper case.
	/// </summary>
	/// <param name="string">String to convert.</param>
	/// <returns>Upper case version of the given string.</returns>
	[[nodiscard]] constexpr std::string ToUpper(std::string str);

	/// <summary>
	/// Convert a character to upper case.
	/// </summary>
	/// <param name="c">Character to convert.</param>
	/// <returns>Upper case version of the given character.</returns>
	[[nodiscard]] constexpr char ToUpper(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a character is a digit (0123456789).
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>True if character is a digit, false otherwise.</returns>
	[[nodiscard]] constexpr bool IsDigit(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a character is a hexdecimal digit (0123456789abcdefABCDEF).
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>True if character is a hexdecimal digit, false otherwise.</returns>
	[[nodiscard]] constexpr bool IsHexDigit(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a character is a whitespace character.
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>True if character is a whitespace character, false otherwise.</returns>
	[[nodiscard]] constexpr bool IsSpace(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a character is an alphabetic character.
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>True if character is an alphabetic character, false otherwise.</returns>
	[[nodiscard]] constexpr bool IsAlpha(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a character is an alphanumeric character.
	/// </summary>
	/// <param name="c">Character to check.</param>
	/// <returns>True if character is an alphanumeric character, false otherwise.</returns>
	[[nodiscard]] constexpr bool IsAlphaNumeric(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Count the occurrences of the given delimiter in a string.
	/// </summary>
	/// <param name="string">String to count occurrences.</param>
	/// <param name="delimiter">Delimiter to find.</param>
	/// <returns>Number of occurrences of the delimiter inside the given string.</returns>
	[[nodiscard]] constexpr int64_t GetCount(std::string_view str, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if two strings are equal without case sensitivity.
	/// </summary>
	/// <param name="left">String to check.</param>
	/// <param name="right">String to check.</param>
	/// <returns>True if both strings are equal, false otherwise</returns>
	[[nodiscard]] constexpr bool CompareAnyCase(std::string_view left, std::string_view right);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to the given type.
	/// </summary>
	/// <typeparam name="T">Type to get.</typeparam>
	/// <param name="input">String to convert.</param>
	/// <returns>String converted to type T.</returns>
	template<typename T>
	[[nodiscard]] T ConvertToType(const std::string& input);
	/// <summary>
	/// Convert a value of type T to a string.
	/// </summary>
	/// <typeparam name="T">Type to convert.</typeparam>
	/// <param name="value">Value to convert.</param>
	/// <returns>Type T value converted to string.</returns>
	template<typename T>
	[[nodiscard]] std::string ConvertToString(T value);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Retrieve the given time-stamp as a string with "HH:MM:SS" format.
	/// </summary>
	/// <param name="timePoint">Time-stamp to convert.</param>
	/// <returns>Time-stamp as string.</returns>
	[[nodiscard]] std::string GetTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timePoint) noexcept;
	/// <summary>
	/// Retrieve the given date-time-stamp as a string with "YYYY-MM-DD HH:MM:SS" format.
	/// </summary>
	/// <param name="timePoint">Timestamp to convert.</param>
	/// <returns>Timestamp as string.</returns>
	[[nodiscard]] std::string GetDateTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timePoint) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Retrieve the last set OS error message.
	/// </summary>
	/// <returns>Last OS error message.</returns>
	[[nodiscard]] std::string GetStrError();

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Encode a single Unicode code point to UTF-8 chars.
	/// </summary>
	/// <param name="codePoint">Unicode code point</param>
	/// <returns>UTF-8 chars.</returns>
	[[nodiscard]] constexpr std::string EncodeUTF8(uint32_t codePoint);

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
	/// <summary>
	/// Returns an UTF-8 string version of the specified wide string.
	/// </summary>
	/// <param name="wStr">Wide string representation.</param>
	/// <returns>UTF-8 string representation of the given wide string.</returns>
	[[nodiscard]] std::string CreateUTF8StringFromWideStringWin32(std::wstring_view wStr);
	/// <summary>
	/// Returns a wide string string version of the specified UTF-8 string.
	/// </summary>
	/// <param name="str">UTF-8 representation.</param>
	/// <returns>Wide string representation of the given UTF-8 string.</returns>
	[[nodiscard]] std::wstring CreateWideStringFromUTF8StringWin32(std::string_view str);
#endif /*TRAP_PLATFORM_WINDOWS*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view str,
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

[[nodiscard]] constexpr std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view str,
                                                                                           const char delimiter)
{
	return SplitStringView(str, std::string_view(&delimiter, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string> TRAP::Utils::String::SplitString(const std::string& str,
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

[[nodiscard]] constexpr std::vector<std::string> TRAP::Utils::String::SplitString(const std::string& str, const char delimiter)
{
	return SplitString(str, std::string_view(&delimiter, 1));
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string_view> TRAP::Utils::String::GetLinesStringView(const std::string_view str)
{
	return SplitStringView(str, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string> TRAP::Utils::String::GetLines(const std::string& str)
{
	return SplitString(str, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::StartsWith(const std::string_view str, const std::string_view start)
{
	return str.starts_with(start);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::StartsWith(const std::string_view str, const char c)
{
	return str.starts_with(c);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::StartsWith(const std::string_view str, const char* cstr)
{
	return str.starts_with(cstr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::EndsWith(const std::string_view str, const std::string_view end)
{
	return str.ends_with(end);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::EndsWith(const std::string_view str, const char c)
{
	return str.ends_with(c);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::EndsWith(const std::string_view str, const char* cstr)
{
	return str.ends_with(cstr);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::Contains(std::string_view str, std::string_view substr) noexcept
{
	return str.find(substr) != std::string_view::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::Contains(std::string_view str, char c) noexcept
{
	return str.find(c) != std::string_view::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::Contains(std::string_view str, const char* substr)
{
	return str.find(substr) != std::string_view::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string_view TRAP::Utils::String::GetSuffixStringView(const std::string_view name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? std::string_view() : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Utils::String::GetSuffix(const std::string& name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Utils::String::ToLower(std::string str)
{
	for(char& c : str)
		c = NumericCast<char>(::tolower(c));

	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr char TRAP::Utils::String::ToLower(const char c)
{
	return (c >= 'A' && c <= 'Z') ? NumericCast<char>(c - 'A' + 'a') : c;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr char TRAP::Utils::String::ToUpper(const char c)
{
	return (c >= 'a' && c <= 'z') ? NumericCast<char>(c - 'a' + 'A') : c;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Utils::String::ToUpper(std::string str)
{
	for(char& c : str)
		c = NumericCast<char>(::toupper(c));

	return str;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::IsDigit(const char c)
{
	return (c >= '0' && c <= '9');
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::IsHexDigit(const char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::IsSpace(const char c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::IsAlpha(const char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::IsAlphaNumeric(const char c)
{
	return (c >= '0' && c <= '9') || IsAlpha(c);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr int64_t TRAP::Utils::String::GetCount(const std::string_view str, const char delimiter)
{
	return std::ranges::count(str, delimiter);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Utils::String::CompareAnyCase(const std::string_view left, const std::string_view right)
{
	if (left.size() != right.size())
		return false;

	for (auto c1 = left.cbegin(), c2 = right.cbegin(); c1 != left.cend(); ++c1, ++c2)
	{
		if (ToLower(*c1) != ToLower(*c2))
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Utils::String::EncodeUTF8(const uint32_t codePoint)
{
	std::string result{};
	result.reserve(4);

	if (codePoint < 0x80u)
		result.push_back(NumericCast<char>(codePoint));
	else if (codePoint < 0x800u)
	{
		result.push_back(NumericCast<char>((codePoint >> 6u) | 0xC0u));
		result.push_back(NumericCast<char>((codePoint & 0x3Fu) | 0x80u));
	}
	else if (codePoint < 0x10000u)
	{
		result.push_back(NumericCast<char>((codePoint >> 12u) | 0xE0u));
		result.push_back(NumericCast<char>(((codePoint >> 6u) & 0x3Fu) | 0x80u));
		result.push_back(NumericCast<char>((codePoint & 0x3Fu) | 0x80u));
	}
	else if (codePoint < 0x110000u)
	{
		result.push_back(NumericCast<char>((codePoint >> 18u) | 0xF0u));
		result.push_back(NumericCast<char>(((codePoint >> 12u) & 0x3Fu) | 0x80u));
		result.push_back(NumericCast<char>(((codePoint >> 6u) & 0x3Fu) | 0x80u));
		result.push_back(NumericCast<char>((codePoint & 0x3Fu) | 0x80u));
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

#include "String.inl"

#endif /*TRAP_STRING_H*/
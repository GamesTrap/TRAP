#ifndef TRAP_STRING_H
#define TRAP_STRING_H

#include "Core/Types.h"
#include "Window/Window.h"
#include "Graphics/API/RendererAPI.h"
#include "FileSystem/FileSystemWatcher.h"
#include "Scene/Components.h"
#include "Utils/NumericCasts.h"

namespace TRAP::Utils::String
{
	/// @brief Split a string by the delimiter(s).
	/// @param str String to split up.
	/// @param delimiters Delimiter(s) to split source string with.
	/// @return Vector of strings splitted by the given delimiter(s).
	[[nodiscard]] constexpr std::vector<std::string_view> SplitStringView(std::string_view str, std::string_view delimiters);
	/// @brief Split a string by the delimiter.
	/// @param str String to split up.
	/// @param delimiter Delimiter to split source string with.
	/// @return Vector of strings splitted by the given delimiter.
	[[nodiscard]] constexpr std::vector<std::string_view> SplitStringView(std::string_view str, char delimiter);
	/// @brief Split a string by the delimiter(s).
	/// @param str String to split up.
	/// @param delimiters Delimiter(s) to split source string with.
	/// @return Vector of strings splitted by the given delimiter(s).
	[[nodiscard]] constexpr std::vector<std::string> SplitString(const std::string& str, std::string_view delimiters);
	/// @brief Split a string by the delimiter.
	/// @param str String to split up.
	/// @param delimiter Delimiter to split source string with.
	/// @return Vector of strings splitted by the given delimiter.
	[[nodiscard]] constexpr std::vector<std::string> SplitString(const std::string& str, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Split a string on new line.
	/// @param str String to split up.
	/// @return Vector of strings splitted by new lines.
	[[nodiscard]] constexpr std::vector<std::string_view> GetLinesStringView(std::string_view str);
	/// @brief Split a string on new line.
	/// @param str String to split up.
	/// @return Vector of strings splitted by new lines.
	[[nodiscard]] constexpr std::vector<std::string> GetLines(const std::string& str);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Checks if the string contains the given substring.
	/// @param str String to check.
	/// @param substr Substring to check if inside string.
	/// @return True if the string contains the provided substring, false otherwise.
	/// @deprecated This function is deprecated when compiling with C++23 standard.
#if __cplusplus >= 202302L
	[[deprecated("Use std::string::contains() instead!")]]
#endif /*__cplusplus >= 202302L*/
	[[nodiscard]] constexpr bool Contains(std::string_view str, std::string_view substr) noexcept;

	/// @brief Checks if the string contains the given substring.
	/// @param str String to check.
	/// @param c Substring to check if inside string.
	/// @return True if the string contains the provided substring, false otherwise.
	/// @deprecated This function is deprecated when compiling with C++23 standard.
#if __cplusplus >= 202302L
	[[deprecated("Use std::string::contains() instead!")]]
#endif /*__cplusplus >= 202302L*/
	[[nodiscard]] constexpr bool Contains(std::string_view str, char c) noexcept;

	/// @brief Checks if the string contains the given substring.
	/// @param str String to check.
	/// @param substr Substring to check if inside string.
	/// @return True if the string contains the provided substring, false otherwise.
	/// @deprecated This function is deprecated when compiling with C++23 standard.
#if __cplusplus >= 202302L
	[[deprecated("Use std::string::contains() instead!")]]
#endif /*__cplusplus >= 202302L*/
	[[nodiscard]] constexpr bool Contains(std::string_view str, const char* substr);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Get the suffix of a string(everything after the last '.' in the string).
	/// @param name String to get suffix from.
	/// @return Suffix if found, nullptr otherwise.
	[[nodiscard]] constexpr std::string_view GetSuffixStringView(std::string_view name);
	/// @brief Get the suffix of a string(everything after the last '.' in the string).
	/// @param name String to get suffix from.
	/// @return Suffix if found, empty string otherwise.
	[[nodiscard]] constexpr std::string GetSuffix(const std::string& name);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert a string to lower case.
	/// @param str String to convert.
	/// @return Lower case version of the given string.
	[[nodiscard]] constexpr std::string ToLower(std::string str);

	/// @brief Convert a character to lower case.
	/// @param c Character to convert.
	/// @return Lower case version of the given character.
	[[nodiscard]] constexpr char ToLower(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert a string to upper case.
	/// @param str String to convert.
	/// @return Upper case version of the given string.
	[[nodiscard]] constexpr std::string ToUpper(std::string str);

	/// @brief Convert a character to upper case.
	/// @param c Character to convert.
	/// @return Upper case version of the given character.
	[[nodiscard]] constexpr char ToUpper(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if a character is a digit (0123456789).
	/// @param c Character to check.
	/// @return True if character is a digit, false otherwise.
	[[nodiscard]] constexpr bool IsDigit(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if a character is a hexdecimal digit (0123456789abcdefABCDEF).
	/// @param c Character to check.
	/// @return True if character is a hexdecimal digit, false otherwise.
	[[nodiscard]] constexpr bool IsHexDigit(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if a character is a whitespace character.
	/// @param c Character to check.
	/// @return True if character is a whitespace character, false otherwise.
	[[nodiscard]] constexpr bool IsSpace(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if a character is an alphabetic character.
	/// @param c Character to check.
	/// @return True if character is an alphabetic character, false otherwise.
	[[nodiscard]] constexpr bool IsAlpha(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if a character is an alphanumeric character.
	/// @param c Character to check.
	/// @return True if character is an alphanumeric character, false otherwise.
	[[nodiscard]] constexpr bool IsAlphaNumeric(char c);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Count the occurrences of the given delimiter in a string.
	/// @param str String to count occurrences.
	/// @param delimiter Delimiter to find.
	/// @return Number of occurrences of the delimiter inside the given string.
	[[nodiscard]] constexpr i64 GetCount(std::string_view str, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Check if two strings are equal without case sensitivity.
	/// @param left String to check.
	/// @param right String to check.
	/// @return True if both strings are equal, false otherwise
	[[nodiscard]] constexpr bool CompareAnyCase(std::string_view left, std::string_view right);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Convert a string to the given type.
	/// @tparam T Type to get
	/// @param input String to convert.
	/// @return String converted to type T.
	template<typename T>
	[[nodiscard]] T ConvertToType(const std::string& input);

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the given time-stamp as a string with "HH:MM:SS" format.
	/// @param timePoint Time-stamp to convert.
	/// @return Time-stamp as string.
	[[nodiscard]] std::string GetTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timePoint) noexcept;
	/// @brief Retrieve the given date-time-stamp as a string with "YYYY-MM-DD HH:MM:SS" format.
	/// @param timePoint Timestamp to convert.
	/// @return Timestamp as string.
	[[nodiscard]] std::string GetDateTimeStamp(const std::chrono::time_point<std::chrono::system_clock>& timePoint) noexcept;

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the last set OS error message.
	/// @return Last OS error message.
	[[nodiscard]] std::string GetStrError();

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Encode a single Unicode code point to UTF-8 chars.
	/// @param codePoint Unicode code point
	/// @return UTF-8 chars.
	[[nodiscard]] constexpr std::string EncodeUTF8(u32 codePoint);

	//-------------------------------------------------------------------------------------------------------------------//

#ifdef TRAP_PLATFORM_WINDOWS
	/// @brief Returns an UTF-8 string version of the specified wide string.
	/// @param wStr Wide string representation.
	/// @return UTF-8 string representation of the given wide string.
	[[nodiscard]] std::string CreateUTF8StringFromWideStringWin32(std::wstring_view wStr);
	/// @brief Returns a wide string string version of the specified UTF-8 string.
	/// @param str UTF-8 representation.
	/// @return Wide string representation of the given UTF-8 string.
	[[nodiscard]] std::wstring CreateWideStringFromUTF8StringWin32(std::string_view str);
#endif /*TRAP_PLATFORM_WINDOWS*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string_view> TRAP::Utils::String::SplitStringView(const std::string_view str,
                                                                                           const std::string_view delimiters)
{
	usize start = 0;
	usize end = str.find_first_of(delimiters);

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
	usize start = 0;
	usize end = str.find_first_of(delimiters);

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
	const usize pos = name.rfind('.');

	return (pos == std::string::npos) ? std::string_view() : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Utils::String::GetSuffix(const std::string& name)
{
	const usize pos = name.rfind('.');

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

[[nodiscard]] constexpr i64 TRAP::Utils::String::GetCount(const std::string_view str, const char delimiter)
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

[[nodiscard]] constexpr std::string TRAP::Utils::String::EncodeUTF8(const u32 codePoint)
{
	std::string result{};
	result.reserve(4);

	if (codePoint < 0x80u)
		result.push_back(NumericCast<char>(codePoint));
	else if (codePoint < 0x800u)
	{
		result.push_back(static_cast<char>((codePoint >> 6u) | 0xC0u));
		result.push_back(static_cast<char>((codePoint & 0x3Fu) | 0x80u));
	}
	else if (codePoint < 0x10000u)
	{
		result.push_back(static_cast<char>((codePoint >> 12u) | 0xE0u));
		result.push_back(static_cast<char>(((codePoint >> 6u) & 0x3Fu) | 0x80u));
		result.push_back(static_cast<char>((codePoint & 0x3Fu) | 0x80u));
	}
	else if (codePoint < 0x110000u)
	{
		result.push_back(static_cast<char>((codePoint >> 18u) | 0xF0u));
		result.push_back(static_cast<char>(((codePoint >> 12u) & 0x3Fu) | 0x80u));
		result.push_back(static_cast<char>(((codePoint >> 6u) & 0x3Fu) | 0x80u));
		result.push_back(static_cast<char>((codePoint & 0x3Fu) | 0x80u));
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

#include "String.inl"

#endif /*TRAP_STRING_H*/
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
	[[nodiscard]] std::vector<std::string_view> SplitStringView(std::string_view str, std::string_view delimiters);
	/// <summary>
	/// Split a string by the delimiter.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiter">Delimiter to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter.</returns>
	[[nodiscard]] std::vector<std::string_view> SplitStringView(std::string_view str, char delimiter);
	/// <summary>
	/// Split a string by the delimiter(s).
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiters">Delimiter(s) to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter(s).</returns>
	[[nodiscard]] std::vector<std::string> SplitString(const std::string& str, std::string_view delimiters);
	/// <summary>
	/// Split a string by the delimiter.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <param name="delimiter">Delimiter to split source string with.</param>
	/// <returns>Vector of strings splitted by the given delimiter.</returns>
	[[nodiscard]] std::vector<std::string> SplitString(const std::string& str, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Split a string on new line.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <returns>Vector of strings splitted by new lines.</returns>
	[[nodiscard]] std::vector<std::string_view> GetLinesStringView(std::string_view str);
	/// <summary>
	/// Split a string on new line.
	/// </summary>
	/// <param name="string">String to split up.</param>
	/// <returns>Vector of strings splitted by new lines.</returns>
	[[nodiscard]] std::vector<std::string> GetLines(const std::string& str);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if a string begins with the given prefix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="start">Prefix to check.</param>
	/// <returns>True if the string begins with the provided prefix, false otherwise.</returns>
	[[nodiscard]] bool StartsWith(std::string_view str, std::string_view start);

	/// <summary>
	/// Check if a string ends with the given suffix.
	/// </summary>
	/// <param name="string">String to check.</param>
	/// <param name="end">Suffix to check.</param>
	/// <returns>True if the string ends with the provided suffix, false otherwise.</returns>
	[[nodiscard]] bool EndsWith(std::string_view str, std::string_view end);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Get the suffix of a string(everything after the last '.' in the string).
	/// </summary>
	/// <param name="name">String to get suffix from.</param>
	/// <returns>Suffix if found, nullptr otherwise.</returns>
	[[nodiscard]] std::string_view GetSuffixStringView(std::string_view name);
	/// <summary>
	/// Get the suffix of a string(everything after the last '.' in the string).
	/// </summary>
	/// <param name="name">String to get suffix from.</param>
	/// <returns>Suffix if found, empty string otherwise.</returns>
	[[nodiscard]] std::string GetSuffix(const std::string& name);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to lower case.
	/// </summary>
	/// <param name="string">String to convert.</param>
	/// <returns>Lower case version of the given string.</returns>
	[[nodiscard]] std::string ToLower(std::string str);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Convert a string to upper case.
	/// </summary>
	/// <param name="string">String to convert.</param>
	/// <returns>Upper case version of the given string.</returns>
	[[nodiscard]] std::string ToUpper(std::string str);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Count the occurrences of the given delimiter in a string.
	/// </summary>
	/// <param name="string">String to count occurrences.</param>
	/// <param name="delimiter">Delimiter to find.</param>
	/// <returns>Number of occurrences of the delimiter inside the given string.</returns>
	[[nodiscard]] int64_t GetCount(std::string_view str, char delimiter);

	//-------------------------------------------------------------------------------------------------------------------//

	/// <summary>
	/// Check if two strings are equal without case sensitivity.
	/// </summary>
	/// <param name="left">String to check.</param>
	/// <param name="right">String to check.</param>
	/// <returns>True if both strings are equal, false otherwise</returns>
	[[nodiscard]] bool CompareAnyCase(std::string_view left, std::string_view right);

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
	[[nodiscard]] std::string EncodeUTF8(uint32_t codePoint);

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

#include "String.inl"

#endif /*TRAP_STRING_H*/
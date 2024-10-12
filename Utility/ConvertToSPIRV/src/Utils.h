#ifndef CONVERTTOSPIRV_UTILS_H
#define CONVERTTOSPIRV_UTILS_H

#include <string>
#include <filesystem>
#include <vector>
#include <optional>
#include <iostream>
#include <fstream>

#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/std.h>

#include "Types.h"

[[nodiscard]] constexpr std::unsigned_integral auto BIT(const std::unsigned_integral auto x) noexcept
{
	return decltype(x)(1u) << x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool FileOrFolderExists(const std::filesystem::path& path)
{
    if(path.empty())
        return false;

    std::error_code ec{};
    const bool res = std::filesystem::exists(path, ec);

    if(ec)
    {
        fmt::print(fg(fmt::color::red), "Couldn't check if file or folder exists: {} ({})\n", path, ec.message());
        return false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::optional<bool> FileHasExtension(const std::filesystem::path& filePath,
                                                          const std::string_view extension)
{
    if(!FileOrFolderExists(filePath))
    {
        fmt::print(fg(fmt::color::red), "File {} doesn't exist!\n", filePath);
        return std::nullopt;
    }

    return filePath.extension() == extension;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::optional<std::string> ReadTextFile(const std::filesystem::path& filePath)
{
    if(!FileOrFolderExists(filePath))
        return std::nullopt;

    std::ifstream file(filePath);
    if(!file.is_open() || !file.good())
    {
        fmt::print(fg(fmt::color::red), "Couldn't open file: {}\n", filePath);
        return std::nullopt;
    }

    std::string line, result;
    while (file.good() && std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        result += line + '\n';
    }

    return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::optional<std::vector<u8>> ReadFile(const std::filesystem::path& filePath)
{
    if(!FileOrFolderExists(filePath))
        return std::nullopt;

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if(!file.is_open() || !file.good())
    {
        fmt::print(fg(fmt::color::red), "Couldn't open file: {}\n", filePath);
        return std::nullopt;
    }

    const usize fileSize = static_cast<usize>(file.tellg());

    file.seekg(std::ios::beg);

    std::vector<u8> data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size() * sizeof(u8)));

    return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string> SplitString(const std::string_view string, const std::string_view delimiters)
{
	usize start = 0u;
	usize end = string.find_first_of(delimiters);

	std::vector<std::string> result;

	while (end <= std::string::npos)
	{
		const std::string_view token = string.substr(start, end - start);

		if (!token.empty())
			result.emplace_back(token);

		if (end == std::string::npos)
			break;

		start = end + 1u;
		end = string.find_first_of(delimiters, start);
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::vector<std::string> GetLines(const std::string_view string)
{
	return SplitString(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string GetSuffix(const std::string& name)
{
	const usize pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(pos + 1u);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr char ToLower(const char c)
{
	return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string ToLower(std::string string)
{
    for(char& c : string)
        c = static_cast<char>(ToLower(c));

    return string;
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef __cpp_lib_is_scoped_enum

namespace std
{
    template<typename E>
    struct is_scoped_enum : std::bool_constant<requires
    {
        requires std::is_enum_v<E>;
        requires !std::is_convertible_v<E, std::underlying_type_t<E>>;
    }>
    {};

    template<class T>
    constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
}

#endif

//-------------------------------------------------------------------------------------------------------------------//

#ifndef __cpp_lib_to_underlying

namespace std
{
    template<class Enum>
    requires std::is_enum_v<Enum> || std::is_scoped_enum_v<Enum>
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}

#endif /*__cpp_lib_to_underlying*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool Contains(const std::string_view str, const std::string_view substr) noexcept
{
	return str.find(substr) != std::string_view::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool Contains(const std::string_view str, const char c) noexcept
{
	return str.find(c) != std::string_view::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool Contains(const std::string_view str, const char* const substr)
{
	return str.find(substr) != std::string_view::npos;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
requires (std::unsigned_integral<T> && !std::same_as<T, u8>)
[[nodiscard]] constexpr T ConvertByte(const u8* const source)
{
    if constexpr (sizeof(T) == 2u)
    {
        return (static_cast<T>(source[0u])) | (static_cast<T>(source[1u]) << 8u);
    }
    else if constexpr (sizeof(T) == 4u)
    {
        return (static_cast<T>(source[0u])) | (static_cast<T>(source[1u]) << 8u) |
                (static_cast<T>(source[2u]) << 16u) | (static_cast<T>(source[3u]) << 24u);
    }
    else if constexpr (sizeof(T) == 8u)
    {
        return (static_cast<T>(source[0u])) | (static_cast<T>(source[1u]) << 8u) |
                (static_cast<T>(source[2u]) << 16u) | (static_cast<T>(source[3u]) << 24u) |
                (static_cast<T>(source[4u]) << 32u) | (static_cast<T>(source[5u]) << 40u) |
                (static_cast<T>(source[6u]) << 48u) | (static_cast<T>(source[7u]) << 56u);
    }
    else
    {
        static_assert(sizeof(T) == 0u, "T has unknown size for unsigned interger type");
        return {};
    }
}

#endif /*CONVERTTOSPIRV_UTILS_H*/

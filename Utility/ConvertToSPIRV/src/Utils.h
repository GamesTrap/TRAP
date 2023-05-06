#ifndef CONVERTTOSPIRV_UTILS_H
#define CONVERTTOSPIRV_UTILS_H

#include <string>
#include <filesystem>
#include <vector>
#include <optional>
#include <iostream>
#include <fstream>
#include <algorithm>

template <typename T>
[[nodiscard]] constexpr T BIT(T x) noexcept
{
	return T(1) << x;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool FileOrFolderExists(const std::filesystem::path& path)
{
    if(path.empty())
        return false;

    std::error_code ec;
    const bool res = std::filesystem::exists(path, ec);

    if(ec)
    {
        std::cerr << "Couldn't check if file or folder exists: \"" << path.u8string() << "\" (" << ec.message() << ")\n";
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
		std::cerr << "File \"" << filePath << "\" doesn't exist!" << std::endl;
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
		std::cerr << "Couldn't open file: \"" << filePath.u8string() << "\"\n";
        return std::nullopt;
    }

    std::string line, result;
    while (file.good() && std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        result += line + '\n';
    }
    file.close();

    return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::optional<std::vector<uint8_t>> ReadFile(const std::filesystem::path& filePath)
{
    if(!FileOrFolderExists(filePath))
        return std::nullopt;

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if(!file.is_open() || !file.good())
    {
		std::cerr << "Couldn't open file: \"" << filePath.u8string() << "\"\n";
        return std::nullopt;
    }

    const std::size_t fileSize = static_cast<std::size_t>(file.tellg());

    file.seekg(std::ios::beg);

    std::vector<uint8_t> data(fileSize);
    file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()) * sizeof(decltype(data)::value_type));

    return data;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
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

[[nodiscard]] inline std::vector<std::string> GetLines(const std::string& string)
{
	return SplitString(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline bool StartsWith(const std::string_view string, const std::string_view start)
{
	return string.substr(0, start.size()) == start;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::string GetSuffix(const std::string& name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] inline std::string ToLower(std::string string)
{
    for(char& c : string)
        c = static_cast<char>(::tolower(c));

    return string;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename Enum>
[[nodiscard]] inline constexpr std::underlying_type_t<Enum> ToUnderlying(const Enum e) noexcept
{
	return static_cast<std::underlying_type_t<Enum>>(e);
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] inline static T ConvertByte(const uint8_t* const source)
{
    if constexpr(std::is_unsigned_v<T> && (std::is_same_v<T, uint16_t> ||
                    std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>))
    {
        if constexpr (sizeof(T) == 2)
        {
            return (static_cast<T>(source[0])) | (static_cast<T>(source[1]) << 8u);
        }
        else if constexpr (sizeof(T) == 4)
        {
            return (static_cast<T>(source[0])) | (static_cast<T>(source[1]) << 8u) |
                    (static_cast<T>(source[2]) << 16u) | (static_cast<T>(source[3]) << 24u);
        }
        else if constexpr (sizeof(T) == 8)
        {
            return (static_cast<T>(source[0])) | (static_cast<T>(source[1]) << 8u) |
                    (static_cast<T>(source[2]) << 16u) | (static_cast<T>(source[3]) << 24u) |
                    (static_cast<T>(source[4]) << 32u) | (static_cast<T>(source[5]) << 40u) |
                    (static_cast<T>(source[6]) << 48u) | (static_cast<T>(source[7]) << 56u);
        }
    }
    else
    {
        static_assert(sizeof(T) == 0, "T must be unsigned interger type (not uint8_t");
        return {};
    }
}

#endif /*CONVERTTOSPIRV_UTILS_H*/
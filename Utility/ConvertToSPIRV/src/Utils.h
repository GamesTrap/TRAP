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
constexpr T BIT(T x)
{
	return T(1) << x;
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool FileOrFolderExists(const std::filesystem::path& path)
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

inline std::optional<std::string> ReadTextFile(const std::filesystem::path& filePath)
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

inline std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
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

inline std::vector<std::string> GetLines(const std::string& string)
{
	return SplitString(string, "\n");
}

//-------------------------------------------------------------------------------------------------------------------//

inline bool StartsWith(const std::string_view string, const std::string_view start)
{
	return string.find(start) == 0;
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string GetSuffix(const std::string& name)
{
	const std::size_t pos = name.rfind('.');

	return (pos == std::string::npos) ? "" : name.substr(pos + 1);
}

//-------------------------------------------------------------------------------------------------------------------//

inline std::string ToLower(std::string string)
{
	std::transform(string.begin(), string.end(), string.begin(), [](const int32_t c) -> char
	{
		return static_cast<char>(::tolower(c));
	});

	return string;
}

#endif /*CONVERTTOSPIRV_UTILS_H*/
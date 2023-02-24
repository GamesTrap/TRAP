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

#endif /*CONVERTTOSPIRV_UTILS_H*/
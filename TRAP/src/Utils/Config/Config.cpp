#include "TRAPPCH.h"
#include "Config.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

namespace
{
	/// @brief Parse a line from the Config files.
	/// Splits keys from value(s).
	/// Ignores lines starting with a '#'.
	/// @param line Line to parse.
	/// @return Pair of key and value.
	[[nodiscard]] constexpr std::pair<std::string, std::string> ParseLine(const std::string_view line)
	{
		//If this line is empty or a comment, return an empty pair
		if(line.empty() || line[0] == '#')
			return { "", "" };

		usize index = 0;
		//Trim leading whitespace
		while (TRAP::Utils::String::IsSpace(line[index]))
			index++;
		//Get the key string
		const usize beginKeyString = index;
		while (!TRAP::Utils::String::IsSpace(line[index]) && line[index] != '=')
			index++;
		const std::string key(line.data() + beginKeyString, index - beginKeyString);

		//Skip the assignment
		while (TRAP::Utils::String::IsSpace(line[index]) || line[index] == '=')
		{
			index++;
			if(index >= line.size())
			{
				//Out of range so line only contains key
				return { key, "" };
			}
		}

		//Get the value string
		const std::string value(line.data() + index, line.size() - index);

		//Return the key value pair
		return { key, value };
	}
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::LoadFromFile(const std::filesystem::path& file)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	m_data.clear();

	//Load
	const auto input = FileSystem::ReadTextFile(file);
	if (!input)
		return false;

	TP_INFO(TRAP::Log::ConfigPrefix, "Loading file: ", file);
	const std::vector<std::string_view> lines = String::SplitStringView(*input, '\n');

	for (const auto& line : lines)
	{
		if (!line.empty())
		{
			//Parse line
			const auto [key, value] = ParseLine(line);

			if (!key.empty())
			{
				//If the line is not empty or a comment save it to the vector
				m_data.emplace_back(key, value);
			}
		}
	}

	m_hasChanged = false;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::SaveToFile(const std::filesystem::path& file)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	if(!m_hasChanged)
		return true;

	m_hasChanged = false;

	//Write
	std::vector<std::pair<std::string, std::string>> fileContents;

	TP_INFO(TRAP::Log::ConfigPrefix, "Saving file: ", file);

	//Read the file into a vector and replace the values of the keys that match with our map
	const auto input = FileSystem::ReadTextFile(file);
	if (input)
	{
		const std::vector<std::string> lines = String::SplitString(*input, '\n');

		for (const auto& line : lines)
		{
			//Parse line
			auto pLine = ParseLine(line);
			auto& [key, value] = pLine;

			if (!key.empty())
			{
				//Check if the key is found in the vector
				const auto it = std::ranges::find_if(m_data,
					[&pLine](const std::pair<std::string, std::string>& element)
					{
						return Utils::String::CompareAnyCase(element.first, pLine.first);
					});
				if (it != m_data.end())
				{
					//If so take it's value, otherwise the value from the file is kept
					value = it->second;
				}
			}
			else
			{
				//If the line is empty or a comment simply take the whole line as the key
				key = line;
			}
			fileContents.emplace_back(key, value);
		}
	}
	else
	{
		//Can't open file for reading. Use only the data from the map
		for (const auto& [key, value] : m_data)
			fileContents.emplace_back(key, value);
	}

	//Add new values to the file
	for (const std::pair<std::string, std::string>& pair : m_data)
	{
		//Check if the key is found in the vector
		const auto it = std::ranges::find_if(fileContents,
			[&pair](const std::pair<std::string, std::string>& element)
			{
				return Utils::String::CompareAnyCase(element.first, pair.first);
			});

		if (it == fileContents.end())
		{
			//If not found add it to the vector
			fileContents.emplace_back(pair.first, pair.second);
		}
	}

	std::string outputContent{};
	for (const auto& [key, value] : fileContents)
		outputContent += fmt::format("{}{}\n", key, (!value.empty() ? fmt::format(" = {}", value) : ""));

	return FileSystem::WriteTextFile(file, outputContent);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Config::Print() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	for (const auto& [key, value] : m_data)
		TP_TRACE(Log::ConfigPrefix, key, " = ", value);

	TP_TRACE(Log::ConfigPrefix, "Size: ", m_data.size());
}

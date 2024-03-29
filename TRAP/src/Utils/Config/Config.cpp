#include "TRAPPCH.h"
#include "Config.h"

#include "Utils/String/String.h"
#include "FileSystem/FileSystem.h"

bool TRAP::Utils::Config::LoadFromFile(const std::filesystem::path& file)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

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
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

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

	std::stringstream ss;
	for (const auto& [key, value] : fileContents)
	{
		ss << key; //Write the key

		if (!value.empty())
			//If this line is not empty or a comment also write the assignment and the value
			ss << " = " << value;

		ss << '\n';
	}

	return FileSystem::WriteTextFile(file, ss.str());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Config::Print() const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	for (const auto& [key, value] : m_data)
		TP_TRACE(Log::ConfigPrefix, key, " = ", value);

	TP_TRACE(Log::ConfigPrefix, "Size: ", m_data.size());
}

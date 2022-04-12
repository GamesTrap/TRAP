#include "TRAPPCH.h"
#include "Config.h"

#include "Utils/String/String.h"
#include "FS/FS.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Config::Config()
	: m_hasChanged(false)
{
	TP_PROFILE_FUNCTION();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::LoadFromFile(const std::filesystem::path& file)
{
	TP_PROFILE_FUNCTION();

	m_data.clear();

	//Load
	const std::string input = FS::ReadTextFile(file);
	if (input.empty())
		return false;

	TP_INFO(TRAP::Log::ConfigPrefix, "Loading file: \"", file.generic_u8string(), "\"");
	std::vector<std::string_view> lines = String::SplitStringView(input, '\n');

	for (const auto& line : lines)
	{
		if (!line.empty())
		{
			//Parse line
			auto [key, value] = ParseLine(line);

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
	TP_PROFILE_FUNCTION();

	if(!m_hasChanged)
		return true;

	m_hasChanged = false;

	//Write
	std::vector<std::pair<std::string, std::string>> fileContents;

	TP_INFO(TRAP::Log::ConfigPrefix, "Saving file: \"", file.generic_u8string(), "\"");

	//Read the file into a vector and replace the values of the keys that match with our map
	const std::string input = FS::ReadTextFile(file);
	if (!input.empty())
	{
		const std::vector<std::string> lines = String::SplitString(input, '\n');

		for (const auto& line : lines)
		{
			//Parse line
			auto pLine = ParseLine(line);
			auto& [key, value] = pLine;

			if (!key.empty())
			{
				//Check if the key is found in the vector
				const auto it = std::find_if(m_data.begin(), m_data.end(),
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

	std::stringstream ss;
	for (const auto& [key, value] : fileContents)
	{
		ss << key; //Write the key

		if (!value.empty())
			//If this line is not empty or a comment also write the assignment and the value
			ss << " = " << value;

		ss << '\n';
	}

	return FS::WriteTextFile(file, ss.str());
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::HasChanged() const
{
	TP_PROFILE_FUNCTION();

	return m_hasChanged;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Config::Print() const
{
	TP_PROFILE_FUNCTION();

	for (const auto& [key, value] : m_data)
		TP_TRACE(Log::ConfigPrefix, key, " = ", value);

	TP_TRACE(Log::ConfigPrefix, "Size: ", m_data.size());
}

//-------------------------------------------------------------------------------------------------------------------//

//This method parses a line from out format("key = value") into a std::pair<std::string, std::string>
//containing the key and the value.
//If the line is empty or a comment(starts with a '#') an empty pair is returned.
std::pair<std::string, std::string> TRAP::Utils::Config::ParseLine(const std::string_view line) const
{
	if (!line.empty() && line[0] != '#')
	{
		std::size_t index = 0;
		//Trim leading whitespace
		while (std::isspace(line[index], m_locale))
			index++;
		//Get the key string
		const std::size_t beginKeyString = index;
		while (!std::isspace(line[index], m_locale) && line[index] != '=')
			index++;
		const std::string key(line.data() + beginKeyString, index - beginKeyString);

		//Skip the assignment
		while (std::isspace(line[index], m_locale) || line[index] == '=')
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

	//If this line is empty or a comment, return an empty pair
	return { "", "" };
}
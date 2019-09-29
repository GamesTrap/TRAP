#include "TRAPPCH.h"
#include "Config.h"

#include "Utils/String.h"
#include "VFS/VFS.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Utils::Config::Config()
	: m_isChanged(false)
{
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::LoadFromFile(const std::string& filename)
{
	m_data.clear();
	m_filename = filename;

	return Read();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::SaveToFile()
{
	if (m_isChanged)
	{
		TRAP_INFO("[Config] Saving: ", m_filename);
		m_isChanged = false;

		return Write();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::Read()
{
	const std::string input = VFS::Get()->ReadTextFile("Engine.cfg");
	if (input.empty())
		return false;

	TRAP_INFO("[Config] Loading settings file \"", m_filename, "\"");
	std::vector<std::string> lines = String::SplitString(input, '\n');

	for (const auto& line : lines)
	{
		//Parse line
		std::pair<std::string, std::string> keyValuePair = ParseLine(line);

		if (!keyValuePair.first.empty())
		{
			//If the line is not empty or a comment save it to the vector
			m_data.push_back(keyValuePair);
		}
	}

	m_isChanged = false;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::Write() const
{
	std::vector<std::pair<std::string, std::string>> fileContents;

	//Read the file into a vector and replace the values of the keys that match with our map
	const std::string input = VFS::Get()->ReadTextFile("Engine.cfg");
	if (!input.empty())
	{
		std::vector<std::string> lines = String::SplitString(input, '\n');

		for (const auto& line : lines)
		{
			//Parse line
			std::pair<std::string, std::string> keyValuePair = ParseLine(line);

			if (!keyValuePair.first.empty())
			{
				//Check if the key is found in the vector
				const auto it = std::find_if(m_data.begin(), m_data.end(), [&keyValuePair](const std::pair<std::string, std::string>& element) {return element.first == keyValuePair.first; });
				if (it != m_data.end())
					//If so take it's value, otherwise the value from the file is kept
					keyValuePair.second = it->second;
			}
			else
			{
				//If the line is empty or a comment simply take the whole line as the key
				keyValuePair.first = line;
			}
			fileContents.push_back(keyValuePair);
		}
	}
	else
	{
		//Can't open file for reading. Use only the data from the map
		for (const auto& it : m_data)
			fileContents.emplace_back(it.first, it.second);
	}

	std::stringstream ss;
	for (const auto& fileContent : fileContents)
	{
		ss << fileContent.first; //Write the key

		if (!fileContent.second.empty())
			//If this line is not empty or a comment also write the assignment and the value
			ss << " = " << fileContent.second;

		ss << '\n';
	}

	return VFS::Get()->WriteTextFile("Engine.cfg", ss.str());
}

//-------------------------------------------------------------------------------------------------------------------//

//This method parses a line from out format("key = value") into a std::pair<std::string, std::string> containing the key and the value.
//If the line is empty or a comment(starts with a '#') an empty pair is returned.
std::pair<std::string, std::string> TRAP::Utils::Config::ParseLine(const std::string& line) const
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
		const std::string key = line.substr(beginKeyString, index - beginKeyString);

		//Skip the assignment
		while (std::isspace(line[index], m_locale) || line[index] == '=')
			index++;

		//Get the value string
		const std::string value = line.substr(index, line.size() - index);

		//Return the key value pair
		return std::make_pair(key, value);
	}

	//If this line is empty or a comment, return an empty pair
	return std::make_pair("", "");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Utils::Config::Print() const
{
	for (auto& element : m_data)
		TRAP_TRACE("[Config] ", element.first, " = ", element.second);

	TRAP_TRACE("[Config] Size: ", m_data.size());
}
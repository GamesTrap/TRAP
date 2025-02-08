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
	[[nodiscard]] constexpr TRAP::Optional<std::pair<std::string_view, std::string_view>> ParseLine(const std::string_view line, const bool filterSpaceAndComments)
	{
		//If this line is empty or a comment, return an empty pair
		if(line.empty() || line[0] == '#')
		{
			if(filterSpaceAndComments)
				return TRAP::NullOpt;

			return std::pair{line, ""};
		}

		usize index = 0u;
		//Trim leading whitespace
		while (TRAP::Utils::String::IsSpace(line[index]))
			++index;
		//Get the key string
		const usize beginKeyString = index;
		while (!TRAP::Utils::String::IsSpace(line[index]) && line[index] != '=')
			++index;
		const std::string_view key(line.data() + beginKeyString, index - beginKeyString);

		//Skip the assignment
		while (TRAP::Utils::String::IsSpace(line[index]) || line[index] == '=')
		{
			++index;
			if(index >= line.size())
			{
				//Out of range so line only contains key which is invalid
				return TRAP::NullOpt;
			}
		}

		//Get the value string
		const std::string_view value(line.data() + index, line.size() - index);

		//Return the key value pair
		return std::pair{ key, value };
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

	for (const auto line : lines)
	{
		//Parse line
		const auto parsedLine = ParseLine(line, true);
		if(!parsedLine)
			continue;

		const auto& [key, value] = *parsedLine;

		if (key.empty())
			continue;

		//If the line is not empty or a comment save it to the vector
		m_data.emplace_back(key, value);
	}

	m_hasChanged = false;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Utils::Config::SaveToFile(const std::filesystem::path& file)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, (GetTRAPProfileSystems() & ProfileSystems::Utils) != ProfileSystems::None);

	m_hasChanged = false;

	//Write
	std::vector<std::pair<std::string, std::string>> fileContents{};

	TP_INFO(TRAP::Log::ConfigPrefix, "Saving file: ", file);

	//1. Read contents of current cfg file
	if(const auto input = FileSystem::ReadTextFile(file))
	{
		const auto lines = String::SplitStringView(*input, '\n');
		fileContents.reserve(lines.size());
		for(const auto line : lines)
		{
			if(auto parsedLine = ParseLine(line, false))
				fileContents.emplace_back(parsedLine->first, parsedLine->second);
		}
	}

	//2. Overwrite fileContents with values from m_data
	if(!fileContents.empty())
	{
		for(const auto& [key, value] : m_data)
		{
			const auto it = std::ranges::find_if(fileContents, [&key](const auto& e){return String::CompareAnyCase(e.first, key);});
			if(it != fileContents.end())
				it->second = value; //Overwrite with new value
			else
				fileContents.emplace_back(key, value); //Add new value
		}
	}
	else //Empty cfg or was unable to read, just use data of m_data directly
	{
		fileContents = m_data;
	}

	//3. Save to disk
	std::string outputContent{};
	for (const auto& [key, value] : fileContents)
		outputContent += fmt::format("{}{}\n", key, (!value.empty() ? fmt::format(" = {}", value) : ""));

	return FileSystem::WriteTextFile(file, outputContent);
}

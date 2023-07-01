#ifndef TRAP_CONFIG_H
#define TRAP_CONFIG_H

#include <optional>

#include "Utils/String/String.h"

namespace TRAP::Utils
{
	class Config
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		Config() noexcept;
		/// <summary>
		/// Destructor.
		/// </summary>
		~Config() = default;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Config(const Config&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Config(Config&&) = default;

		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Config& operator=(const Config&) = delete;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Config& operator=(Config&&) = default;

		/// <summary>
		/// Load a config file from disk.
		/// </summary>
		/// <param name="file">File path to load.</param>
		/// <returns>True if loading was successful, false otherwise.</returns>
		bool LoadFromFile(const std::filesystem::path& file);
		/// <summary>
		/// Save a config file to disk.
		/// </summary>
		/// <param name="file">File path to save to.</param>
		/// <returns>True if saving was successful, false otherwise.</returns>
		bool SaveToFile(const std::filesystem::path& file);

		/// <summary>
		/// Check if config has changed after last load/save.
		/// </summary>
		/// <returns>True if config has changed, false otherwise.</returns>
		[[nodiscard]] bool HasChanged() const noexcept;

		/// <summary>
		/// Retrieve the value of a specific key in the config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get value from.</param>
		/// <returns>Found value or std::nullopt.</returns>
		template<typename T>
		[[nodiscard]] std::optional<T> Get(std::string_view key) const;
		/// <summary>
		/// Retrieve the values of a specific key in the config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get values from.</param>
		/// <returns>Found values or std::nullopt.</returns>
		template<typename T>
		[[nodiscard]] std::optional<std::vector<T>> GetVector(std::string_view key) const;

		/// <summary>
		/// Set a value in the config.
		/// </summary>
		/// <typeparam name="T">Value type.</typeparam>
		/// <param name="key">Key for the new or updated value.</param>
		/// <param name="value">Value.</param>
		template<typename T>
		void Set(const std::string& key, T value);
		/// <summary>
		/// Set values in the config.
		/// </summary>
		/// <typeparam name="T">Value type.</typeparam>
		/// <param name="key">Key for the new or updated values.</param>
		/// <param name="value">Values.</param>
		template<typename T>
		void Set(const std::string& key, const std::vector<T>& value);

		/// <summary>
		/// Print all stored values to the console.
		/// </summary>
		void Print() const;

	private:
		/// <summary>
		/// Parse a line from the Config files.
		/// Splits keys from value(s).
		/// Ignores lines starting with a '#'.
		/// </summary>
		/// <param name="line">Line to parse.</param>
		/// <returns>Pair of key and value.</returns>
		[[nodiscard]] static constexpr std::pair<std::string, std::string> ParseLine(std::string_view line);

		bool m_hasChanged;
		std::vector<std::pair<std::string, std::string>> m_data;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::optional<T> TRAP::Utils::Config::Get(const std::string_view key) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::find_if(m_data.begin(), m_data.end(),
		[key](const std::pair<std::string, std::string>& element)
		{
			return Utils::String::CompareAnyCase(element.first, key);
		});
	if (it != m_data.end())
		return String::ConvertToType<T>(it->second);

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

//This method tries to read the value of a key into a vector.
//The values have to be separated by comma.
//The vector is cleared before it it filled.
template<typename T>
[[nodiscard]] std::optional<std::vector<T>> TRAP::Utils::Config::GetVector(const std::string_view key) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::find_if(m_data.begin(), m_data.end(),
		[key](const std::pair<std::string, std::string>& element)
		{
			return element.first == key;
		});
	if (it != m_data.end())
	{
		const std::vector<std::string> splitted = Utils::String::SplitString(it->second, ',');
		if(splitted.empty())
			return std::nullopt;

		std::vector<T> data(splitted.size());

		for (const std::string& str : splitted)
			data.push_back(String::ConvertToType<T>(str));

		return data;
	}

	return std::nullopt;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const T value)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//Replaces the value if the key is found
	m_hasChanged = true;
	auto elementIterator = std::find_if(m_data.begin(), m_data.end(),
		[&key](const std::pair<std::string, std::string>& test)
		{
			return Utils::String::CompareAnyCase(test.first, key);
		});
	if(elementIterator != m_data.end())
		elementIterator->second = String::ConvertToString<T>(value);
	else
	{
		//If not it creates a new element
		m_data.emplace_back(key, String::ConvertToString<T>(value));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const std::vector<T>& value)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//Transform the vector into a string that separates the elements with a comma
	std::string valueAsString;
	for (std::size_t i = 0; i < value.size() - 1; ++i)
		valueAsString += String::ConvertToString<T>(value[i]) + ',';
	valueAsString += String::ConvertToString<T>(value.back());

	//Replace the value if the key is found
	m_hasChanged = true;
	auto elementIterator = std::find_if(m_data.begin(), m_data.end(),
		[&key](const std::pair<std::string, std::string>& test)
		{
			return Utils::String::CompareAnyCase(test.first, key);
		});
	if (elementIterator != m_data.end())
		elementIterator->second = valueAsString;
	else
	{
		//If not it creates a new element
		m_data.emplace_back(key, valueAsString);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

//This method parses a line from out format("key = value") into a std::pair<std::string, std::string>
//containing the key and the value.
//If the line is empty or a comment(starts with a '#') an empty pair is returned.
[[nodiscard]] inline constexpr std::pair<std::string, std::string> TRAP::Utils::Config::ParseLine(const std::string_view line)
{
	//If this line is empty or a comment, return an empty pair
	if(line.empty() || line[0] == '#')
		return { "", "" };

	std::size_t index = 0;
	//Trim leading whitespace
	while (Utils::String::IsSpace(line[index]))
		index++;
	//Get the key string
	const std::size_t beginKeyString = index;
	while (!Utils::String::IsSpace(line[index]) && line[index] != '=')
		index++;
	const std::string key(line.data() + beginKeyString, index - beginKeyString);

	//Skip the assignment
	while (Utils::String::IsSpace(line[index]) || line[index] == '=')
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

#endif /*TRAP_CONFIG_H*/
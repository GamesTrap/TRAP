#ifndef TRAP_CONFIG_H
#define TRAP_CONFIG_H

#include <optional>

#include "Utils/String/String.h"

namespace TRAP::Utils
{
	class Config
	{
	public:
		/// @brief Constructor.
		constexpr Config() noexcept = default;
		/// @brief Destructor.
		constexpr ~Config() = default;

		/// @brief Copy constructor.
		consteval Config(const Config&) = delete;
		/// @brief Move constructor.
		constexpr Config(Config&&) noexcept = default;

		/// @brief Copy assignment operator.
		consteval Config& operator=(const Config&) = delete;
		/// @brief Move assignment operator.
		constexpr Config& operator=(Config&&) noexcept = default;

		/// @brief Load a config file from disk.
		/// @param file File path to load.
		/// @return True if loading was successful, false otherwise.
		bool LoadFromFile(const std::filesystem::path& file);
		/// @brief Save a config file to disk.
		/// @param file File path to save to.
		/// @return True if saving was successful, false otherwise.
		bool SaveToFile(const std::filesystem::path& file);

		/// @brief Check if config has changed after last load/save.
		/// @return True if config has changed, false otherwise.
		[[nodiscard]] constexpr bool HasChanged() const noexcept;

		/// @brief Retrieve the value of a specific key in the config.
		/// @tparam T Output variable.
		/// @param key Key to get value from.
		/// @return Found value or std::nullopt.
		template<typename T>
		[[nodiscard]] std::optional<T> Get(std::string_view key) const;
		/// @brief Retrieve the values of a specific key in the config.
		/// @tparam T Output variable.
		/// @param key Key to get values from.
		/// @return Found values or std::nullopt.
		template<typename T>
		[[nodiscard]] std::optional<std::vector<T>> GetVector(std::string_view key) const;

		/// @brief Set a value in the config.
		/// @tparam T Value type.
		/// @param key Key for the new or updated value.
		/// @param value Value.
		template<typename T>
		void Set(const std::string& key, T value);
		/// @brief Set values in the config.
		/// @tparam T Value type.
		/// @param key Key for the new or updated values.
		/// @param value Values.
		template<typename T>
		void Set(const std::string& key, const std::vector<T>& value);

		/// @brief Print all stored values to the console.
		void Print() const;

	private:
		/// @brief Parse a line from the Config files.
		/// Splits keys from value(s).
		/// Ignores lines starting with a '#'.
		/// @param line Line to parse.
		/// @return Pair of key and value.
		[[nodiscard]] static constexpr std::pair<std::string, std::string> ParseLine(std::string_view line);

		bool m_hasChanged = false;
		std::vector<std::pair<std::string, std::string>> m_data{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::optional<T> TRAP::Utils::Config::Get(const std::string_view key) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::ranges::find_if(m_data,
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

	const auto it = std::ranges::find_if(m_data,
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
	auto elementIterator = std::ranges::find_if(m_data,
		[&key](const std::pair<std::string, std::string>& test)
		{
			return Utils::String::CompareAnyCase(test.first, key);
		});
	if(elementIterator != m_data.end())
		elementIterator->second = fmt::format("{}", value);
	else
	{
		//If not it creates a new element
		m_data.emplace_back(key, fmt::format("{}", value));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const std::vector<T>& value)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//Transform the vector into a string that separates the elements with a comma
	std::string valueAsString;
	for (usize i = 0; i < value.size() - 1; ++i)
		valueAsString += fmt::format("{},", value[i]);
	valueAsString += fmt::format("{}", value.back());

	//Replace the value if the key is found
	m_hasChanged = true;
	auto elementIterator = std::ranges::find_if(m_data,
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

[[nodiscard]] constexpr bool TRAP::Utils::Config::HasChanged() const noexcept
{
	return m_hasChanged;
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

	usize index = 0;
	//Trim leading whitespace
	while (Utils::String::IsSpace(line[index]))
		index++;
	//Get the key string
	const usize beginKeyString = index;
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
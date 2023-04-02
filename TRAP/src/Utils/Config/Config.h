#ifndef TRAP_CONFIG_H
#define TRAP_CONFIG_H

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
		/// <param name="value">
		/// Output variable for the value.
		/// Note: Unchanged when key was not found!
		/// </param>
		template<typename T>
		void Get(std::string_view key, T& value) const;
		/// <summary>
		/// Retrieve the values of a specific key in the config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get values from.</param>
		/// <param name="value">
		/// Output variable for the values.
		/// Note: Unchanged when key was not found!
		/// </param>
		template<typename T>
		void Get(std::string_view key, std::vector<T>& value) const;

		/// <summary>
		/// Retrieve the value of a specific key in the config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get value from.</param>
		/// <returns>Found value or default constructor for the given type.</returns>
		template<typename T>
		[[nodiscard]] T Get(std::string_view key) const;
		/// <summary>
		/// Retrieve the values of a specific key in the config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get values from.</param>
		/// <returns>Found values or default constructor for the given type.</returns>
		template<typename T>
		[[nodiscard]] std::vector<T> GetVector(std::string_view key) const;

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
		[[nodiscard]] std::pair<std::string, std::string> ParseLine(std::string_view line) const;

		bool m_hasChanged;
		std::vector<std::pair<std::string, std::string>> m_data;
		const std::locale m_locale;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Get(const std::string_view key, T& value) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::find_if(m_data.begin(), m_data.end(),
		[&key](const std::pair<std::string, std::string>& element)
		{
			return Utils::String::CompareAnyCase(element.first, key);
		});
	if (it != m_data.end())
		value = String::ConvertToType<T>(it->second);
}

//-------------------------------------------------------------------------------------------------------------------//

//This method tries to read the value of a key into a vector.
//The values have to be separated by comma.
//The vector is cleared before it it filled.
template<typename T>
void TRAP::Utils::Config::Get(const std::string_view key, std::vector<T>& value) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::find_if(m_data.begin(), m_data.end(),
		[&key](const std::pair<std::string, std::string>& element)
		{
			return Utils::String::CompareAnyCase(element.first, key);
		});
	if (it != m_data.end())
	{
		value.clear();

		//Split by comma
		for (const std::string& str : Utils::String::SplitString(it->second, ','))
			value.push_back(String::ConvertToType<T>(str));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] T TRAP::Utils::Config::Get(const std::string_view key) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::find_if(m_data.begin(), m_data.end(),
		[&key](const std::pair<std::string, std::string>& element)
		{
			return Utils::String::CompareAnyCase(element.first, key);
		});
	if (it != m_data.end())
		return String::ConvertToType<T>(it->second);

	return T();
}

//-------------------------------------------------------------------------------------------------------------------//

//This method tries to read the value of a key into a vector.
//The values have to be separated by comma.
//The vector is cleared before it it filled.
template<typename T>
[[nodiscard]] std::vector<T> TRAP::Utils::Config::GetVector(const std::string_view key) const
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	const auto it = std::find_if(m_data.begin(), m_data.end(),
		[&key](const std::pair<std::string, std::string>& element)
		{
			return element.first == key;
		});
	if (it != m_data.end())
	{
		std::vector<T> data{};

		//Split by comma
		for (const std::string& str : Utils::String::SplitString(it->second, ','))
			data.push_back(String::ConvertToType<T>(str));
	}

	return std::vector<T>();
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const T value)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

	//Replaces the value if the key is found
	m_hasChanged = true;
	auto elementIterator = std::find_if(m_data.begin(), m_data.end(),
		[key](const std::pair<std::string, std::string>& test)
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
		[key](const std::pair<std::string, std::string>& test)
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

#endif /*TRAP_CONFIG_H*/
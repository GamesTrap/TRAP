#ifndef _TRAP_CONFIG_H_
#define _TRAP_CONFIG_H_

#include "Window/Window.h"
#include "Utils/String/String.h"

namespace TRAP::Utils
{
	class Config
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		Config();
		/// <summary>
		/// Default Destructor.
		/// </summary>
		~Config() = default;
		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Config(const Config&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Config(Config&&) = default;
		/// <summary>
		/// Deleted Copy Assignment Operator.
		/// </summary>
		Config& operator=(const Config&) = delete;
		/// <summary>
		/// Deleted Move Assignment Operator.
		/// </summary>
		Config& operator=(Config&&) = delete;

		/// <summary>
		/// Load a Config file from disk.
		/// </summary>
		/// <param name="file">Virtual or physical file path to load.</param>
		/// <returns>True if loading was successful, false otherwise.</returns>
		bool LoadFromFile(std::string_view file);
		/// <summary>
		/// Save a Config file to disk.
		/// </summary>
		/// <param name="file">Virtual or physical file path to save to.</param>
		/// <returns>True if saving was successful, false otherwise.</returns>
		bool SaveToFile(std::string_view file);

		/// <summary>
		/// Check if Config got changed after last Loading/Saving.
		/// </summary>
		/// <returns>True if Config got changed, false otherwise.</returns>
		bool IsChanged() const;

		/// <summary>
		/// Retrieve the value of a specific key in the Config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get value from.</param>
		/// <param name="value">Output variable for the value.<br>Note: Unchanged when key was not found!</param>
		template<typename T>
		void Get(std::string_view key, T& value) const;
		/// <summary>
		/// Retrieve the values of a specific key in the Config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get values from.</param>
		/// <param name="value">Output variable for the values.<br>Note: Unchanged when key was not found!</param>
		template<typename T>
		void Get(std::string_view key, std::vector<T>& value) const;

		/// <summary>
		/// Retrieve the value of a specific key in the Config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get value from.</param>
		/// <returns>Found value or default constructor for the given type.</returns>
		template<typename T>
		T Get(std::string_view key) const;
		/// <summary>
		/// Retrieve the values of a specific key in the Config.
		/// </summary>
		/// <typeparam name="T">Output variable.</typeparam>
		/// <param name="key">Key to get values from.</param>
		/// <returns>Found values or default constructor for the given type.</returns>
		template<typename T>
		std::vector<T> GetVector(std::string_view key) const;

		/// <summary>
		/// Set a value in the Config.
		/// </summary>
		/// <typeparam name="T">Value type.</typeparam>
		/// <param name="key">Key for the new or updated value.</param>
		/// <param name="value">Value.</param>
		template<typename T>
		void Set(const std::string& key, T value);
		/// <summary>
		/// Set values in the Config.
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
		//Return the string in the type of T

		/// <summary>
		/// Convert a string to the given type.
		/// </summary>
		/// <typeparam name="T">Type to get.</typeparam>
		/// <param name="input">String to convert.</param>
		/// <returns>String converted to type T.</returns>
		template<typename T>
		T ConvertToType(const std::string& input) const;
		/// <summary>
		/// Convert a value of type T to a string.
		/// </summary>
		/// <typeparam name="T">Type to convert.</typeparam>
		/// <param name="value">Value to convert.</param>
		/// <returns>Type T value converted to string.</returns>
		template<typename T>
		std::string ConvertToString(T value) const;

		/// <summary>
		/// Parse a line from the Config files.<br>
		/// Splits keys from value(s).<br>
		/// Ignores lines starting with a '#'.
		/// </summary>
		/// <param name="line">Line to parse.</param>
		/// <returns>Pair of key and value.</returns>
		std::pair<std::string, std::string> ParseLine(const std::string_view& line) const;

		bool m_isChanged;
		std::vector<std::pair<std::string, std::string>> m_data;
		const std::locale m_locale;
	};	
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Get(const std::string_view key, T& value) const
{
	TP_PROFILE_FUNCTION();
	
	const auto it = std::find_if(m_data.begin(), m_data.end(), 
		[&key](const std::pair<std::string, std::string>& element)
		{
			return Utils::String::CompareAnyCase(element.first, key);
		});
	if (it != m_data.end())
		value = ConvertToType<T>(it->second);
}

//-------------------------------------------------------------------------------------------------------------------//

//This method tries to read the value of a key into a vector.
//The values have to be separated by comma.
//The vector is cleared before it it filled.
template<typename T>
void TRAP::Utils::Config::Get(const std::string_view key, std::vector<T>& value) const
{
	TP_PROFILE_FUNCTION();
	
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
			value.push_back(ConvertToType<T>(str));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Utils::Config::Get(const std::string_view key) const
{
	TP_PROFILE_FUNCTION();

	const auto it = std::find_if(m_data.begin(), m_data.end(), 
		[&key](const std::pair<std::string, std::string>& element)
		{
			return Utils::String::CompareAnyCase(element.first, key);
		});
	if (it != m_data.end())
		return ConvertToType<T>(it->second);

	return T();
}

//-------------------------------------------------------------------------------------------------------------------//

//This method tries to read the value of a key into a vector.
//The values have to be separated by comma.
//The vector is cleared before it it filled.
template<typename T>
std::vector<T> TRAP::Utils::Config::GetVector(const std::string_view key) const
{
	TP_PROFILE_FUNCTION();

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
			data.push_back(ConvertToType<T>(str));
	}

	return std::vector<T>();
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const T value)
{
	TP_PROFILE_FUNCTION();
	
	//Replaces the value if the key is found
	m_isChanged = true;
	auto elementIterator = std::find_if(m_data.begin(), m_data.end(),
		[key](const std::pair<std::string, std::string>& test)
		{
			return Utils::String::CompareAnyCase(test.first, key);
		});
	if(elementIterator != m_data.end())
		elementIterator->second = ConvertToString<T>(value);
	else
	{
		//If not it creates a new element
		m_data.emplace_back(key, ConvertToString<T>(value));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const std::vector<T>& value)
{
	TP_PROFILE_FUNCTION();
	
	//Transform the vector into a string that separates the elements with a comma
	std::string valueAsString;
	for (std::size_t i = 0; i < value.size() - 1; ++i)
		valueAsString += ConvertToString<T>(value[i]) + ',';
	valueAsString += ConvertToString<T>(value.back());

	//Replace the value if the key is found
	m_isChanged = true;
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

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Utils::Config::ConvertToType(const std::string&) const
{
	TRAP_ASSERT(false, "Unconvertable type encountered, please use a different type, or define the handle case in Config.h");
	TP_ERROR(TRAP::Log::ConfigPrefix, "Unconvertable type encountered, please use a different type, or define the handle case in Config.h");
	return T();
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline int8_t TRAP::Utils::Config::ConvertToType<int8_t>(const std::string& input) const
{
	try
	{
		const int32_t value = std::stoi(input);
		if (value > std::numeric_limits<int8_t>::max() || value < std::numeric_limits<int8_t>::min())
			return 0;

		return static_cast<int8_t>(value);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to int8_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline uint8_t TRAP::Utils::Config::ConvertToType<uint8_t>(const std::string& input) const
{
	try
	{
		const uint32_t value = std::stoul(input);
		if (value > std::numeric_limits<uint8_t>::max() || value < std::numeric_limits<uint8_t>::min())
			return 0;

		return static_cast<uint8_t>(value);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to uint8_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline int16_t TRAP::Utils::Config::ConvertToType<int16_t>(const std::string& input) const
{
	try
	{
		const int32_t value = std::stoi(input);
		if (value > std::numeric_limits<int16_t>::max() || value < std::numeric_limits<int16_t>::min())
			return 0;

		return static_cast<int16_t>(value);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to int16_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline uint16_t TRAP::Utils::Config::ConvertToType<uint16_t>(const std::string& input) const
{
	try
	{
		const uint32_t value = std::stoul(input);
		if (value > std::numeric_limits<uint16_t>::max() || value < std::numeric_limits<uint16_t>::min())
			return 0;

		return static_cast<uint16_t>(value);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to int16_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline int32_t TRAP::Utils::Config::ConvertToType<int32_t>(const std::string& input) const
{
	try
	{
		return std::stoi(input);
	}
	catch(std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to int32_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline uint32_t TRAP::Utils::Config::ConvertToType<uint32_t>(const std::string& input) const
{
	try
	{
		return std::stoul(input);
	}
	catch(std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to uint32_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline int64_t TRAP::Utils::Config::ConvertToType<int64_t>(const std::string& input) const
{
	try
	{
		return std::stoll(input);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to int64_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline uint64_t TRAP::Utils::Config::ConvertToType<uint64_t>(const std::string& input) const
{
	try
	{
		return std::stoull(input);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to uint64_t!");
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline float TRAP::Utils::Config::ConvertToType<float>(const std::string& input) const
{
	try
	{
		return std::stof(input);
	}
	catch (std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to float!");
		return 0.0f;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline double TRAP::Utils::Config::ConvertToType<double>(const std::string& input) const
{
	try
	{
		return std::stod(input);
	}
	catch(std::exception&)
	{
		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to double!");
		return 0.0;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline bool TRAP::Utils::Config::ConvertToType<bool>(const std::string& input) const
{
	if (String::CompareAnyCase("TRUE", input))
		return true;
	
	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline char TRAP::Utils::Config::ConvertToType<char>(const std::string& input) const
{
	return input[0];
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToType<std::string>(const std::string& input) const
{
	return input;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string_view TRAP::Utils::Config::ConvertToType<std::string_view>(const std::string& input) const
{
	return input;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline TRAP::Window::DisplayMode TRAP::Utils::Config::ConvertToType<TRAP::Window::DisplayMode>(const std::string& input) const
{
	if (Utils::String::CompareAnyCase("Windowed", input))
		return Window::DisplayMode::Windowed;
	if (Utils::String::CompareAnyCase("Borderless", input))
		return Window::DisplayMode::Borderless;
	if (Utils::String::CompareAnyCase("Fullscreen", input))
		return Window::DisplayMode::Fullscreen;

	TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Window::DisplayMode!");
	return Window::DisplayMode::Windowed;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline TRAP::Graphics::RenderAPI TRAP::Utils::Config::ConvertToType<TRAP::Graphics::RenderAPI>(const std::string& input) const
{
	if (Utils::String::CompareAnyCase("Vulkan", input) || Utils::String::CompareAnyCase("VulkanAPI", input))
		return Graphics::RenderAPI::Vulkan;

	TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::API::RenderAPI!");
	return Graphics::RenderAPI::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Utils::Config::ConvertToString(T) const
{
	TP_ERROR(TRAP::Log::ConfigPrefix, "Unsupported type supplied, either change types, or write a correct conversion function for the template type.");
	throw "[Config] Unsupported type supplied, either change types, or write a correct conversion function for the template type.";
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<std::string>(std::string value) const
{
	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<const char*>(const char* value) const
{
	return std::string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<bool>(const bool value) const
{
	return (value) ? "True" : "False";
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<char>(const char value) const
{
	return std::string(1, value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<uint8_t>(const uint8_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<int16_t>(const int16_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<uint16_t>(const uint16_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<int32_t>(const int32_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<uint32_t>(const uint32_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<int64_t>(const int64_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<uint64_t>(const uint64_t value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<float>(const float value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<double>(const double value) const
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<TRAP::Window::DisplayMode>(const Window::DisplayMode value) const
{
	switch (value)
	{
	case Window::DisplayMode::Windowed:
		return "Windowed";

	case Window::DisplayMode::Borderless:
		return "Borderless";

	case Window::DisplayMode::Fullscreen:
		return "Fullscreen";

	default:
		return "Windowed";
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<TRAP::Graphics::RenderAPI>(const Graphics::RenderAPI value) const
{
	switch (value)
	{
	case Graphics::RenderAPI::Vulkan:
		return "Vulkan";

	default:
		return "";
	}
}

#endif /*_TRAP_CONFIG_H_*/
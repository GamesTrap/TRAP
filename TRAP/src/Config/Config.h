#ifndef _TRAP_CONFIG_H_
#define _TRAP_CONFIG_H_

#include "Window/Window.h"

namespace TRAP::Utils
{
	class Config
	{
	public:
		Config();
		~Config() = default;
		Config(const Config&) = default;
		Config(Config&&) = default;
		Config& operator=(const Config&) = delete;
		Config& operator=(Config&&) = delete;

		bool LoadFromFile(const std::string& filename);
		bool SaveToFile(const std::string& filename);

		bool IsChanged() const;

		template<typename T>
		void Get(const std::string& key, T& value) const;
		template<typename T>
		void Get(const std::string& key, std::vector<T>& value) const;

		template<typename T>
		void Set(const std::string& key, T value);
		template<typename T>
		void Set(const std::string& key, const std::vector<T>& value);

		void Print() const;

	private:
		//Return the string in the type of T
		template<typename T>
		T ConvertToType(const std::string& input) const;
		//Return string of type T
		template<typename T>
		std::string ConvertToString(T value) const;

		bool Read();
		bool Write() const;
		std::pair<std::string, std::string> ParseLine(const std::string_view& line) const;

		bool m_isChanged;
		std::string m_filename;
		std::vector<std::pair<std::string, std::string>> m_data;
		const std::locale m_locale;
	};	
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Get(const std::string& key, T& value) const
{
	TP_PROFILE_FUNCTION();
	
	const auto it = std::find_if(m_data.begin(), m_data.end(), [&key](const std::pair<std::string, std::string>& element) {return element.first == key; });
	if (it != m_data.end())
		value = ConvertToType<T>(it->second);
}

//-------------------------------------------------------------------------------------------------------------------//

//This method tries to read the value of a key into a vector.
//The values have to be separated by comma.
//The vector is cleared before it it filled.
template<typename T>
void TRAP::Utils::Config::Get(const std::string& key, std::vector<T>& value) const
{
	TP_PROFILE_FUNCTION();
	
	constexpr auto it = std::find_if(m_data.begin(), m_data.end(), [&key](const std::pair<std::string, std::string>& element) {return element.first == key; });
	if (it != m_data.end())
	{
		std::string output;
		std::istringstream parser(it->second);

		value.clear();

		//Split by comma
		while (getline(parser, output, ','))
			value.push_back(ConvertToType<T>(output));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const T value)
{
	TP_PROFILE_FUNCTION();
	
	//Replaces the value if the key is found
	m_isChanged = true;
	auto elementIterator = std::find_if(m_data.begin(), m_data.end(),
		[key](const std::pair<std::string, std::string>& test) {return test.first == key ? true : false; });
	if(elementIterator != m_data.end())
		elementIterator->second = ConvertToString<T>(value);
	else
	{
		//If not it creates a new element
		m_data.push_back({ key, ConvertToString<T>(value) });
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
void TRAP::Utils::Config::Set(const std::string& key, const std::vector<T>& value)
{
	TP_PROFILE_FUNCTION();
	
	//Transform the vector into a string that separates the elements with a comma
	const std::string valueAsString;
	for (std::size_t i = 0; i < value.size() - 1; ++i)
		valueAsString += ConvertToString<T>(value[i]) + ',';
	valueAsString += ConvertToString<T>(value.back());

	//Replace the value if the key is found
	m_isChanged = true;
	auto elementIterator = std::find_if(m_data.begin(), m_data.end(),
		[key](const std::pair<std::string, std::string>& test) {return test.first == key ? true : false; });
	if (elementIterator != m_data.end())
		elementIterator->second = valueAsString;
	else
	{
		//If not it creates a new element
		m_data.push_back({ key, valueAsString });
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
T TRAP::Utils::Config::ConvertToType(const std::string& input) const
{
	TP_ERROR("[Config] Unconvertable type encountered, please use a different type, or define the handle case in Config.h");
	throw "[Config] Unconvertable type encountered, please use a different type, or define the handle case in Config.h";
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline int32_t TRAP::Utils::Config::ConvertToType<int32_t>(const std::string& input) const
{
	int32_t value;
	std::stringstream ss(input);
	ss >> value;

	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline uint32_t TRAP::Utils::Config::ConvertToType<uint32_t>(const std::string& input) const
{
	uint32_t value;
	std::stringstream ss(input);
	ss >> value;

	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline double TRAP::Utils::Config::ConvertToType<double>(const std::string& input) const
{
	double value;
	std::stringstream ss(input);
	ss >> value;

	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline float TRAP::Utils::Config::ConvertToType<float>(const std::string& input) const
{
	float value;
	std::stringstream ss(input);
	ss >> value;

	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline short TRAP::Utils::Config::ConvertToType<short>(const std::string& input) const
{
	short value;
	std::stringstream ss(input);
	ss >> value;

	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline bool TRAP::Utils::Config::ConvertToType<bool>(const std::string& input) const
{
	return input == "TRUE" ? true : false;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline char TRAP::Utils::Config::ConvertToType<char>(const std::string& input) const
{
	return input[0];
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline unsigned char TRAP::Utils::Config::ConvertToType<unsigned char>(const std::string& input) const
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
inline TRAP::Window::DisplayMode TRAP::Utils::Config::ConvertToType<TRAP::Window::DisplayMode>(const std::string& input) const
{
	if (input == "Windowed")
		return Window::DisplayMode::Windowed;
	if (input == "Borderless")
		return Window::DisplayMode::Borderless;
	if (input == "Fullscreen")
		return Window::DisplayMode::Fullscreen;

	return Window::DisplayMode::Windowed;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline TRAP::Graphics::API::RenderAPI TRAP::Utils::Config::ConvertToType<TRAP::Graphics::API::RenderAPI>(const std::string& input) const
{
	if (input == "Vulkan")
		return Graphics::API::RenderAPI::Vulkan;
	if (input == "OpenGL")
		return Graphics::API::RenderAPI::OpenGL;

	return Graphics::API::RenderAPI::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Utils::Config::ConvertToString(T value) const
{
	TP_ERROR("[Config] Unsupported type supplied, either change types, or write a correct conversion function for the template type.");
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
	return (value) ? "TRUE" : "FALSE";
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<char>(const char value) const
{
	std::string tmp;
	tmp = value;

	return tmp;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<unsigned char>(const unsigned char value) const
{
	std::string tmp;
	tmp = value;

	return tmp;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<int32_t>(const int32_t value) const
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<uint32_t>(const uint32_t value) const
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<float>(const float value) const
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<short>(const short value) const
{
	std::stringstream ss;
	ss << value;

	return ss.str();
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::Config::ConvertToString<double>(const double value) const
{
	std::stringstream ss;
	ss << value;

	return ss.str();
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
inline std::string TRAP::Utils::Config::ConvertToString<TRAP::Graphics::API::RenderAPI>(const Graphics::API::RenderAPI value) const
{
	switch (value)
	{
	case Graphics::API::RenderAPI::Vulkan:
		return "Vulkan";

	case Graphics::API::RenderAPI::OpenGL:
		return "OpenGL";

	default:
		return "";
	}
}

#endif /*_TRAP_CONFIG_H_*/
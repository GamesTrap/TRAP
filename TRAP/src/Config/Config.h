#pragma once

#include "Graphics/API/Context.h"
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
		bool SaveToFile();

		bool IsChanged() const;

		template<typename T>
		void Get(const std::string& key, T& value) const;
		template<typename T>
		void Get(const std::string& key, std::vector<T>& value) const;

		template<typename T>
		void Set(const std::string& key, T value);
		template<typename T>
		void Set(const std::string& key, std::vector<T> value);

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
		std::pair<std::string, std::string> ParseLine(const std::string& line) const;

		bool m_isChanged;
		std::string m_filename;
		//std::map<std::string, std::string> m_data;
		std::vector<std::pair<std::string, std::string>> m_data;
		const std::locale m_locale;
	};

	template<typename T>
	std::string Config::ConvertToString(T value) const
	{
		TP_ERROR("[Config] Unsupported type supplied, either change types, or write a correct conversion function for the template type.");
		throw "[Config] Unsupported type supplied, either change types, or write a correct conversion function for the template type.";
	}

	template<>
	inline std::string Config::ConvertToString<std::string>(std::string value) const
	{
		return value;
	}

	template<>
	inline std::string Config::ConvertToString<const char*>(const char* value) const
	{
		return std::string(value);
	}

	template<>
	inline std::string Config::ConvertToString<bool>(const bool value) const
	{
		return (value) ? "TRUE" : "FALSE";
	}

	template<>
	inline std::string Config::ConvertToString<char>(const char value) const
	{
		std::string tmp;
		tmp = value;

		return tmp;
	}

	template<>
	inline std::string Config::ConvertToString<unsigned char>(const unsigned char value) const
	{
		std::string tmp;
		tmp = value;

		return tmp;
	}

	template<>
	inline std::string Config::ConvertToString<int>(const int value) const
	{
		std::stringstream ss;
		ss << value;

		return ss.str();
	}

	template<>
	inline std::string Config::ConvertToString<unsigned int>(const unsigned int value) const
	{
		std::stringstream ss;
		ss << value;

		return ss.str();
	}

	template<>
	inline std::string Config::ConvertToString<float>(const float value) const
	{
		std::stringstream ss;
		ss << value;

		return ss.str();
	}

	template<>
	inline std::string Config::ConvertToString<short>(const short value) const
	{
		std::stringstream ss;
		ss << value;

		return ss.str();
	}

	template<>
	inline std::string Config::ConvertToString<double>(const double value) const
	{
		std::stringstream ss;
		ss << value;

		return ss.str();
	}

	template<>
	inline std::string Config::ConvertToString<DisplayMode>(const DisplayMode value) const
	{
		switch (value)
		{
		case DisplayMode::WINDOWED:
			return "Windowed";

		case DisplayMode::BORDERLESS:
			return "Borderless";

		case DisplayMode::FULLSCREEN:
			return "Fullscreen";

		default:
			return "Windowed";
		}
	}

	template<>
	inline std::string Config::ConvertToString<Graphics::API::RenderAPI>(const Graphics::API::RenderAPI value) const
	{
		switch (value)
		{
		case Graphics::API::RenderAPI::VULKAN:
			return "Vulkan";

		case Graphics::API::RenderAPI::D3D12:
			return "D3D12";

		case Graphics::API::RenderAPI::OPENGL:
			return "OpenGL";

		default:
			return "";
		}
	}

	template<typename T>
	T Config::ConvertToType(const std::string& input) const
	{
		TP_ERROR("[Config] Unconvertable type encountered, please use a different type, or define the handle case in Config.h");
		throw "[Config] Unconvertable type encountered, please use a different type, or define the handle case in Config.h";
	}

	template<>
	inline int Config::ConvertToType<int>(const std::string& input) const
	{
		int value;
		std::stringstream ss(input);
		ss >> value;

		return value;
	}

	template<>
	inline unsigned int Config::ConvertToType<unsigned int>(const std::string& input) const
	{
		unsigned int value;
		std::stringstream ss(input);
		ss >> value;

		return value;
	}

	template<>
	inline double Config::ConvertToType<double>(const std::string& input) const
	{
		double value;
		std::stringstream ss(input);
		ss >> value;

		return value;
	}

	template<>
	inline float Config::ConvertToType<float>(const std::string& input) const
	{
		float value;
		std::stringstream ss(input);
		ss >> value;

		return value;
	}

	template<>
	inline short Config::ConvertToType<short>(const std::string& input) const
	{
		short value;
		std::stringstream ss(input);
		ss >> value;

		return value;
	}

	template<>
	inline bool Config::ConvertToType<bool>(const std::string& input) const
	{
		return input == "TRUE" ? true : false;
	}

	template<>
	inline char Config::ConvertToType<char>(const std::string& input) const
	{
		return input[0];
	}

	template<>
	inline unsigned char Config::ConvertToType<unsigned char>(const std::string& input) const
	{
		return input[0];
	}

	template<>
	inline std::string Config::ConvertToType<std::string>(const std::string& input) const
	{
		return input;
	}

	template<>
	inline DisplayMode Config::ConvertToType<DisplayMode>(const std::string& input) const
	{
		if (input == "Windowed")
			return DisplayMode::WINDOWED;
		if (input == "Borderless")
			return DisplayMode::BORDERLESS;
		if (input == "Fullscreen")
			return DisplayMode::FULLSCREEN;

		return DisplayMode::WINDOWED;
	}

	template<>
	inline Graphics::API::RenderAPI Config::ConvertToType<Graphics::API::RenderAPI>(const std::string & input) const
	{
		if (input == "Vulkan")
			return Graphics::API::RenderAPI::VULKAN;
		if (input == "D3D12")
			return Graphics::API::RenderAPI::D3D12;
		if (input == "OpenGL")
			return Graphics::API::RenderAPI::OPENGL;

		return Graphics::API::RenderAPI::NONE;
	}

	template<typename T>
	void Config::Get(const std::string & key, T & value) const
	{
		const auto it = std::find_if(m_data.begin(), m_data.end(), [&key](const std::pair<std::string, std::string>& element) {return element.first == key; });
		if (it != m_data.end())
			value = ConvertToType<T>(it->second);
	}

	//This method tries to read the value of a key into a vector.
	//The values have to be separated by comma.
	//The vector is cleared before it it filled.
	template<typename T>
	void Config::Get(const std::string & key, std::vector<T> & value) const
	{
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

	template<typename T>
	void Config::Set(const std::string & key, const T value)
	{
		//The [] operator replaces the value if the key is found
		m_isChanged = true;
		for (auto& item : m_data)
		{
			if (item.first == key)
			{
				item.second = ConvertToString<T>(value);

				return;
			}
		}

		//If not it creates a new element
		m_data.push_back(std::make_pair(key, ConvertToString<T>(value)));
	}

	template<typename T>
	void Config::Set(const std::string & key, const std::vector<T> value)
	{
		//Transform the vector into a string that separates the elements with a comma
		const std::string valueAsString;
		for (std::size_t i = 0; i < value.size() - 1; ++i)
			valueAsString += ConvertToString<T>(value[i]) + ',';
		valueAsString += ConvertToString<T>(value.back());

		//The [] operator replaces the value if the key is found, if not it creates a new element
		m_isChanged = true;
		for (auto& item : m_data)
		{
			if (item.first == key)
			{
				item.second = valueAsString;

				return;
			}
		}

		//If not it creates a new element
		m_data.push_back(std::make_pair(key, valueAsString));
	}
}

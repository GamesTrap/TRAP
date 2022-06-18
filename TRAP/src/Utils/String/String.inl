#ifndef TRAP_STRING_INL
#define TRAP_STRING_INL

template<typename T>
T TRAP::Utils::String::ConvertToType(const std::string&)
{
	TRAP_ASSERT(false, "Unconvertable type encountered, please use a different type, "
	                   "or define the handle case in Config.h");
	TP_ERROR(TRAP::Log::ConfigPrefix, "Unconvertable type encountered, please use a different type, "
	                                  "or define the handle case in Config.h");
	return T();
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline int8_t TRAP::Utils::String::ConvertToType<int8_t>(const std::string& input)
{
	try
	{
		const int32_t value = std::stoi(input);
		if (value > std::numeric_limits<int8_t>::max() ||
		    value < std::numeric_limits<int8_t>::min())
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
inline uint8_t TRAP::Utils::String::ConvertToType<uint8_t>(const std::string& input)
{
	try
	{
		const uint32_t value = std::stoul(input);
		if (value > std::numeric_limits<uint8_t>::max() ||
		    value < std::numeric_limits<uint8_t>::min())
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
inline int16_t TRAP::Utils::String::ConvertToType<int16_t>(const std::string& input)
{
	try
	{
		const int32_t value = std::stoi(input);
		if (value > std::numeric_limits<int16_t>::max() ||
		    value < std::numeric_limits<int16_t>::min())
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
inline uint16_t TRAP::Utils::String::ConvertToType<uint16_t>(const std::string& input)
{
	try
	{
		const uint32_t value = std::stoul(input);
		if (value > std::numeric_limits<uint16_t>::max() ||
		    value < std::numeric_limits<uint16_t>::min())
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
inline int32_t TRAP::Utils::String::ConvertToType<int32_t>(const std::string& input)
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
inline uint32_t TRAP::Utils::String::ConvertToType<uint32_t>(const std::string& input)
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
inline int64_t TRAP::Utils::String::ConvertToType<int64_t>(const std::string& input)
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
inline uint64_t TRAP::Utils::String::ConvertToType<uint64_t>(const std::string& input)
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
inline float TRAP::Utils::String::ConvertToType<float>(const std::string& input)
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
inline double TRAP::Utils::String::ConvertToType<double>(const std::string& input)
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
inline bool TRAP::Utils::String::ConvertToType<bool>(const std::string& input)
{
	if (String::CompareAnyCase("TRUE", input))
		return true;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline char TRAP::Utils::String::ConvertToType<char>(const std::string& input)
{
	return input[0];
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToType<std::string>(const std::string& input)
{
	return input;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string_view TRAP::Utils::String::ConvertToType<std::string_view>(const std::string& input)
{
	return input;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline TRAP::Window::DisplayMode TRAP::Utils::String::ConvertToType<TRAP::Window::DisplayMode>(const std::string& input)
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
inline TRAP::Graphics::RenderAPI TRAP::Utils::String::ConvertToType<TRAP::Graphics::RenderAPI>(const std::string& input)
{
	if (Utils::String::CompareAnyCase("Vulkan", input) || Utils::String::CompareAnyCase("VulkanAPI", input))
		return Graphics::RenderAPI::Vulkan;

	if (Utils::String::CompareAnyCase("NONE", input))
		return Graphics::RenderAPI::NONE;

	TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::API::RenderAPI!");
	return Graphics::RenderAPI::NONE;
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Utils::String::ConvertToString(T)
{
	TP_ERROR(TRAP::Log::ConfigPrefix, "Unsupported type supplied, either change types, "
	                                  "or write a correct conversion function for the template type.");
	throw "[Config] Unsupported type supplied, either change types, or write a correct conversion "
	      "function for the template type.";
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<std::string>(std::string value)
{
	return value;
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<const char*>(const char* value)
{
	return std::string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<bool>(const bool value)
{
	return (value) ? "True" : "False";
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<char>(const char value)
{
	return std::string(1, value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<uint8_t>(const uint8_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<int16_t>(const int16_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<uint16_t>(const uint16_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<int32_t>(const int32_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<uint32_t>(const uint32_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<int64_t>(const int64_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<uint64_t>(const uint64_t value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<float>(const float value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<double>(const double value)
{
	return std::to_string(value);
}

//-------------------------------------------------------------------------------------------------------------------//

template<>
inline std::string TRAP::Utils::String::ConvertToString<TRAP::Window::DisplayMode>(const Window::DisplayMode value)
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
inline std::string TRAP::Utils::String::ConvertToString<TRAP::Graphics::RenderAPI>(const Graphics::RenderAPI value)
{
	switch (value)
	{
	case Graphics::RenderAPI::Vulkan:
		return "Vulkan";

	case Graphics::RenderAPI::NONE:
		return "NONE";

	default:
		return "";
	}
}

#endif /*TRAP_STRING_INL*/
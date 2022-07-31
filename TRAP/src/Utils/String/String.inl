#ifndef TRAP_STRING_INL
#define TRAP_STRING_INL

template<typename T>
T TRAP::Utils::String::ConvertToType(const std::string& input)
{
	if constexpr(std::is_integral_v<T> && std::is_signed_v<T> && !std::is_same_v<T, char> && !std::is_same_v<T, bool>) //Integers
	{
		try
		{
			const int64_t value = std::stoll(input);
			if (value > std::numeric_limits<T>::max() ||
				value < std::numeric_limits<T>::min())
				return 0;

			return static_cast<T>(value);
		}
		catch (std::exception&)
		{
			TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
			return 0;
		}
	}
	else if constexpr(std::is_integral_v<T> && !std::is_signed_v<T> && !std::is_same_v<T, char> && !std::is_same_v<T, bool>) //Unsigned integers
	{
		try
		{
			const uint64_t value = std::stoull(input);
			if (value > std::numeric_limits<T>::max() ||
				value < std::numeric_limits<T>::min())
				return 0;

			return static_cast<T>(value);
		}
		catch (std::exception&)
		{
			TP_ERROR(TRAP::Log::UtilsStringPrefix, "Exception while converting string!");
			return 0;
		}
	}
	else if constexpr(std::is_same_v<T, float>) //Float
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
	else if constexpr(std::is_same_v<T, double>) //Double
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
	else if constexpr(std::is_same_v<T, bool>) //Bool
	{
		if (String::CompareAnyCase("TRUE", input))
			return true;

		return false;
	}
	else if constexpr(std::is_same_v<T, char>) //Char
	{
		return input[0];
	}
	else if constexpr(std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) //String
	{
		return input;
	}
	else if constexpr(std::is_same_v<T, TRAP::Window::DisplayMode>) //DisplayMode
	{
		if (Utils::String::CompareAnyCase("Windowed", input))
			return Window::DisplayMode::Windowed;
		if (Utils::String::CompareAnyCase("Fullscreen (Borderless)", input))
			return Window::DisplayMode::Borderless;
		if (Utils::String::CompareAnyCase("Fullscreen", input))
			return Window::DisplayMode::Fullscreen;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Window::DisplayMode!");
		return Window::DisplayMode::Windowed;
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RenderAPI>) //RenderAPI
	{
		if (Utils::String::CompareAnyCase("Vulkan", input) || Utils::String::CompareAnyCase("VulkanAPI", input))
			return Graphics::RenderAPI::Vulkan;

		if (Utils::String::CompareAnyCase("NONE", input))
			return Graphics::RenderAPI::NONE;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RenderAPI!");
		return Graphics::RenderAPI::NONE;
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::AntiAliasing>) //AntiAliasing
	{
		if (Utils::String::CompareAnyCase("off", input))
			return Graphics::RendererAPI::AntiAliasing::Off;

		if (Utils::String::CompareAnyCase("msaa", input))
			return Graphics::RendererAPI::AntiAliasing::MSAA;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::AntiAliasing!");
		return Graphics::RendererAPI::AntiAliasing::Off;
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::SampleCount>) //SampleCount
	{
		if (Utils::String::CompareAnyCase("1", input))
			return Graphics::RendererAPI::SampleCount::One;

		if (Utils::String::CompareAnyCase("2", input))
			return Graphics::RendererAPI::SampleCount::Two;

		if (Utils::String::CompareAnyCase("4", input))
			return Graphics::RendererAPI::SampleCount::Four;

		if (Utils::String::CompareAnyCase("8", input))
			return Graphics::RendererAPI::SampleCount::Eight;

		if (Utils::String::CompareAnyCase("16", input))
			return Graphics::RendererAPI::SampleCount::Sixteen;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::SampleCount!");
		return Graphics::RendererAPI::SampleCount::One;
	}
	else if constexpr(std::is_same_v<T, TRAP::Utils::LinuxWindowManager>) //LinuxWindowManager
	{
		if(Utils::String::CompareAnyCase("X11", input))
			return Utils::LinuxWindowManager::X11;

		if(Utils::String::CompareAnyCase("Wayland", input))
			return Utils::LinuxWindowManager::Wayland;

		return Utils::LinuxWindowManager::Unknown;
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unconvertable type encountered, please use a different type, "
									  "or define the handle case in String.inl");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
std::string TRAP::Utils::String::ConvertToString(T value)
{
	if constexpr(std::is_same_v<T, std::string>)
	{
		return value;
	}
	else if constexpr(std::is_same_v<T, const char*>)
	{
		return std::string(value);
	}
	else if constexpr(std::is_same_v<T, bool>)
	{
		return (value) ? "True" : "False";
	}
	else if constexpr(std::is_same_v<T, char>)
	{
		return std::string(1, value);
	}
	else if constexpr(std::is_integral_v<T> || std::is_floating_point_v<T>)
	{
		return std::to_string(value);
	}
	else if constexpr(std::is_same_v<T, TRAP::Window::DisplayMode>)
	{
		switch (value)
		{
		case Window::DisplayMode::Windowed:
			return "Windowed";

		case Window::DisplayMode::Borderless:
			return "Fullscreen (Borderless)";

		case Window::DisplayMode::Fullscreen:
			return "Fullscreen";

		default:
			return "Windowed";
		}
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RenderAPI>)
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
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::AntiAliasing>)
	{
		switch (value)
		{
		case Graphics::RendererAPI::AntiAliasing::Off:
			return "Off";

		case Graphics::RendererAPI::AntiAliasing::MSAA:
			return "MSAA";

		default:
			return "";
		}
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::SampleCount>)
	{
		switch (value)
		{
		case Graphics::RendererAPI::SampleCount::One:
			return "1";

		case Graphics::RendererAPI::SampleCount::Two:
			return "2";

		case Graphics::RendererAPI::SampleCount::Four:
			return "4";

		case Graphics::RendererAPI::SampleCount::Eight:
			return "8";

		case Graphics::RendererAPI::SampleCount::Sixteen:
			return "16";

		default:
			return "";
		}
	}
	else if constexpr(std::is_same_v<T, TRAP::Utils::LinuxWindowManager>)
	{
		switch(value)
		{
		case LinuxWindowManager::X11:
			return "X11";

		case LinuxWindowManager::Wayland:
			return "Wayland";

		default:
			return "Unknown";
		}
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unsupported type supplied, either change types, "
	                                  "or write a correct conversion function for the template type in String.inl.");
	}
}

#endif /*TRAP_STRING_INL*/
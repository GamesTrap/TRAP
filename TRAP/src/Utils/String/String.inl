#ifndef TRAP_STRING_INL
#define TRAP_STRING_INL

template<typename T>
[[nodiscard]] T TRAP::Utils::String::ConvertToType(const std::string& input)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

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
#ifndef TRAP_HEADLESS_MODE
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
#endif /*TRAP_HEADLESS_MODE*/
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
		if (Utils::String::CompareAnyCase("Off", input))
			return Graphics::RendererAPI::AntiAliasing::Off;

		if (Utils::String::CompareAnyCase("MSAA", input))
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
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::GPUVendor>) //GPUVendor
	{
		if(Utils::String::CompareAnyCase("AMD", input))
			return Graphics::RendererAPI::GPUVendor::AMD;

		if(Utils::String::CompareAnyCase("ImgTec", input))
			return Graphics::RendererAPI::GPUVendor::ImgTec;

		if(Utils::String::CompareAnyCase("NVIDIA", input))
			return Graphics::RendererAPI::GPUVendor::NVIDIA;

		if(Utils::String::CompareAnyCase("ARM", input))
			return Graphics::RendererAPI::GPUVendor::ARM;

		if(Utils::String::CompareAnyCase("Broadcom", input))
			return Graphics::RendererAPI::GPUVendor::Broadcom;

		if(Utils::String::CompareAnyCase("Qualcomm", input))
			return Graphics::RendererAPI::GPUVendor::Qualcomm;

		if(Utils::String::CompareAnyCase("Intel", input))
			return Graphics::RendererAPI::GPUVendor::Intel;

		if(Utils::String::CompareAnyCase("Apple", input))
			return Graphics::RendererAPI::GPUVendor::Apple;

		if(Utils::String::CompareAnyCase("Vivante", input))
			return Graphics::RendererAPI::GPUVendor::Vivante;

		if(Utils::String::CompareAnyCase("VeriSilicon", input))
			return Graphics::RendererAPI::GPUVendor::VeriSilicon;

		if(Utils::String::CompareAnyCase("Kazan", input))
			return Graphics::RendererAPI::GPUVendor::Kazan;

		if(Utils::String::CompareAnyCase("Codeplay", input))
			return Graphics::RendererAPI::GPUVendor::Codeplay;

		if(Utils::String::CompareAnyCase("Mesa", input))
			return Graphics::RendererAPI::GPUVendor::Mesa;

		return Graphics::RendererAPI::GPUVendor::Unknown;
	}
#ifndef TRAP_HEADLESS_MODE
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::LatencyMode>) //LatencyMode
	{
		if(Utils::String::CompareAnyCase("Enabled", input))
			return Graphics::RendererAPI::LatencyMode::Enabled;
		if(Utils::String::CompareAnyCase("Enabled+Boost", input))
			return Graphics::RendererAPI::LatencyMode::EnabledBoost;
		if(Utils::String::CompareAnyCase("Disabled", input))
			return Graphics::RendererAPI::LatencyMode::Disabled;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::LatencyMode!");
		return Graphics::RendererAPI::LatencyMode::Disabled;
	}
#endif /*TRAP_HEADLESS_MODE*/
	else if constexpr(std::is_same_v<T, TRAP::FileSystem::FileStatus>) //FileStatus
	{
		if(Utils::String::CompareAnyCase("Created", input))
			return FileSystem::FileStatus::Created;
		if(Utils::String::CompareAnyCase("Renamed", input))
			return FileSystem::FileStatus::Renamed;
		if(Utils::String::CompareAnyCase("Modified", input))
			return FileSystem::FileStatus::Modified;
		if(Utils::String::CompareAnyCase("Erased", input))
			return FileSystem::FileStatus::Erased;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::FileSystem::FileStatus!");
		throw std::invalid_argument("Exception while converting string to TRAP::FileSystem::FileStatus!");
	}
#ifndef TRAP_HEADLESS_MODE
	else if constexpr(std::is_same_v<T, TRAP::Input::MouseButton>) //MouseButton
	{
		if(Utils::String::CompareAnyCase("Left", input))
			return Input::MouseButton::Left;
		if(Utils::String::CompareAnyCase("Right", input))
			return Input::MouseButton::Right;
		if(Utils::String::CompareAnyCase("Middle", input))
			return Input::MouseButton::Middle;
		if(Utils::String::CompareAnyCase("Button1", input))
			return Input::MouseButton::One;
		if(Utils::String::CompareAnyCase("Button2", input))
			return Input::MouseButton::Two;
		if(Utils::String::CompareAnyCase("Button3", input))
			return Input::MouseButton::Three;
		if(Utils::String::CompareAnyCase("Button4", input))
			return Input::MouseButton::Four;
		if(Utils::String::CompareAnyCase("Button5", input))
			return Input::MouseButton::Five;
		if(Utils::String::CompareAnyCase("Button6", input))
			return Input::MouseButton::Six;
		if(Utils::String::CompareAnyCase("Button7", input))
			return Input::MouseButton::Seven;
		if(Utils::String::CompareAnyCase("Button8", input))
			return Input::MouseButton::Eight;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Input::MouseButton!");
		throw std::invalid_argument("Exception while converting string to TRAP::Input::MouseButton!");
	}
#endif /*TRAP_HEADLESS_MODE*/
	else if constexpr(std::is_same_v<T, TRAP::Rigidbody2DComponent::BodyType>) //Rigidbody2DComponent::BodyType
	{
		if(Utils::String::CompareAnyCase("Static", input))
			return Rigidbody2DComponent::BodyType::Static;
		if(Utils::String::CompareAnyCase("Dynamic", input))
			return Rigidbody2DComponent::BodyType::Dynamic;
		if(Utils::String::CompareAnyCase("Kinematic", input))
			return Rigidbody2DComponent::BodyType::Kinematic;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Rigidbody2DComponent::BodyType!");
		throw std::invalid_argument("Exception while converting string to TRAP::Rigidbody2DComponent::BodyType!");
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::ResourceState>)
	{
		if(Utils::String::CompareAnyCase("Undefined", input))
			return TRAP::Graphics::RendererAPI::ResourceState::Undefined;
		if(Utils::String::CompareAnyCase("VertexAndConstantBuffer", input))
			return TRAP::Graphics::RendererAPI::ResourceState::VertexAndConstantBuffer;
		if(Utils::String::CompareAnyCase("IndexBuffer", input))
			return TRAP::Graphics::RendererAPI::ResourceState::IndexBuffer;
		if(Utils::String::CompareAnyCase("RenderTarget", input))
			return TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
		if(Utils::String::CompareAnyCase("UnorderedAccess", input))
			return TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess;
		if(Utils::String::CompareAnyCase("DepthWrite", input))
			return TRAP::Graphics::RendererAPI::ResourceState::DepthWrite;
		if(Utils::String::CompareAnyCase("DepthRead", input))
			return TRAP::Graphics::RendererAPI::ResourceState::DepthRead;
		if(Utils::String::CompareAnyCase("NonPixelShaderResource", input))
			return TRAP::Graphics::RendererAPI::ResourceState::NonPixelShaderResource;
		if(Utils::String::CompareAnyCase("PixelShaderResource", input))
			return TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource;
		if(Utils::String::CompareAnyCase("ShaderResource", input))
			return TRAP::Graphics::RendererAPI::ResourceState::ShaderResource;
		if(Utils::String::CompareAnyCase("StreamOut", input))
			return TRAP::Graphics::RendererAPI::ResourceState::StreamOut;
		if(Utils::String::CompareAnyCase("IndirectArgument", input))
			return TRAP::Graphics::RendererAPI::ResourceState::IndirectArgument;
		if(Utils::String::CompareAnyCase("CopyDestination", input))
			return TRAP::Graphics::RendererAPI::ResourceState::CopyDestination;
		if(Utils::String::CompareAnyCase("CopySource", input))
			return TRAP::Graphics::RendererAPI::ResourceState::CopySource;
		if(Utils::String::CompareAnyCase("GenericRead", input))
			return TRAP::Graphics::RendererAPI::ResourceState::GenericRead;
		if(Utils::String::CompareAnyCase("Present", input))
			return TRAP::Graphics::RendererAPI::ResourceState::Present;
		if(Utils::String::CompareAnyCase("Common", input))
			return TRAP::Graphics::RendererAPI::ResourceState::Common;
		if(Utils::String::CompareAnyCase("RayTracingAccelerationStructure", input))
			return TRAP::Graphics::RendererAPI::ResourceState::RayTracingAccelerationStructure;
		if(Utils::String::CompareAnyCase("ShadingRateSource", input))
			return TRAP::Graphics::RendererAPI::ResourceState::ShadingRateSource;

		TP_ERROR(TRAP::Log::ConfigPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::ResourceState!");
		throw std::invalid_argument("Exception while converting string to TRAP::Graphics::RendererAPI::ResourceState!");
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::QueueType>) //QueueType
	{
		if (Utils::String::CompareAnyCase("Graphics", input))
			return TRAP::Graphics::RendererAPI::QueueType::Graphics;
		if (Utils::String::CompareAnyCase("Transfer", input))
			return TRAP::Graphics::RendererAPI::QueueType::Transfer;
		if (Utils::String::CompareAnyCase("Compute", input))
			return TRAP::Graphics::RendererAPI::QueueType::Compute;

		TP_ERROR(TRAP::Log::RendererPrefix, "Exception while converting string to TRAP::Graphics::RendererAPI::QueueType!");
		throw std::invalid_argument("Exception while converting string to TRAP::Graphics::RendererAPI::QueueType!");
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unconvertable type encountered, please use a different type, "
									  "or define the handle case in String.inl");
	}
}

//-------------------------------------------------------------------------------------------------------------------//

template<typename T>
[[nodiscard]] std::string TRAP::Utils::String::ConvertToString(T value)
{
	ZoneNamedC(__tracy, tracy::Color::Violet, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Utils);

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
#ifndef TRAP_HEADLESS_MODE
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
#endif /*TRAP_HEADLESS_MODE*/
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
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::GPUVendor>) //GPUVendor
	{
		switch(value)
		{
		case Graphics::RendererAPI::GPUVendor::AMD:
			return "AMD";
		case Graphics::RendererAPI::GPUVendor::ImgTec:
			return "ImgTec";
		case Graphics::RendererAPI::GPUVendor::NVIDIA:
			return "NVIDIA";
		case Graphics::RendererAPI::GPUVendor::ARM:
			return "ARM";
		case Graphics::RendererAPI::GPUVendor::Broadcom:
			return "Broadcom";
		case Graphics::RendererAPI::GPUVendor::Qualcomm:
			return "Qualcomm";
		case Graphics::RendererAPI::GPUVendor::Intel:
			return "Intel";
		case Graphics::RendererAPI::GPUVendor::Apple:
			return "Apple";
		case Graphics::RendererAPI::GPUVendor::Vivante:
			return "Vivante";
		case Graphics::RendererAPI::GPUVendor::VeriSilicon:
			return "VeriSilicon";
		case Graphics::RendererAPI::GPUVendor::Kazan:
			return "Kazan";
		case Graphics::RendererAPI::GPUVendor::Codeplay:
			return "Codeplay";
		case Graphics::RendererAPI::GPUVendor::Mesa:
			return "Mesa";

		default:
			return "Unknown";
		}
	}
#ifndef TRAP_HEADLESS_MODE
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::LatencyMode>) //LatencyMode
	{
		switch(value)
		{
		case Graphics::RendererAPI::LatencyMode::Enabled:
			return "Enabled";
		case Graphics::RendererAPI::LatencyMode::EnabledBoost:
			return "Enabled+Boost";

		case Graphics::RendererAPI::LatencyMode::Disabled:
			[[fallthrough]];
		default:
			return "Disabled";
		}
	}
#endif /*TRAP_HEADLESS_MODE*/
	else if constexpr(std::is_same_v<T, TRAP::FileSystem::FileStatus>) //FileStatus
	{
		switch(value)
		{
		case FileSystem::FileStatus::Created:
			return "Created";

		case FileSystem::FileStatus::Renamed:
			return "Renamed";

		case FileSystem::FileStatus::Modified:
			return "Modified";

		case FileSystem::FileStatus::Erased:
			return "Erased";

		default:
			return "";
		}
	}
#ifndef TRAP_HEADLESS_MODE
	else if constexpr(std::is_same_v<T, TRAP::Input::MouseButton>) //MouseButton
	{
		switch(value)
		{
		case Input::MouseButton::Left:
			return "Left";

		case Input::MouseButton::Right:
			return "Right";

		case Input::MouseButton::Middle:
			return "Middle";

		// case Input::MouseButton::One:
		// 	return "Button1";

		// case Input::MouseButton::Two:
		// 	return "Button2";

		// case Input::MouseButton::Three:
		// 	return "Button3";

		case Input::MouseButton::Four:
			return "Button4";

		case Input::MouseButton::Five:
			return "Button5";

		case Input::MouseButton::Six:
			return "Button6";

		case Input::MouseButton::Seven:
			return "Button7";

		case Input::MouseButton::Eight:
			return "Button8";

		default:
			return "";
		}
	}
#endif /*TRAP_HEADLESS_MODE*/
	else if constexpr(std::is_same_v<T, TRAP::Rigidbody2DComponent::BodyType>) //Rigidbody2DComponent::BodyType
	{
		switch(value)
		{
		case Rigidbody2DComponent::BodyType::Static:
			return "Static";

		case Rigidbody2DComponent::BodyType::Dynamic:
			return "Dynamic";

		case Rigidbody2DComponent::BodyType::Kinematic:
			return "Kinematic";

		default:
			return "";
		}
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::ResourceState>)
	{
		switch(value)
		{
		case TRAP::Graphics::RendererAPI::ResourceState::Undefined:
			return "Undefined";
		case TRAP::Graphics::RendererAPI::ResourceState::VertexAndConstantBuffer:
			return "VertexAndConstantBuffer";
		case TRAP::Graphics::RendererAPI::ResourceState::IndexBuffer:
			return "IndexBuffer";
		case TRAP::Graphics::RendererAPI::ResourceState::RenderTarget:
			return "RenderTarget";
		case TRAP::Graphics::RendererAPI::ResourceState::UnorderedAccess:
			return "UnorderedAccess";
		case TRAP::Graphics::RendererAPI::ResourceState::DepthWrite:
			return "DepthWrite";
		case TRAP::Graphics::RendererAPI::ResourceState::DepthRead:
			return "DepthRead";
		case TRAP::Graphics::RendererAPI::ResourceState::NonPixelShaderResource:
			return "NonPixelShaderResource";
		case TRAP::Graphics::RendererAPI::ResourceState::PixelShaderResource:
			return "PixelShaderResource";
		case TRAP::Graphics::RendererAPI::ResourceState::ShaderResource:
			return "ShaderResource";
		case TRAP::Graphics::RendererAPI::ResourceState::StreamOut:
			return "StreamOut";
		case TRAP::Graphics::RendererAPI::ResourceState::IndirectArgument:
			return "IndirectArgument";
		case TRAP::Graphics::RendererAPI::ResourceState::CopyDestination:
			return "CopyDestination";
		case TRAP::Graphics::RendererAPI::ResourceState::CopySource:
			return "CopySource";
		case TRAP::Graphics::RendererAPI::ResourceState::GenericRead:
			return "GenericRead";
		case TRAP::Graphics::RendererAPI::ResourceState::Present:
			return "Present";
		case TRAP::Graphics::RendererAPI::ResourceState::Common:
			return "Common";
		case TRAP::Graphics::RendererAPI::ResourceState::RayTracingAccelerationStructure:
			return "RayTracingAccelerationStructure";
		case TRAP::Graphics::RendererAPI::ResourceState::ShadingRateSource:
			return "ShadingRateSource";

		default:
			return "";
		}
	}
	else if constexpr(std::is_same_v<T, TRAP::Graphics::RendererAPI::QueueType>) //QueueType
	{
		switch(value)
		{
		case TRAP::Graphics::RendererAPI::QueueType::Graphics:
			return "Graphics";

		case TRAP::Graphics::RendererAPI::QueueType::Transfer:
			return "Transfer";

		case TRAP::Graphics::RendererAPI::QueueType::Compute:
			return "Compute";

		default:
			return "";
		}
	}
	else
	{
		static_assert(sizeof(T) == 0, "Unsupported type supplied, either change types, "
	                                  "or write a correct conversion function for the template type in String.inl.");
	}
}

#endif /*TRAP_STRING_INL*/
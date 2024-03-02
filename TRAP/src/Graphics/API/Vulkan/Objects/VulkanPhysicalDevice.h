#ifndef TRAP_VULKANPHYSICALDEVICE_H
#define TRAP_VULKANPHYSICALDEVICE_H

#include <map>

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	struct RatedVulkanPhysicalDevice
	{
		TRAP::Utils::UUID PhysicalDeviceUUID{};
		std::string PhysicalDeviceName{};
	};

	class VulkanPhysicalDevice
	{
	public:
		/// @brief Constructor.
		/// @param instance Vulkan instance.
		/// @param physicalDeviceUUID UUID of physical device to create.
		VulkanPhysicalDevice(TRAP::Ref<VulkanInstance> instance,
			                 const TRAP::Utils::UUID& physicalDeviceUUID);
		/// @brief Destructor.
		~VulkanPhysicalDevice();

		/// @brief Copy constructor.
		consteval VulkanPhysicalDevice(const VulkanPhysicalDevice&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) noexcept = delete;
		/// @brief Move constructor.
		VulkanPhysicalDevice(VulkanPhysicalDevice&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) noexcept = default;

		/// @brief Retrieve the Vulkan physical device handle.
		/// @return Vulkan physical device handle.
		[[nodiscard]] constexpr VkPhysicalDevice GetVkPhysicalDevice() const noexcept;
		/// @brief Retrieve the Vulkan physical device format properties for a specific format.
		/// @param format Format to retrieve properties for.
		/// @return VkFormatProperties.
		[[nodiscard]] constexpr const VkFormatProperties& GetVkPhysicalDeviceFormatProperties(ImageFormat format) const noexcept;
		/// @brief Retrieve the Vulkan physical device properties.
		/// @return VkPhysicalDeviceProperties.
		[[nodiscard]] constexpr const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const noexcept;
		/// @brief Retrieve the Vulkan physical device subgroup properties.
		/// @return VkPhysicalDeviceSubgroupProperties.
		[[nodiscard]] constexpr const VkPhysicalDeviceSubgroupProperties& GetVkPhysicalDeviceSubgroupProperties() const noexcept;
		/// @brief Retrieve the Vulkan physical device id properties.
		/// @return VkPhysicalDeviceIDProperties.
		[[nodiscard]] constexpr const VkPhysicalDeviceIDProperties& GetVkPhysicalDeviceIDProperties() const noexcept;
		/// @brief Retrieve the Vulkan physical device memory properties.
		/// @return VkPhysicalDeviceMemoryProperties.
		[[nodiscard]] constexpr const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties() const noexcept;
		/// @brief Retrieve the Vulkan physical device features.
		/// @return VkPhysicalDeviceFeatures.
		[[nodiscard]] constexpr const VkPhysicalDeviceFeatures& GetVkPhysicalDeviceFeatures() const noexcept;
		/// @brief Retrieve the Vulkan physical device driver properties.
		/// @return VkPhysicalDeviceDriverProperties.
		[[nodiscard]] constexpr const VkPhysicalDeviceDriverProperties& GetVkPhysicalDeviceDriverProperties() const noexcept;
		/// @brief Retrieve the Vulkan queue family properties of each queue family.
		/// @return List of VkQueueFamilyProperties.
		[[nodiscard]] constexpr const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const noexcept;
		/// @brief Check whether an extension is supported by the physical device or not.
		/// @param extension Extension to check.
		/// @return True if extension is supported, false otherwise.
		[[nodiscard]] constexpr bool IsExtensionSupported(std::string_view extension) const;

		/// @brief Retrieve the properties for the given physical device extension.
		/// @param physicalDeviceExtension Physical device extension to get properties from.
		/// @return Physical device extension properties.
		[[nodiscard]] constexpr std::optional<VkExtensionProperties> GetPhysicalDeviceExtensionProperties(std::string_view physicalDeviceExtension) const;

		/// @brief Retrieve the physical device's UUID.
		/// @return Physical device UUID.
		[[nodiscard]] constexpr TRAP::Utils::UUID GetUUID() const noexcept;

		/// @brief Load the physical device fragment shader interlock features.
		/// Value is saved in RendererAPI::GPUSettings.ROVsSupported.
		/// @note Only call this function when VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION is enabled.
		void LoadPhysicalDeviceFragmentShaderInterlockFeatures();

		/// @brief Retrieve the vendor of the physical device.
		/// @return Vendor.
		[[nodiscard]] constexpr RendererAPI::GPUVendor GetVendor() const noexcept;

		/// @brief Retrieve the name of the physical device.
		/// @return Vendor.
		[[nodiscard]] constexpr std::string GetName() const noexcept;

		/// @brief Retrieve a list of all rated physical devices.
		/// Key is the devices score, value is the UUID of the physical device.
		/// @param instance Vulkan instance.
		/// @return List of rated physical devices.
		[[nodiscard]] static const std::multimap<u32, RatedVulkanPhysicalDevice>& GetAllRatedPhysicalDevices(const VulkanInstance& instance);

	private:
		friend bool TRAP::Graphics::RendererAPI::IsVulkanCapable();

		/// @brief Loads physical device properties and features for future use.
		void LoadDevicePropertiesAndFeatures();

		/// @brief Retrieve a list of all rated physical devices.
		/// Key is the devices score, value is the UUID of the physical device.
		/// @param instance Vulkan instance handle.
		/// @return List of rated physical devices.
		[[nodiscard]] static const std::multimap<u32, RatedVulkanPhysicalDevice>& GetAllRatedPhysicalDevices(VkInstance instance);

		/// @brief Rate a list of physical devices.
		///
		/// Does the following checks for rating the device (some are required, others are optional):
		///  1. Must be Vulkan API 1.1 capable
		///  2. Must be iGPU or dGPU (5000 score for dGPU, 2500 score for iGPU).
		///  3. Must support at least 4 simultaneously bound descriptor sets.
		///  4. Retrieves all supported extensions (50 score for each supported extension).
		///  5. Must support swapchain extensions (Disabled for Headless mode).
		///  6. Must be able to create a Vulkan surface test window (Disabled for Headless mode).
		///  7. Must be able to create a Vulkan surface (Disabled for Headless mode).
		///  8. Must have at least one queue family.
		///  9. Must support at least one graphics queue family.
		/// 10. Must support at least one queue family with present support (Disabled for Headless mode).
		/// 11. Must support at least one present mode (Disabled for Headless mode).
		/// 12. Must support at least one surface format (Disabled for Headless mode).
		/// 13. Optionally supports a compute queue family (1000 score for each).
		/// 14. Optionally supports a transfer queue family (1000 score for each).
		/// 15. Optionally supports RayTracing extensions (2000 score).
		/// 16. Optionally supports geometry shaders (1000 score).
		/// 17. Optionally supports tessellation shaders (1000 score).
		/// 18. Optionally supports Variable Rate Shading extensions and Tier 1/Tier 2 (1000 + 100/200 score).
		/// 19. Optionally supports fill mode non solid (250 score).
		/// 20. Optionally does surface has optimal format (B8G8R8A8 Unorm & SRGB Non-linear) (250 score) (Disabled for Headless mode).
		/// 21. Optionally check VRAM size (size / 1 Gigabyte * 100 score).
		/// 22. Optionally check max resolution of 2D and cube images. ((Max image resolution / 32) is score).
		/// 23. Optionally check max sample count for MSAA. (Sample count * 10 score).
		/// 24. Optionally check if anisotropic filtering is supported (500 score).
		/// @param physicalDevices Vulkan physical devices to rate.
		/// @param instance Vulkan instance used to retrieve the physical devices.
		static void RatePhysicalDevices(std::span<const VkPhysicalDevice> physicalDevices, [[maybe_unused]] VkInstance instance);

		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties m_physicalDeviceProperties{};
		VkPhysicalDeviceSubgroupProperties m_physicalDeviceSubgroupProperties{};
		VkPhysicalDeviceIDProperties m_physicalDeviceIDProperties{};
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties{};
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures{};
		VkPhysicalDeviceDriverProperties m_physicalDeviceDriverProperties{};
		std::array<VkFormatProperties, std::to_underlying(ImageFormat::IMAGE_FORMAT_COUNT)> m_physicalDeviceFormatProperties{};
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties{};

		std::vector<VkExtensionProperties> m_availablePhysicalDeviceExtensions{};

		TRAP::Ref<VulkanInstance> m_instance = nullptr; //Take ownership of VulkanInstance because we rely on it being around.

		//Multiple physical devices can theoretically have the same score, so we use a multimap here.
		inline static std::multimap<u32, RatedVulkanPhysicalDevice> s_ratedPhysicalDevices{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPhysicalDevice TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDevice() const noexcept
{
	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkFormatProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFormatProperties(const TRAP::Graphics::API::ImageFormat format) const noexcept
{
	return m_physicalDeviceFormatProperties[std::to_underlying(format)];
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkQueueFamilyProperties> &TRAP::Graphics::API::VulkanPhysicalDevice::GetQueueFamilyProperties() const noexcept
{
	return m_queueFamilyProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::API::VulkanPhysicalDevice::IsExtensionSupported(const std::string_view extension) const
{
	const auto result = std::ranges::find_if(m_availablePhysicalDeviceExtensions,
									         [extension](const VkExtensionProperties& props)
									         {
								                 return extension == props.extensionName;
									         });

	if (result == m_availablePhysicalDeviceExtensions.end())
	{
		if (extension == VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
			TP_WARN(Log::RendererVulkanPhysicalDevicePrefix, "Extension: \"", extension,
					"\" is not supported(Vulkan SDK installed?)");
		else
			TP_WARN(Log::RendererVulkanPhysicalDevicePrefix, "Extension: \"", extension, "\" is not supported");

		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::optional<VkExtensionProperties> TRAP::Graphics::API::VulkanPhysicalDevice::GetPhysicalDeviceExtensionProperties(const std::string_view physicalDeviceExtension) const
{
	const auto res = std::ranges::find_if(m_availablePhysicalDeviceExtensions, [physicalDeviceExtension](const VkExtensionProperties& extensionProps)
		{return physicalDeviceExtension == extensionProps.extensionName;});

	if(res == m_availablePhysicalDeviceExtensions.end())
		return std::nullopt;

	return *res;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceProperties() const noexcept
{
	return m_physicalDeviceProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceSubgroupProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceSubgroupProperties() const noexcept
{
	return m_physicalDeviceSubgroupProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceIDProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceIDProperties() const noexcept
{
	return m_physicalDeviceIDProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceMemoryProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceMemoryProperties() const noexcept
{
	return m_physicalDeviceMemoryProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceFeatures& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFeatures() const noexcept
{
	return m_physicalDeviceFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceDriverProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceDriverProperties() const noexcept
{
	return m_physicalDeviceDriverProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Utils::UUID TRAP::Graphics::API::VulkanPhysicalDevice::GetUUID() const noexcept
{
	TRAP::Utils::UUID devUUID{};
	std::ranges::copy(m_physicalDeviceIDProperties.deviceUUID, devUUID.begin());

	return devUUID;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::GPUVendor TRAP::Graphics::API::VulkanPhysicalDevice::GetVendor() const noexcept
{
	switch(m_physicalDeviceProperties.vendorID)
	{
	case 0x1002:
		[[fallthrough]];
	case 0x1010:
		[[fallthrough]];
	case 0x10DE:
		[[fallthrough]];
	case 0x13B5:
		[[fallthrough]];
	case 0x14E4:
		[[fallthrough]];
	case 0x5143:
		[[fallthrough]];
	case 0x8086:
		[[fallthrough]];
	case 0x106B:
		[[fallthrough]];
	case 0x7A05:
		[[fallthrough]];
	case 0x1EB1:
		[[fallthrough]];
	case 0x10003:
		[[fallthrough]];
	case 0x10004:
		[[fallthrough]];
	case 0x10005:
		return static_cast<RendererAPI::GPUVendor>(m_physicalDeviceProperties.vendorID);

	default:
		return RendererAPI::GPUVendor::Unknown;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::string TRAP::Graphics::API::VulkanPhysicalDevice::GetName() const noexcept
{
	return m_physicalDeviceProperties.deviceName;
}

#endif /*TRAP_VULKANPHYSICALDEVICE_H*/

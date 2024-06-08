#ifndef TRAP_VULKANPHYSICALDEVICE_H
#define TRAP_VULKANPHYSICALDEVICE_H

#include <map>

#include "Graphics/API/RendererAPI.h"
#include "Utils/Optional.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;
#ifndef TRAP_HEADLESS_MODE
	class VulkanSurface;
#endif /*TRAP_HEADLESS_MODE*/

	struct RatedVulkanPhysicalDevice
	{
		TRAP::Utils::UUID PhysicalDeviceUUID{};
		std::string PhysicalDeviceName{};
	};

	class VulkanPhysicalDevice final
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
		/// @brief Retrieve the Vulkan physical device image format properties for a specific format image type and image usage combination.
		/// @param format Format to retrieve image properties for.
		/// @param imageType Vulkan image type.
		/// @param imageUsageFlags Vulkan image usage flags.
		/// @return VkImageFormatProperties on success, empty optional otherwise.
		[[nodiscard]] TRAP::Optional<VkImageFormatProperties> GetVkPhysicalDeviceImageFormatProperties(ImageFormat format,
																					                   VkImageType imageType,
																					                   VkImageUsageFlags imageUsageFlags) const;
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

#ifndef TRAP_HEADLESS_MODE
		/// @brief Retrieve whether the given queue family supports presentation to the given surface.
		/// @param surface Surface to check for.
		/// @param queueFamilyIndex Queue family to check for.
		/// @return True if presentation is supported, false otherwise.
		[[nodiscard]] bool GetPhysicalDeviceSurfaceSupport(const VulkanSurface& surface, u32 queueFamilyIndex) const;
#endif /*TRAP_HEADLESS_MODE*/

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

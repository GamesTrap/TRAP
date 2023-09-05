#ifndef TRAP_VULKANPHYSICALDEVICE_H
#define TRAP_VULKANPHYSICALDEVICE_H

#include <map>

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;

	class VulkanPhysicalDevice
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="physicalDeviceUUID">UUID of physical device to create.</param>
		VulkanPhysicalDevice(const TRAP::Ref<VulkanInstance>& instance,
			                 const std::array<uint8_t, 16>& physicalDeviceUUID);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanPhysicalDevice();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr VulkanPhysicalDevice(const VulkanPhysicalDevice&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr VulkanPhysicalDevice(VulkanPhysicalDevice&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan physical device handle.
		/// </summary>
		/// <returns>Vulkan physical device handle.</returns>
		[[nodiscard]] constexpr VkPhysicalDevice GetVkPhysicalDevice() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device format properties for a specific format.
		/// </summary>
		/// <param name="format">Format to retrieve properties for.</param>
		/// <returns>VkFormatProperties.</returns>
		[[nodiscard]] VkFormatProperties GetVkPhysicalDeviceFormatProperties(VkFormat format) const;
		/// <summary>
		/// Retrieve the Vulkan physical device properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceProperties.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device subgroup properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceSubgroupProperties.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceSubgroupProperties& GetVkPhysicalDeviceSubgroupProperties() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device id properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceIDProperties.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceIDProperties& GetVkPhysicalDeviceIDProperties() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device memory properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceMemoryProperties.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device features.
		/// </summary>
		/// <returns>VkPhysicalDeviceFeatures.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceFeatures& GetVkPhysicalDeviceFeatures() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device fragment shader interlock features.
		/// </summary>
		/// <returns>VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT& GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan physical device driver properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceDriverProperties.</returns>
		[[nodiscard]] constexpr const VkPhysicalDeviceDriverProperties& GetVkPhysicalDeviceDriverProperties() const noexcept;
		/// <summary>
		/// Retrieve the Vulkan queue family properties of each queue family.
		/// </summary>
		/// <returns>List of VkQueueFamilyProperties.</returns>
		[[nodiscard]] constexpr const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const noexcept;
		/// <summary>
		/// Check whether an extension is supported by the physical device or not.
		/// </summary>
		/// <param name="extension">Extension to check.</param>
		/// <returns>True if extension is supported, false otherwise.</returns>
		[[nodiscard]] bool IsExtensionSupported(std::string_view extension);

		/// <summary>
		/// Retrieve a list of all available physical device extensions.
		/// </summary>
		/// <returns>List of physical device extensions.</returns>
		[[nodiscard]] const std::vector<VkExtensionProperties>& GetAvailablePhysicalDeviceExtensions();
		/// <summary>
		/// Retrieve the properties for the given physical device extension.
		/// </summary>
		/// <param name="physicalDeviceExtension">Physical device extension to get properties from.</param>
		/// <returns>Physical device extension properties.</returns>
		[[nodiscard]] std::optional<VkExtensionProperties> GetPhysicalDeviceExtensionProperties(std::string_view physicalDeviceExtension);

		/// <summary>
		/// Retrieve the physical device's UUID.
		/// </summary>
		/// <returns>Physical device UUID.</returns>
		[[nodiscard]] constexpr const std::array<uint8_t, 16>& GetPhysicalDeviceUUID() const noexcept;

		/// <summary>
		/// Retrieve the physical device fragment shader interlock features.
		/// Value is saved in RendererAPI::GPUSettings.ROVsSupported.
		/// </summary>
		void RetrievePhysicalDeviceFragmentShaderInterlockFeatures();

		/// <summary>
		/// Retrieve the vendor of the physical device.
		/// </summary>
		/// <returns>Vendor.</returns>
		[[nodiscard]] constexpr RendererAPI::GPUVendor GetVendor() const noexcept;

		/// <summary>
		/// Retrieve a list of all rated physical devices.
		/// Key is the devices score, value is the UUID of the physical device.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <returns>List of rated physical devices.</returns>
		[[nodiscard]] static const std::multimap<uint32_t, std::array<uint8_t, 16>>& GetAllRatedPhysicalDevices(const TRAP::Ref<VulkanInstance>& instance);

		/// <summary>
		/// Retrieve a phyiscal device via its UUID.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="physicalDeviceUUID">Physical device UUID to retrieve.</param>
		/// <returns>Vulkan physical device.</returns>
		[[nodiscard]] static VkPhysicalDevice FindPhysicalDeviceViaUUID(const TRAP::Ref<VulkanInstance>& instance,
			                                                            const std::array<uint8_t, 16>& physicalDeviceUUID);

	private:
		friend bool TRAP::Graphics::RendererAPI::IsVulkanCapable();
		/// <summary>
		/// Retrieve a list of all rated physical devices.
		/// Key is the devices score, value is the UUID of the physical device.
		/// </summary>
		/// <param name="instance">Vulkan instance handle.</param>
		/// <returns>List of rated physical devices.</returns>
		[[nodiscard]] static const std::multimap<uint32_t, std::array<uint8_t, 16>>& GetAllRatedPhysicalDevices(const VkInstance& instance);

		/// <summary>
		/// Retrieve a list of all Vulkan physical device handles.
		/// </summary>
		/// <param name="instance">Vulkan instance handle.</param>
		/// <returns>List of Vulkan physical device handles.</returns>
		[[nodiscard]] static std::vector<VkPhysicalDevice> GetAllVkPhysicalDevices(const VkInstance& instance);

		/// <summary>
		/// Rate a list of physical devices.
		///
		/// Does the following checks for rating the device (some are required, others are optional):
		///  1. Must be Vulkan API 1.2 capable
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
		/// 22. Optionally check max resolution of 2D and cube images. (Max image resolution is score).
		/// 23. Optionally check max sample count for MSAA. (Sample count * 10 score).
		/// 24. Optionally check if anisotropic filtering is supported (500 score).
		/// </summary>
		/// <param name="physicalDevices">Vulkan physical devices to rate.</param>
		/// <param name="instance">Vulkan instance used to retrieve the physical devices.</param>
		static void RatePhysicalDevices(const std::vector<VkPhysicalDevice>& physicalDevices, VkInstance instance);

		/// <summary>
		/// Load a list of available instance layers.
		/// </summary>
		void LoadAllPhysicalDeviceExtensions();

		/// <summary>
		/// Retrieve the max usable MSAA sample count for the GPU.
		/// </summary>
		/// <returns>Max usable MSAA sample count.</returns>
		[[nodiscard]] constexpr uint32_t GetMaxUsableMSAASampleCount() const noexcept;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceSubgroupProperties m_physicalDeviceSubgroupProperties;
		VkPhysicalDeviceIDProperties m_physicalDeviceIDProperties;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
		VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT m_physicalDeviceFragmentShaderInterlockFeatures;
		VkPhysicalDeviceDriverProperties m_physicalDeviceDriverProperties;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;

		std::array<uint8_t, 16> m_deviceUUID;

		std::vector<VkExtensionProperties> m_availablePhysicalDeviceExtensions;

		static std::multimap<uint32_t, std::array<uint8_t, 16>> s_availablePhysicalDeviceUUIDs;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkPhysicalDevice TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDevice() const noexcept
{
	return m_physicalDevice;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<VkQueueFamilyProperties> &TRAP::Graphics::API::VulkanPhysicalDevice::GetQueueFamilyProperties() const noexcept
{
	return m_queueFamilyProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceProperties() const noexcept
{
	return m_physicalDeviceProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceSubgroupProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceSubgroupProperties() const noexcept
{
	return m_physicalDeviceSubgroupProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceIDProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceIDProperties() const noexcept
{
	return m_physicalDeviceIDProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceMemoryProperties &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceMemoryProperties() const noexcept
{
	return m_physicalDeviceMemoryProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceFeatures &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFeatures() const noexcept
{
	return m_physicalDeviceFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT &TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const noexcept
{
	return m_physicalDeviceFragmentShaderInterlockFeatures;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const VkPhysicalDeviceDriverProperties& TRAP::Graphics::API::VulkanPhysicalDevice::GetVkPhysicalDeviceDriverProperties() const noexcept
{
	return m_physicalDeviceDriverProperties;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::array<uint8_t, 16> &TRAP::Graphics::API::VulkanPhysicalDevice::GetPhysicalDeviceUUID() const noexcept
{
	return m_deviceUUID;
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

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanPhysicalDevice::GetMaxUsableMSAASampleCount() const noexcept
{
	VkSampleCountFlags sampleCounts = TRAP::Math::Min(m_physicalDeviceProperties.limits.framebufferColorSampleCounts,
	                                                  m_physicalDeviceProperties.limits.framebufferDepthSampleCounts);
	sampleCounts = TRAP::Math::Min(sampleCounts, m_physicalDeviceProperties.limits.framebufferStencilSampleCounts);

	if((sampleCounts & VK_SAMPLE_COUNT_64_BIT) != 0u)
		return VK_SAMPLE_COUNT_64_BIT;
	if((sampleCounts & VK_SAMPLE_COUNT_32_BIT) != 0u)
		return VK_SAMPLE_COUNT_32_BIT;
	if((sampleCounts & VK_SAMPLE_COUNT_16_BIT) != 0u)
		return VK_SAMPLE_COUNT_16_BIT;
	if((sampleCounts & VK_SAMPLE_COUNT_8_BIT) != 0u)
		return VK_SAMPLE_COUNT_8_BIT;
	if((sampleCounts & VK_SAMPLE_COUNT_4_BIT) != 0u)
		return VK_SAMPLE_COUNT_4_BIT;
	if((sampleCounts & VK_SAMPLE_COUNT_2_BIT) != 0u)
		return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

#endif /*TRAP_VULKANPHYSICALDEVICE_H*/
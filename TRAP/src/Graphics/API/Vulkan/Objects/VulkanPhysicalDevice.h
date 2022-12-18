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
		VulkanPhysicalDevice(const VulkanPhysicalDevice&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanPhysicalDevice(VulkanPhysicalDevice&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan physical device handle.
		/// </summary>
		/// <returns>Vulkan physical device handle.</returns>
		VkPhysicalDevice GetVkPhysicalDevice() const;
		/// <summary>
		/// Retrieve the Vulkan physical device format properties for a specific format.
		/// </summary>
		/// <param name="format">Format to retrieve properties for.</param>
		/// <returns>VkFormatProperties.</returns>
		VkFormatProperties GetVkPhysicalDeviceFormatProperties(VkFormat format) const;
		/// <summary>
		/// Retrieve the Vulkan physical device properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceProperties.</returns>
		const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const;
		/// <summary>
		/// Retrieve the Vulkan physical device subgroup properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceSubgroupProperties.</returns>
		const VkPhysicalDeviceSubgroupProperties& GetVkPhysicalDeviceSubgroupProperties() const;
		/// <summary>
		/// Retrieve the Vulkan physical device id properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceIDProperties.</returns>
		const VkPhysicalDeviceIDProperties& GetVkPhysicalDeviceIDProperties() const;
		/// <summary>
		/// Retrieve the Vulkan physical device memory properties.
		/// </summary>
		/// <returns>VkPhysicalDeviceMemoryProperties.</returns>
		const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties() const;
		/// <summary>
		/// Retrieve the Vulkan physical device features.
		/// </summary>
		/// <returns>VkPhysicalDeviceFeatures.</returns>
		const VkPhysicalDeviceFeatures& GetVkPhysicalDeviceFeatures() const;
		/// <summary>
		/// Retrieve the Vulkan physical device fragment shader interlock features.
		/// </summary>
		/// <returns>VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT.</returns>
		const VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT& GetVkPhysicalDeviceFragmentShaderInterlockFeatures() const;
		/// <summary>
		/// Retrieve the Vulkan queue family properties of each queue family.
		/// </summary>
		/// <returns>List of VkQueueFamilyProperties.</returns>
		const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const;
		/// <summary>
		/// Check whether an extension is supported by the physical device or not.
		/// </summary>
		/// <param name="extension">Extension to check.</param>
		/// <returns>True if extension is supported, false otherwise.</returns>
		bool IsExtensionSupported(const std::string_view extension);

		/// <summary>
		/// Retrieve a list of all available physical device extensions.
		/// </summary>
		/// <returns>List of physical device extensions.</returns>
		const std::vector<VkExtensionProperties>& GetAvailablePhysicalDeviceExtensions();

		/// <summary>
		/// Retrieve the physical device's UUID.
		/// </summary>
		/// <returns>Physical device UUID.</returns>
		const std::array<uint8_t, 16>& GetPhysicalDeviceUUID() const;

		/// <summary>
		/// Retrieve the physical device fragment shader interlock features.
		/// Value is saved in RendererAPI::GPUSettings.ROVsSupported.
		/// </summary>
		void RetrievePhysicalDeviceFragmentShaderInterlockFeatures();

		/// <summary>
		/// Retrieve the vendor of the physical device.
		/// </summary>
		/// <returns>Vendor.</returns>
		RendererAPI::GPUVendor GetVendor() const;

		/// <summary>
		/// Retrieve a list of all rated physical devices.
		/// Key is the devices score, value is the UUID of the physical device.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <returns>List of rated physical devices.</returns>
		static const std::multimap<uint32_t, std::array<uint8_t, 16>>& GetAllRatedPhysicalDevices(const TRAP::Ref<VulkanInstance>& instance);

		/// <summary>
		/// Retrieve a phyiscal device via its UUID.
		/// </summary>
		/// <param name="instance">Vulkan instance.</param>
		/// <param name="physicalDeviceUUID">Physical device UUID to retrieve.</param>
		/// <returns>Vulkan physical device.</returns>
		static VkPhysicalDevice FindPhysicalDeviceViaUUID(const TRAP::Ref<VulkanInstance>& instance,
			                                              const std::array<uint8_t, 16>& physicalDeviceUUID);

	private:
		friend bool TRAP::Graphics::RendererAPI::IsVulkanCapable();
		/// <summary>
		/// Retrieve a list of all rated physical devices.
		/// Key is the devices score, value is the UUID of the physical device.
		/// </summary>
		/// <param name="instance">Vulkan instance handle.</param>
		/// <returns>List of rated physical devices.</returns>
		static const std::multimap<uint32_t, std::array<uint8_t, 16>>& GetAllRatedPhysicalDevices(const VkInstance& instance);

		/// <summary>
		/// Retrieve a list of all Vulkan physical device handles.
		/// </summary>
		/// <param name="instance">Vulkan instance handle.</param>
		/// <returns>List of Vulkan physical device handles.</returns>
		static std::vector<VkPhysicalDevice> GetAllVkPhysicalDevices(const VkInstance& instance);

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
		uint32_t GetMaxUsableMSAASampleCount() const;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;
		VkPhysicalDeviceSubgroupProperties m_physicalDeviceSubgroupProperties;
		VkPhysicalDeviceIDProperties m_physicalDeviceIDProperties;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
		VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT m_physicalDeviceFragmentShaderInterlockFeatures;
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;

		std::array<uint8_t, 16> m_deviceUUID;

		std::vector<VkExtensionProperties> m_availablePhysicalDeviceExtensions;

		static std::multimap<uint32_t, std::array<uint8_t, 16>> s_availablePhysicalDeviceUUIDs;
	};
}

#endif /*TRAP_VULKANPHYSICALDEVICE_H*/
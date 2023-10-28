#ifndef TRAP_VULKANDEVICE_H
#define TRAP_VULKANDEVICE_H

#include "Core/Base.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;
	class VulkanPhysicalDevice;
	class VulkanQueue;

	class VulkanDevice
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="physicalDevice">Vulkan physical device.</param>
		/// <param name="deviceExtensions">Device extensions to use.</param>
		VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
		             std::vector<std::string> deviceExtensions);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanDevice();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr VulkanDevice(const VulkanDevice&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr VulkanDevice& operator=(const VulkanDevice&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDevice(VulkanDevice&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDevice& operator=(VulkanDevice&&) noexcept = default;

		/// <summary>
		/// Retrieve the VkDevice.
		/// </summary>
		/// <returns>VkDevice.</returns>
		[[nodiscard]] constexpr VkDevice GetVkDevice() const noexcept;

		/// <summary>
		/// Retrieve the Vulkan physical device.
		/// </summary>
		/// <returns>Vulkan physical device.</returns>
		[[nodiscard]] VulkanPhysicalDevice* GetPhysicalDevice() const noexcept;

		/// <summary>
		/// Retrieve a list of all used physical device extensions.
		/// </summary>
		/// <returns>List of physical device extensions.</returns>
		[[nodiscard]] constexpr const std::vector<std::string>& GetUsedPhysicalDeviceExtensions() const noexcept;

		/// <summary>
		/// Find a queue family index for each queue type.
		/// </summary>
		void FindQueueFamilyIndices();

		/// <summary>
		/// Wait for the completion of outstanding queue operations for all queues on the device.
		/// </summary>
		void WaitIdle() const;

		/// <summary>
		/// Retrieve the graphics queue family index.
		/// </summary>
		/// <returns>Graphics queue family index.</returns>
		[[nodiscard]] constexpr u8 GetGraphicsQueueFamilyIndex() const noexcept;
		/// <summary>
		/// Retrieve the transfer queue family index.
		/// </summary>
		/// <returns>Transfer queue family index.</returns>
		[[nodiscard]] constexpr u8 GetTransferQueueFamilyIndex() const noexcept;
		/// <summary>
		/// Retrieve the compute queue family index.
		/// </summary>
		/// <returns>Compute queue family index.</returns>
		[[nodiscard]] constexpr u8 GetComputeQueueFamilyIndex() const noexcept;
		/// <summary>
		/// Retrieve the all queue family (graphics, transfer, compute) indices.
		/// </summary>
		/// <returns>All queue family indices.</returns>
		[[nodiscard]] constexpr std::array<u8, 3> GetQueueFamilyIndices() const noexcept;

		/// <summary>
		/// Retrieve the graphics queue index.
		/// </summary>
		/// <returns>Graphics queue index.</returns>
		[[nodiscard]] constexpr u8 GetGraphicsQueueIndex() const noexcept;
		/// <summary>
		/// Retrieve the transfer queue index.
		/// </summary>
		/// <returns>Transfer queue index.</returns>
		[[nodiscard]] constexpr u8 GetTransferQueueIndex() const noexcept;
		/// <summary>
		/// Retrieve the compute queue index.
		/// </summary>
		/// <returns>Compute queue index.</returns>
		[[nodiscard]] constexpr u8 GetComputeQueueIndex() const noexcept;

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		/// <summary>
		/// Retrieve the NVIDIA Reflex semaphore.
		/// </summary>
		/// <returns>Semaphore.</returns>
		[[nodiscard]] constexpr VkSemaphore& GetReflexSemaphore() noexcept;
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	private:
		friend VulkanQueue;

		/// <summary>
		/// Find the queue family index for a given queue type.
		/// </summary>
		/// <param name="queueType">Queue type to search for.</param>
		/// <param name="queueFamilyIndex">Output queue family index.</param>
		/// <param name="queueIndex">Output queue index.</param>
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, u8& queueFamilyIndex, u8& queueIndex);
		/// <summary>
		/// Find the queue family index for a given queue type from the given queue family properties.
		/// </summary>
		/// <param name="queueType">Queue type to search for.</param>
		/// <param name="queueFamilyProperties">Queue family properties to search in.</param>
		/// <param name="queueFamilyIndex">Output queue family index.</param>
		/// <param name="queueIndex">Output queue index.</param>
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, VkQueueFamilyProperties& queueFamilyProperties,
		                          u8& queueFamilyIndex, u8& queueIndex);

		/// <summary>
		/// Set a name for the device device.
		/// </summary>
		/// <param name="name">Name for the device.</param>
		void SetDeviceName(std::string_view name) const;

		/// <summary>
		/// Load the variable shading rate capabilities from the physical device.
		/// </summary>
		void LoadShadingRateCaps(const VkPhysicalDeviceFragmentShadingRateFeaturesKHR& shadingRateFeatures) const;

		TRAP::Scope<VulkanPhysicalDevice> m_physicalDevice;

		std::vector<std::string> m_deviceExtensions;

		std::unordered_map<VkQueueFlags, u32> m_availableQueueCount;
		std::unordered_map<VkQueueFlags, u32> m_usedQueueCount;

		u8 m_graphicsQueueFamilyIndex = 0;
		u8 m_transferQueueFamilyIndex = 0;
		u8 m_computeQueueFamilyIndex = 0;

		u8 m_graphicsQueueIndex = 0;
		u8 m_transferQueueIndex = 0;
		u8 m_computeQueueIndex = 0;

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		VkSemaphore m_reflexSemaphore;
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

		VkDevice m_device = VK_NULL_HANDLE;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkDevice TRAP::Graphics::API::VulkanDevice::GetVkDevice() const noexcept
{
	return m_device;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr const std::vector<std::string>& TRAP::Graphics::API::VulkanDevice::GetUsedPhysicalDeviceExtensions() const noexcept
{
	return m_deviceExtensions;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDevice::GetGraphicsQueueFamilyIndex() const noexcept
{
	return m_graphicsQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDevice::GetTransferQueueFamilyIndex() const noexcept
{
	return m_transferQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDevice::GetComputeQueueFamilyIndex() const noexcept
{
	return m_computeQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr std::array<u8, 3> TRAP::Graphics::API::VulkanDevice::GetQueueFamilyIndices() const noexcept
{
	return { m_graphicsQueueFamilyIndex, m_transferQueueFamilyIndex, m_computeQueueFamilyIndex };
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDevice::GetGraphicsQueueIndex() const noexcept
{
	return m_graphicsQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDevice::GetTransferQueueIndex() const noexcept
{
	return m_transferQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanDevice::GetComputeQueueIndex() const noexcept
{
	return m_computeQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
[[nodiscard]] constexpr VkSemaphore& TRAP::Graphics::API::VulkanDevice::GetReflexSemaphore() noexcept
{
	return m_reflexSemaphore;
}
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

#endif /*TRAP_VULKANDEVICE_H*/
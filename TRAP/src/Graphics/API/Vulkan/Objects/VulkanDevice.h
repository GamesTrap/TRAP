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
		/// @brief Constructor.
		/// @param physicalDevice Vulkan physical device.
		/// @param deviceExtensions Device extensions to use.
		VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
		             std::vector<std::string> deviceExtensions);
		/// @brief Destructor.
		~VulkanDevice();

		/// @brief Copy constructor.
		constexpr VulkanDevice(const VulkanDevice&) = delete;
		/// @brief Copy assignment operator.
		constexpr VulkanDevice& operator=(const VulkanDevice&) = delete;
		/// @brief Move constructor.
		VulkanDevice(VulkanDevice&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanDevice& operator=(VulkanDevice&&) noexcept = default;

		/// @brief Retrieve the VkDevice.
		/// @return VkDevice.
		[[nodiscard]] constexpr VkDevice GetVkDevice() const noexcept;

		/// @brief Retrieve the Vulkan physical device.
		/// @return Vulkan physical device.
		[[nodiscard]] VulkanPhysicalDevice* GetPhysicalDevice() const noexcept;

		/// @brief Retrieve a list of all used physical device extensions.
		/// @return List of physical device extensions.
		[[nodiscard]] constexpr const std::vector<std::string>& GetUsedPhysicalDeviceExtensions() const noexcept;

		/// @brief Find a queue family index for each queue type.
		void FindQueueFamilyIndices();

		/// @brief Wait for the completion of outstanding queue operations for all queues on the device.
		void WaitIdle() const;

		/// @brief Retrieve the graphics queue family index.
		/// @return Graphics queue family index.
		[[nodiscard]] constexpr u8 GetGraphicsQueueFamilyIndex() const noexcept;
		/// @brief Retrieve the transfer queue family index.
		/// @return Transfer queue family index.
		[[nodiscard]] constexpr u8 GetTransferQueueFamilyIndex() const noexcept;
		/// @brief Retrieve the compute queue family index.
		/// @return Compute queue family index.
		[[nodiscard]] constexpr u8 GetComputeQueueFamilyIndex() const noexcept;
		/// @brief Retrieve the all queue family (graphics, transfer, compute) indices.
		/// @return All queue family indices.
		[[nodiscard]] constexpr std::array<u8, 3> GetQueueFamilyIndices() const noexcept;

		/// @brief Retrieve the graphics queue index.
		/// @return Graphics queue index.
		[[nodiscard]] constexpr u8 GetGraphicsQueueIndex() const noexcept;
		/// @brief Retrieve the transfer queue index.
		/// @return Transfer queue index.
		[[nodiscard]] constexpr u8 GetTransferQueueIndex() const noexcept;
		/// @brief Retrieve the compute queue index.
		/// @return Compute queue index.
		[[nodiscard]] constexpr u8 GetComputeQueueIndex() const noexcept;

#if defined(NVIDIA_REFLEX_AVAILABLE) && !defined(TRAP_HEADLESS_MODE)
		/// @brief Retrieve the NVIDIA Reflex semaphore.
		/// @return Semaphore.
		/// @remark @headless This function is not available in headless mode.
		/// @remark This function is only available when NVIDIA Reflex SDK is provided.
		[[nodiscard]] constexpr VkSemaphore& GetReflexSemaphore() noexcept;
#endif /*NVIDIA_REFLEX_AVAILABLE && !TRAP_HEADLESS_MODE*/

	private:
		friend VulkanQueue;

		/// @brief Find the queue family index for a given queue type.
		/// @param queueType Queue type to search for.
		/// @param queueFamilyIndex Output queue family index.
		/// @param queueIndex Output queue index.
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, u8& queueFamilyIndex, u8& queueIndex);
		/// @brief Find the queue family index for a given queue type from the given queue family properties.
		/// @param queueType Queue type to search for.
		/// @param queueFamilyProperties Queue family properties to search in.
		/// @param queueFamilyIndex Output queue family index.
		/// @param queueIndex Output queue index.
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, VkQueueFamilyProperties& queueFamilyProperties,
		                          u8& queueFamilyIndex, u8& queueIndex);

		/// @brief Set a name for the device device.
		/// @param name Name for the device.
		void SetDeviceName(std::string_view name) const;

		/// @brief Load the variable shading rate capabilities from the physical device.
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
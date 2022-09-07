#ifndef TRAP_VULKANDEVICE_H
#define TRAP_VULKANDEVICE_H

#include "VulkanQueue.h"
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanInstance;
	class VulkanPhysicalDevice;

	class VulkanDevice
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="physicalDevice">Vulkan physical device.</param>
		/// <param name="deviceExtensions">Device extensions to use.</param>
		/// <param name="requestAllAvailableQueues">
		/// Request all available queues.
		/// Default: false (only 1 queue for each type).
		/// </param>
		explicit VulkanDevice(TRAP::Scope<VulkanPhysicalDevice> physicalDevice,
		                      std::vector<std::string> deviceExtensions,
		                      bool requestAllAvailableQueues = false);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanDevice();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanDevice(const VulkanDevice&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanDevice& operator=(const VulkanDevice&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanDevice(VulkanDevice&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanDevice& operator=(VulkanDevice&&) = default;

		/// <summary>
		/// Retrieve the VkDevice.
		/// </summary>
		/// <returns>VkDevice.</returns>
		VkDevice GetVkDevice() const;

		/// <summary>
		/// Retrieve the Vulkan physical device.
		/// </summary>
		/// <returns>Vulkan physical device.</returns>
		VulkanPhysicalDevice* GetPhysicalDevice() const;

		/// <summary>
		/// Retrieve a list of all used physical device extensions.
		/// </summary>
		/// <returns>List of physical device extensions.</returns>
		const std::vector<std::string>& GetUsedPhysicalDeviceExtensions() const;

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
		uint8_t GetGraphicsQueueFamilyIndex() const;
		/// <summary>
		/// Retrieve the transfer queue family index.
		/// </summary>
		/// <returns>Transfer queue family index.</returns>
		uint8_t GetTransferQueueFamilyIndex() const;
		/// <summary>
		/// Retrieve the compute queue family index.
		/// </summary>
		/// <returns>Compute queue family index.</returns>
		uint8_t GetComputeQueueFamilyIndex() const;
		/// <summary>
		/// Retrieve the all queue family (graphics, transfer, compute) indices.
		/// </summary>
		/// <returns>All queue family indices.</returns>
		std::array<uint8_t, 3> GetQueueFamilyIndices() const;

		/// <summary>
		/// Retrieve the graphics queue index.
		/// </summary>
		/// <returns>Graphics queue index.</returns>
		uint8_t GetGraphicsQueueIndex() const;
		/// <summary>
		/// Retrieve the transfer queue index.
		/// </summary>
		/// <returns>Transfer queue index.</returns>
		uint8_t GetTransferQueueIndex() const;
		/// <summary>
		/// Retrieve the compute queue index.
		/// </summary>
		/// <returns>Compute queue index.</returns>
		uint8_t GetComputeQueueIndex() const;

#ifdef NVIDIA_REFLEX_AVAILABLE
		/// <summary>
		/// Retrieve the NVIDIA Reflex semaphore.
		/// </summary>
		/// <returns>Semaphore.</returns>
		VkSemaphore& GetReflexSemaphore();
#endif /*NVIDIA_REFLEX_AVAILABLE*/

	private:
		friend VulkanQueue;

		/// <summary>
		/// Find the queue family index for a given queue type.
		/// </summary>
		/// <param name="queueType">Queue type to search for.</param>
		/// <param name="queueFamilyIndex">Output queue family index.</param>
		/// <param name="queueIndex">Output queue index.</param>
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, uint8_t& queueFamilyIndex, uint8_t& queueIndex);
		/// <summary>
		/// Find the queue family index for a given queue type from the given queue family properties.
		/// </summary>
		/// <param name="queueType">Queue type to search for.</param>
		/// <param name="queueFamilyProperties">Queue family properties to search in.</param>
		/// <param name="queueFamilyIndex">Output queue family index.</param>
		/// <param name="queueIndex">Output queue index.</param>
		void FindQueueFamilyIndex(RendererAPI::QueueType queueType, VkQueueFamilyProperties& queueFamilyProperties,
		                          uint8_t& queueFamilyIndex, uint8_t& queueIndex);

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

		std::unordered_map<VkQueueFlags, uint32_t> m_availableQueueCount;
		std::unordered_map<VkQueueFlags, uint32_t> m_usedQueueCount;

		uint8_t m_graphicsQueueFamilyIndex;
		uint8_t m_transferQueueFamilyIndex;
		uint8_t m_computeQueueFamilyIndex;

		uint8_t m_graphicsQueueIndex;
		uint8_t m_transferQueueIndex;
		uint8_t m_computeQueueIndex;

#ifdef NVIDIA_REFLEX_AVAILABLE
		VkSemaphore m_reflexSemaphore;
#endif /*NVIDIA_REFLEX_AVAILABLE*/

		VkDevice m_device;
	};
}

#endif /*TRAP_VULKANDEVICE_H*/
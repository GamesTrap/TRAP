#ifndef TRAP_VULKANQUEUE_H
#define TRAP_VULKANQUEUE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Queue.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanQueue final : public Queue
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Queue description.</param>
		explicit VulkanQueue(const RendererAPI::QueueDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanQueue() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanQueue(const VulkanQueue&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanQueue& operator=(const VulkanQueue&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanQueue(VulkanQueue&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanQueue& operator=(VulkanQueue&&) = delete;

		/// <summary>
		/// Retrieve the Vulkan queue handle.
		/// </summary>
		/// <returns>Vulkan queue handle.</returns>
		VkQueue GetVkQueue() const;
		/// <summary>
		/// Retrieve the queue family index.
		/// </summary>
		/// <returns>Queue family index.</returns>
		uint8_t GetQueueFamilyIndex() const;
		/// <summary>
		/// Retrieve the queue index.
		/// </summary>
		/// <returns>Queue index.</returns>
		uint8_t GetQueueIndex() const;
		/// <summary>
		/// Retrieve the queue type.
		/// </summary>
		/// <returns>Queue type.</returns>
		RendererAPI::QueueType GetQueueType() const;
		/// <summary>
		/// Retrieve the queue flags.
		/// Indicates capabilities of the queue.
		/// </summary>
		/// <returns>Queue flags.</returns>
		uint32_t GetFlags() const;
		/// <summary>
		/// Retrieve the number of nanoseconds required
		/// for a timestamp to be incremented by 1.
		/// </summary>
		/// <returns>Nanoseconds per timestamp increment.</returns>
		float GetTimestampPeriod() const;
		/// <summary>
		/// Retrieve the number of ticks per second
		/// required to increment a timestamp by 1.
		/// </summary>
		/// <returns>Ticks per second.</returns>
		double GetTimestampFrequency() const;

		/// <summary>
		/// Wait for the queue to finish all submitted commands.
		/// </summary>
		void WaitQueueIdle() const override;

		/// <summary>
		/// Submit work to the queue.
		/// </summary>
		/// <param name="desc">Queue submit description.</param>
		void Submit(const RendererAPI::QueueSubmitDesc& desc) const override;

		/// <summary>
		/// Queue an image for presentation.
		/// </summary>
		/// <param name="desc">Queue presentation description.</param>
		/// <returns>Presentation status.</returns>
		RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const override;

	private:
		TRAP::Ref<VulkanDevice> m_device;

		VkQueue m_vkQueue;
		std::mutex& m_submitMutex;
		uint8_t m_vkQueueFamilyIndex;
		uint8_t m_vkQueueIndex;
		RendererAPI::QueueType m_type;
		uint32_t m_flags;
		float m_timestampPeriod;
	};
}

#endif /*TRAP_VULKANQUEUE_H*/
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
		VulkanQueue(const VulkanQueue&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanQueue& operator=(const VulkanQueue&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanQueue(VulkanQueue&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanQueue& operator=(VulkanQueue&&) = delete;

		/// <summary>
		/// Retrieve the Vulkan queue handle.
		/// </summary>
		/// <returns>Vulkan queue handle.</returns>
		VkQueue GetVkQueue() const noexcept;
		/// <summary>
		/// Retrieve the queue family index.
		/// </summary>
		/// <returns>Queue family index.</returns>
		uint8_t GetQueueFamilyIndex() const noexcept;
		/// <summary>
		/// Retrieve the queue index.
		/// </summary>
		/// <returns>Queue index.</returns>
		uint8_t GetQueueIndex() const noexcept;
		/// <summary>
		/// Retrieve the queue type.
		/// </summary>
		/// <returns>Queue type.</returns>
		RendererAPI::QueueType GetQueueType() const noexcept;
		/// <summary>
		/// Retrieve the queue flags.
		/// Indicates capabilities of the queue.
		/// </summary>
		/// <returns>Queue flags.</returns>
		uint32_t GetFlags() const noexcept;
		/// <summary>
		/// Retrieve the number of nanoseconds required
		/// for a timestamp to be incremented by 1.
		/// </summary>
		/// <returns>Nanoseconds per timestamp increment.</returns>
		float GetTimestampPeriod() const noexcept;
		/// <summary>
		/// Retrieve the number of ticks per second
		/// required to increment a timestamp by 1.
		/// </summary>
		/// <returns>Ticks per second.</returns>
		double GetTimestampFrequency() const noexcept;

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
		/// <summary>
		/// Set a name for the queue.
		/// </summary>
		/// <param name="name">Name for the queue.</param>
		void SetQueueName(std::string_view name) const;

		TRAP::Ref<VulkanDevice> m_device;

		VkQueue m_vkQueue;
#ifdef TRACY_ENABLE
		tracy::Lockable<std::mutex>& m_submitMutex;
#else
		std::mutex& m_submitMutex;
#endif
		uint8_t m_vkQueueFamilyIndex;
		uint8_t m_vkQueueIndex;
		RendererAPI::QueueType m_type;
		uint32_t m_flags;
		float m_timestampPeriod;
	};
}

#endif /*TRAP_VULKANQUEUE_H*/
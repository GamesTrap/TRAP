#ifndef TRAP_VULKANQUEUE_H
#define TRAP_VULKANQUEUE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Queue.h"
#include "Graphics/API/Vulkan/Objects/VulkanDevice.h"
#include "Graphics/API/Vulkan/Objects/VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

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
		constexpr VulkanQueue& operator=(const VulkanQueue&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanQueue(VulkanQueue&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr VulkanQueue& operator=(VulkanQueue&&) = delete;

		/// <summary>
		/// Retrieve the Vulkan queue handle.
		/// </summary>
		/// <returns>Vulkan queue handle.</returns>
		[[nodiscard]] constexpr VkQueue GetVkQueue() const noexcept;
		/// <summary>
		/// Retrieve the queue family index.
		/// </summary>
		/// <returns>Queue family index.</returns>
		[[nodiscard]] constexpr uint8_t GetQueueFamilyIndex() const noexcept;
		/// <summary>
		/// Retrieve the queue index.
		/// </summary>
		/// <returns>Queue index.</returns>
		[[nodiscard]] constexpr uint8_t GetQueueIndex() const noexcept;
		/// <summary>
		/// Retrieve the queue flags.
		/// Indicates capabilities of the queue.
		/// </summary>
		/// <returns>Queue flags.</returns>
		[[nodiscard]] constexpr uint32_t GetFlags() const noexcept;
		/// <summary>
		/// Retrieve the number of nanoseconds required
		/// for a timestamp to be incremented by 1.
		/// </summary>
		/// <returns>Nanoseconds per timestamp increment.</returns>
		[[nodiscard]] constexpr float GetTimestampPeriod() const noexcept;
		/// <summary>
		/// Retrieve the number of ticks per second
		/// required to increment a timestamp by 1.
		/// </summary>
		/// <returns>Ticks per second.</returns>
		[[nodiscard]] constexpr double GetTimestampFrequency() const noexcept;

		/// <summary>
		/// Wait for the queue to finish all submitted commands.
		/// </summary>
		void WaitQueueIdle() const override;

		/// <summary>
		/// Submit work to the queue.
		/// </summary>
		/// <param name="desc">Queue submit description.</param>
		void Submit(const RendererAPI::QueueSubmitDesc& desc) const override;

#ifndef TRAP_HEADLESS_MODE
		/// <summary>
		/// Queue an image for presentation.
		/// </summary>
		/// <param name="desc">Queue presentation description.</param>
		/// <returns>Presentation status.</returns>
		[[nodiscard]] RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const override;
#endif /*TRAP_HEADLESS_MODE*/

	private:
		/// <summary>
		/// Set a name for the queue.
		/// </summary>
		/// <param name="name">Name for the queue.</param>
		void SetQueueName(std::string_view name) const;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		VkQueue m_vkQueue = VK_NULL_HANDLE;
#ifdef TRACY_ENABLE
		tracy::Lockable<std::mutex>& m_submitMutex = VulkanRenderer::s_NullDescriptors->SubmitMutex;
#else
		std::mutex& m_submitMutex = VulkanRenderer::s_NullDescriptors->SubmitMutex;
#endif
		uint8_t m_vkQueueFamilyIndex = std::numeric_limits<uint8_t>::max();
		uint8_t m_vkQueueIndex = std::numeric_limits<uint8_t>::max();
		uint32_t m_flags = 0;
		float m_timestampPeriod = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.timestampPeriod;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueue TRAP::Graphics::API::VulkanQueue::GetVkQueue() const noexcept
{
	return m_vkQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint8_t TRAP::Graphics::API::VulkanQueue::GetQueueFamilyIndex() const noexcept
{
	return m_vkQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint8_t TRAP::Graphics::API::VulkanQueue::GetQueueIndex() const noexcept
{
	return m_vkQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::API::VulkanQueue::GetFlags() const noexcept
{
	return m_flags;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::API::VulkanQueue::GetTimestampPeriod() const noexcept
{
	return m_timestampPeriod;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr double TRAP::Graphics::API::VulkanQueue::GetTimestampFrequency() const noexcept
{
	//The engine is using ticks per sec as frequency.
	//Vulkan is nano sec per tick.
	//Handle the conversion logic here.

	return 1.0 / (NumericCast<double>(m_timestampPeriod) * 1e-9);
}

#endif /*TRAP_VULKANQUEUE_H*/
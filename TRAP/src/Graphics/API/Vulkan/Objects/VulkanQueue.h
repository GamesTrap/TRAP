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
		[[nodiscard]] constexpr u8 GetQueueFamilyIndex() const noexcept;
		/// <summary>
		/// Retrieve the queue index.
		/// </summary>
		/// <returns>Queue index.</returns>
		[[nodiscard]] constexpr u8 GetQueueIndex() const noexcept;
		/// <summary>
		/// Retrieve the queue flags.
		/// Indicates capabilities of the queue.
		/// </summary>
		/// <returns>Queue flags.</returns>
		[[nodiscard]] constexpr u32 GetFlags() const noexcept;
		/// <summary>
		/// Retrieve the number of nanoseconds required
		/// for a timestamp to be incremented by 1.
		/// </summary>
		/// <returns>Nanoseconds per timestamp increment.</returns>
		[[nodiscard]] constexpr f32 GetTimestampPeriod() const noexcept;
		/// <summary>
		/// Retrieve the number of ticks per second
		/// required to increment a timestamp by 1.
		/// </summary>
		/// <returns>Ticks per second.</returns>
		[[nodiscard]] constexpr f64 GetTimestampFrequency() const noexcept;

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
		u8 m_vkQueueFamilyIndex = std::numeric_limits<u8>::max();
		u8 m_vkQueueIndex = std::numeric_limits<u8>::max();
		u32 m_flags = 0;
		f32 m_timestampPeriod = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.timestampPeriod;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkQueue TRAP::Graphics::API::VulkanQueue::GetVkQueue() const noexcept
{
	return m_vkQueue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanQueue::GetQueueFamilyIndex() const noexcept
{
	return m_vkQueueFamilyIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u8 TRAP::Graphics::API::VulkanQueue::GetQueueIndex() const noexcept
{
	return m_vkQueueIndex;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr u32 TRAP::Graphics::API::VulkanQueue::GetFlags() const noexcept
{
	return m_flags;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::API::VulkanQueue::GetTimestampPeriod() const noexcept
{
	return m_timestampPeriod;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f64 TRAP::Graphics::API::VulkanQueue::GetTimestampFrequency() const noexcept
{
	//The engine is using ticks per sec as frequency.
	//Vulkan is nano sec per tick.
	//Handle the conversion logic here.

	return 1.0 / (NumericCast<f64>(m_timestampPeriod) * 1e-9);
}

#endif /*TRAP_VULKANQUEUE_H*/
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
		/// @brief Constructor.
		/// @param desc Queue description.
		explicit VulkanQueue(const RendererAPI::QueueDesc& desc);
		/// @brief Destructor.
		~VulkanQueue() override;

		/// @brief Copy constructor.
		consteval VulkanQueue(const VulkanQueue&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanQueue& operator=(const VulkanQueue&) = delete;
		/// @brief Move constructor.
		VulkanQueue(VulkanQueue&&) noexcept = default;
		/// @brief Move assignment operator.
		consteval VulkanQueue& operator=(VulkanQueue&&) = delete;

		/// @brief Retrieve the Vulkan queue handle.
		/// @return Vulkan queue handle.
		[[nodiscard]] constexpr VkQueue GetVkQueue() const noexcept;
		/// @brief Retrieve the queue family index.
		/// @return Queue family index.
		[[nodiscard]] constexpr u8 GetQueueFamilyIndex() const noexcept;
		/// @brief Retrieve the queue index.
		/// @return Queue index.
		[[nodiscard]] constexpr u8 GetQueueIndex() const noexcept;
		/// @brief Retrieve the queue flags.
		/// Indicates capabilities of the queue.
		/// @return Queue flags.
		[[nodiscard]] constexpr u32 GetFlags() const noexcept;
		/// @brief Retrieve the number of nanoseconds required
		/// for a timestamp to be incremented by 1.
		/// @return Nanoseconds per timestamp increment.
		[[nodiscard]] constexpr f32 GetTimestampPeriod() const noexcept;
		/// @brief Retrieve the number of ticks per second
		/// required to increment a timestamp by 1.
		/// @return Ticks per second.
		[[nodiscard]] constexpr f64 GetTimestampFrequency() const noexcept;

		/// @brief Wait for the queue to finish all submitted commands.
		void WaitQueueIdle() const override;

		/// @brief Submit work to the queue.
		/// @param desc Queue submit description.
		void Submit(const RendererAPI::QueueSubmitDesc& desc) const override;

#ifndef TRAP_HEADLESS_MODE
		/// @brief Queue an image for presentation.
		/// @param desc Queue presentation description.
		/// @return Presentation status.
		/// @remark @headless This function is not available in headless mode.
		[[nodiscard]] RendererAPI::PresentStatus Present(const RendererAPI::QueuePresentDesc& desc) const override;
#endif /*TRAP_HEADLESS_MODE*/

	private:
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		VkQueue m_vkQueue = VK_NULL_HANDLE;
		std::reference_wrapper<decltype(VulkanRenderer::s_NullDescriptors->SubmitMutex)> m_submitMutex = VulkanRenderer::s_NullDescriptors->SubmitMutex;
		u8 m_vkQueueFamilyIndex = std::numeric_limits<u8>::max();
		u8 m_vkQueueIndex = std::numeric_limits<u8>::max();
		u32 m_flags = 0;
		f32 m_timestampPeriod = m_device->GetPhysicalDevice().GetVkPhysicalDeviceProperties().limits.timestampPeriod;
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

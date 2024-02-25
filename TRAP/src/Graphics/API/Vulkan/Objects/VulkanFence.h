#ifndef TRAP_VULKANFENCE_H
#define TRAP_VULKANFENCE_H

#include "VulkanSwapChain.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Fence.h"

namespace TRAP::Graphics::API
{
	class VulkanQueue;
	class VulkanDevice;

	class VulkanFence final : public Fence
	{
	public:
		/// @brief Constructor.
		/// Fence is not signaled.
		/// @param signalled Whether the Fence should be in signalled state or not. Default: Not signalled.
		/// @param name Optional: Name for the Fence.
		explicit VulkanFence(bool signalled = false, [[maybe_unused]] std::string_view name = "");
		/// @brief Destructor.
		~VulkanFence() override;

		/// @brief Copy constructor.
		consteval VulkanFence(const VulkanFence&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanFence& operator=(const VulkanFence&) noexcept = delete;
		/// @brief Move constructor.
		VulkanFence(VulkanFence&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanFence& operator=(VulkanFence&&) noexcept = default;

		/// @brief Retrieve the VkFence handle.
		/// @return VkFence handle.
		[[nodiscard]] constexpr VkFence GetVkFence() const noexcept;

		/// @brief Retrieve the current status of the fence.
		/// @return Fence status.
		[[nodiscard]] RendererAPI::FenceStatus GetStatus() override;

		/// @brief Wait for the fence to be signaled.
		///
		/// 1. Waits for the fence to be signaled.
		/// 2. Resets the fence.
		void Wait() override;

	private:
#ifndef TRAP_HEADLESS_MODE
		friend std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(const TRAP::Ref<Semaphore>& signalSemaphore,
		                                                                                      const TRAP::Ref<Fence>& fence) const;
#endif /*TRAP_HEADLESS_MODE*/

		VkFence m_fence = VK_NULL_HANDLE;
		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkFence TRAP::Graphics::API::VulkanFence::GetVkFence() const noexcept
{
	return m_fence;
}

#endif /*TRAP_VULKANFENCE_H*/

#ifndef TRAP_VULKANFENCE_H
#define TRAP_VULKANFENCE_H

#include "VulkanSwapChain.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Fence.h"

#ifndef TRAP_HEADLESS_MODE
struct ImGuiViewport;
namespace ImGui::INTERNAL::Vulkan
{
	void RenderWindow(ImGuiViewport* viewport, void* render_arg);
}
#endif /*TRAP_HEADLESS_MODE*/

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

		/// @brief Reset the fence to the unsignalled state.
		/// @note If fence is already in unsignalled state then this function has no effect.
		void ResetState() override;

	private:
#ifndef TRAP_HEADLESS_MODE
		friend std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(Semaphore& signalSemaphore,
		                                                                                 Fence& fence) const;
		friend std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(Semaphore& signalSemaphore) const;
		friend std::optional<u32> TRAP::Graphics::API::VulkanSwapChain::AcquireNextImage(Fence& fence) const;
		friend void ImGui::INTERNAL::Vulkan::RenderWindow(ImGuiViewport* viewport, void* render_arg);
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

#ifndef TRAP_FENCE_H
#define TRAP_FENCE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Fence
	{
	public:
		/// @brief Create a new fence. Fence is not signaled.
		/// @return Created fence.
		/// @param signalled Whether the Fence should be in signalled state or not. Default: Not signalled.
		[[nodiscard]] static TRAP::Ref<Fence> Create(bool signalled = false);

		/// @brief Destructor.
		virtual ~Fence();

		/// @brief Copy constructor.
		constexpr Fence(const Fence&) noexcept = default;
		/// @brief Copy assignment operator.
		Fence& operator=(const Fence&) noexcept = default;
		/// @brief Move constructor.
		constexpr Fence(Fence&&) noexcept = default;
		/// @brief Move assignment operator.
		Fence& operator=(Fence&&) noexcept = default;

		/// @brief Retrieve whether the Fence was submitted or not.
		/// @return True if Fence was submitted, false otherwise.
		[[nodiscard]] constexpr bool IsSubmitted() const noexcept;

		/// @brief Retrieve the current status of the fence.
		/// @return Fence status.
		[[nodiscard]] virtual RendererAPI::FenceStatus GetStatus() = 0;
		/// @brief Wait for the fence to be signaled.
		///        1. Waits for the fence to be signaled.
		///        2. Resets the fence.
		virtual void Wait() = 0;

		/// @brief Utility function to wait for multiple fences.
		/// @param fences Fences to wait for.
		static void WaitForFences(std::vector<Fence>& fences);

	protected:
		/// @brief Constructor. Fence is not signaled.
		Fence();

		bool m_submitted = false;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;

#ifndef TRAP_HEADLESS_MODE
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
#endif /*TRAP_HEADLESS_MODE*/
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Fence::IsSubmitted() const noexcept
{
	return m_submitted;
}

#endif /*TRAP_FENCE_H*/
#ifndef TRAP_FENCE_H
#define TRAP_FENCE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Fence
	{
	public:
		/// <summary>
		/// Create a new fence.
		/// Fence is not signaled.
		/// </summary>
		/// <returns>Created fence.</returns>
		[[nodiscard]] static TRAP::Ref<Fence> Create();

		/// <summary>
		/// Destructor.
		/// </summary>
		constexpr virtual ~Fence();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Fence(const Fence&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Fence& operator=(const Fence&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Fence(Fence&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Fence& operator=(Fence&&) noexcept = default;

		/// <summary>
		/// Retrieve whether the Fence was submitted or not.
		/// </summary>
		/// <returns>True if Fence was submitted, false otherwise.</returns>
		[[nodiscard]] bool IsSubmitted() const noexcept;

		/// <summary>
		/// Retrieve the current status of the fence.
		/// </summary>
		/// <returns>Fence status.</returns>
		[[nodiscard]] virtual RendererAPI::FenceStatus GetStatus() = 0;
		/// <summary>
		/// Wait for the fence to be signaled.
		///
		/// 1. Waits for the fence to be signaled.
		/// 2. Resets the fence.
		/// </summary>
		virtual void Wait() = 0;

		/// <summary>
		/// Utility function to wait for multiple fences.
		/// </summary>
		/// <param name="fences">Fences to wait for.</param>
		static void WaitForFences(std::vector<Fence>& fences);

	protected:
		/// <summary>
		/// Constructor.
		/// Fence is not signaled.
		/// </summary>
		Fence();

		bool m_submitted;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;

#ifndef TRAP_HEADLESS_MODE
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
#endif /*TRAP_HEADLESS_MODE*/
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Fence::~Fence()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererFencePrefix, "Destroying Fence");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_FENCE_H*/
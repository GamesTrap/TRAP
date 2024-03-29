#ifndef TRAP_SEMAPHORE_H
#define TRAP_SEMAPHORE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Semaphore
	{
	public:
		/// <summary>
		/// Create a new semaphore.
		/// </summary>
		/// <returns>Created semaphore.</returns>
		[[nodiscard]] static TRAP::Ref<Semaphore> Create();

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Semaphore();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Semaphore(const Semaphore&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Semaphore& operator=(const Semaphore&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Semaphore(Semaphore&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Semaphore& operator=(Semaphore&&) noexcept = default;

		/// <summary>
		/// Is the semaphore signaled?
		/// </summary>
		/// <returns>True if the semaphore is signaled, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsSignaled() const noexcept;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Semaphore();

		bool m_signaled = false;

	private:
		friend void TRAP::Graphics::API::VulkanQueue::Submit(const RendererAPI::QueueSubmitDesc& desc) const;

#ifndef TRAP_HEADLESS_MODE
		friend TRAP::Graphics::RendererAPI::PresentStatus TRAP::Graphics::API::VulkanQueue::Present(const RendererAPI::QueuePresentDesc& desc) const;
#endif /*TRAP_HEADLESS_MODE*/
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Semaphore::IsSignaled() const noexcept
{
	return m_signaled;
}

#endif /*TRAP_SEMAPHORE_H*/
#ifndef TRAP_SEMAPHORE_H
#define TRAP_SEMAPHORE_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanQueue.h"

namespace TRAP::Graphics
{
	class Semaphore
	{
	public:
		/// @brief Create a new semaphore.
		/// @param name Optional: Debug name used in GPU-profile
		/// @return Created semaphore.
		[[nodiscard]] static TRAP::Ref<Semaphore> Create(std::string_view name = "");

		/// @brief Destructor.
		virtual ~Semaphore();

		/// @brief Copy constructor.
		consteval Semaphore(const Semaphore&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval Semaphore& operator=(const Semaphore&) noexcept = delete;
		/// @brief Move constructor.
		constexpr Semaphore(Semaphore&&) noexcept = default;
		/// @brief Move assignment operator.
		Semaphore& operator=(Semaphore&&) noexcept = default;

		/// @brief Is the semaphore signaled?
		/// @return True if the semaphore is signaled, false otherwise.
		[[nodiscard]] constexpr bool IsSignaled() const noexcept;

	protected:
		/// @brief Constructor.
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

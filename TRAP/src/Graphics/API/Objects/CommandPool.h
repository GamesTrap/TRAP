#ifndef TRAP_COMMANDPOOL_H
#define TRAP_COMMANDPOOL_H

#include "Graphics/API/RendererAPI.h"
#include "CommandBuffer.h"

namespace TRAP::Graphics
{
	class CommandPool
	{
	public:
		/// @brief Create a new command pool from the given description.
		/// @param desc Command pool description.
		/// @return Created command pool.
		[[nodiscard]] static TRAP::Ref<CommandPool> Create(const RendererAPI::CommandPoolDesc& desc);

		/// @brief Destructor.
		virtual ~CommandPool();

		/// @brief Copy constructor.
		consteval CommandPool(const CommandPool&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval CommandPool& operator=(const CommandPool&) noexcept = delete;
		/// @brief Move constructor.
		CommandPool(CommandPool&&) noexcept = default;
		/// @brief Move assignment operator.
		CommandPool& operator=(CommandPool&&) noexcept = default;

		/// @brief Retrieve a new command buffer.
		/// @param secondary Should the command buffer be a secondary command buffer.
		/// @return Newly created command buffer.
		/// @note The lifetime of the command buffer ends when the command pool gets
		///       destroyed or when ReleaseCommandBuffer() gets called.
		[[nodiscard]] virtual CommandBuffer& GetCommandBuffer(bool secondary) = 0;
		/// @brief Release a command buffer
		/// @param cmdBuffer Command buffer to release.
		virtual void ReleaseCommandBuffer(const CommandBuffer& cmdBuffer) = 0;

		/// @brief Reset the command pool.
		///        All allocated command buffers are reset to their initial state (i.e. valid non-recording).
		virtual void Reset() const = 0;

	protected:
		/// @brief Constructor.
		explicit CommandPool(TRAP::Ref<TRAP::Graphics::Queue> queue);

		std::vector<TRAP::Scope<CommandBuffer>> m_commandBuffers{};

		TRAP::Ref<Queue> m_queue = nullptr;
	};
}

#endif /*TRAP_COMMANDPOOL_H*/

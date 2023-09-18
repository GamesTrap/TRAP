#ifndef TRAP_COMMANDPOOL_H
#define TRAP_COMMANDPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandBuffer;

	class CommandPool
	{
	public:
		/// <summary>
		/// Create a new command pool from the given description.
		/// </summary>
		/// <param name="desc">Command pool description.</param>
		/// <returns>Created command pool.</returns>
		[[nodiscard]] static TRAP::Ref<CommandPool> Create(const RendererAPI::CommandPoolDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~CommandPool();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		CommandPool(const CommandPool&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CommandPool& operator=(const CommandPool&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CommandPool(CommandPool&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CommandPool& operator=(CommandPool&&) noexcept = default;

		/// <summary>
		/// Allocate a new command buffer.
		/// </summary>
		/// <param name="secondary">Should the command buffer be a secondary command buffer.</param>
		///<returns>New command buffer.</returns>
		[[nodiscard]] virtual CommandBuffer* AllocateCommandBuffer(bool secondary) = 0;
		/// <summary>
		/// Free a command buffer
		/// </summary>
		/// <param name="cmdBuffer">Command buffer to free.</param>
		virtual void FreeCommandBuffer(const CommandBuffer* cmdBuffer) = 0;

		/// <summary>
		/// Reset the command pool.
		/// </summary>
		virtual void Reset() const = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit CommandPool(TRAP::Ref<TRAP::Graphics::Queue> queue);

		std::vector<TRAP::Scope<CommandBuffer>> m_commandBuffers{};

		TRAP::Ref<Queue> m_queue = nullptr;
	};
}

#endif /*TRAP_COMMANDPOOL_H*/
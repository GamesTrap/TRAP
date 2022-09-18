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
		static TRAP::Ref<CommandPool> Create(const RendererAPI::CommandPoolDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~CommandPool();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		CommandPool(const CommandPool&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CommandPool& operator=(const CommandPool&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CommandPool(CommandPool&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CommandPool& operator=(CommandPool&&) = default;

		/// <summary>
		/// Allocate a new command buffer.
		/// </summary>
		/// <param name="secondary">Should the command buffer be a secondary command buffer.</param>
		///<returns>New command buffer.</returns>
		virtual CommandBuffer* AllocateCommandBuffer(bool secondary) = 0;
		/// <summary>
		/// Free a command buffer
		/// </summary>
		/// <param name="cmdBuffer">Command buffer to free.</param>
		virtual void FreeCommandBuffer(const CommandBuffer* const cmdBuffer) = 0;

		/// <summary>
		/// Reset the command pool.
		/// </summary>
		virtual void Reset() const = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		CommandPool();

		std::vector<TRAP::Scope<CommandBuffer>> m_commandBuffers;

		TRAP::Ref<Queue> m_queue;
	};
}

#endif /*TRAP_COMMANDPOOL_H*/
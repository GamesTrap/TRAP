#ifndef TRAP_COMMANDPOOL_H
#define TRAP_COMMANDPOOL_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandBuffer;

	class CommandPool
	{
	public:
		static TRAP::Ref<CommandPool> Create(const RendererAPI::CommandPoolDesc& desc);

		virtual ~CommandPool();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		CommandPool(const CommandPool&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		CommandPool& operator=(const CommandPool&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		CommandPool(CommandPool&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		CommandPool& operator=(CommandPool&&) = default;

		virtual CommandBuffer* AllocateCommandBuffer(bool secondary) = 0;
		virtual void FreeCommandBuffer(CommandBuffer* cmdBuffer) = 0;

		virtual void Reset() const = 0;

	protected:
		CommandPool();

		std::vector<TRAP::Scope<CommandBuffer>> m_commandBuffers;

		TRAP::Ref<Queue> m_queue;
	};
}

#endif /*TRAP_COMMANDPOOL_H*/
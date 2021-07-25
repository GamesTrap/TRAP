#ifndef _TRAP_COMMANDPOOL_H_
#define _TRAP_COMMANDPOOL_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class CommandBuffer;

	class CommandPool
	{
	public:
		static TRAP::Ref<CommandPool> Create(const RendererAPI::CommandPoolDesc& desc);

		virtual ~CommandPool() = default;

		virtual CommandBuffer* AllocateCommandBuffer(bool secondary) = 0;
		virtual void FreeCommandBuffer(CommandBuffer* cmdBuffer) = 0;

		virtual void Reset() const = 0;

	protected:
		CommandPool() = default;

		std::vector<TRAP::Scope<CommandBuffer>> m_commandBuffers;

		TRAP::Ref<Queue> m_queue;
	};
}

#endif /*_TRAP_COMMANDPOOL_H_*/
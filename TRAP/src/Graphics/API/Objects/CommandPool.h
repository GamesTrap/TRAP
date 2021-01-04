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
		
		CommandPool() = default;
		virtual ~CommandPool() = default;

		virtual CommandBuffer* AllocateCommandBuffer(bool secondary) = 0;
		virtual void FreeCommandBuffer(CommandBuffer* cmdBuffer) = 0;
		
		virtual void Reset() const = 0;
	
	protected:
		std::vector<TRAP::Scope<CommandBuffer>> m_commandBuffers;

		TRAP::Ref<API::VulkanQueue> m_queue; //TODO Change to Queue
	};
}

#endif /*_TRAP_COMMANDPOOL_H_*/
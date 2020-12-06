#ifndef _TRAP_VULKANDESCRIPTORSET_H_
#define _TRAP_VULKANDESCRIPTORSET_H_

#include "VulkanDescriptorPool.h"

namespace TRAP::Graphics::API
{
	class VulkanDescriptorSet
	{
	public:
		~VulkanDescriptorSet();
		
	private:
		friend VulkanDescriptorPool;
		
		VulkanDescriptorSet();
	};
}

#endif /*_TRAP_VULKANDESCRIPTORSET_H_*/
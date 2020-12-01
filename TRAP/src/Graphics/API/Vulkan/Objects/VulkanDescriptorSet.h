#ifndef _TRAP_VULKANDESCRIPTORSET_H_
#define _TRAP_VULKANDESCRIPTORSET_H_

#include "VulkanDescriptorPool.h"

namespace TRAP::Graphics::API
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(const TRAP::Ref<VulkanDescriptorPool>& descriptorPool);
		~VulkanDescriptorSet();

		VkDescriptorSet& GetVkDescriptorSet();
		
	private:
		VkDescriptorSet m_descriptorSet;
	};
}

#endif /*_TRAP_VULKANDESCRIPTORSET_H_*/
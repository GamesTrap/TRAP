#ifndef _TRAP_VULKANQUERYPOOL_H_
#define _TRAP_VULKANQUERYPOOL_H_
#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	
	class VulkanQueryPool
	{
	public:
		VulkanQueryPool(TRAP::Ref<VulkanDevice> device, const RendererAPI::QueryPoolDesc& desc);
		~VulkanQueryPool();

		VkQueryPool& GetVkQueryPool();
		VkQueryType GetVkQueryType() const;
		uint32_t GetCount() const;
		
	private:
		VkQueryPool m_vkQueryPool;
		VkQueryType m_type;
		uint32_t m_count;
		
		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANQUERYPOOL_H_*/
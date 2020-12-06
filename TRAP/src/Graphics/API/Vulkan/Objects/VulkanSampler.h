#ifndef _TRAP_VULKANSAMPLER_H_
#define _TRAP_VULKANSAMPLER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;
	
	class VulkanSampler
	{
	public:
		VulkanSampler(TRAP::Ref<VulkanDevice> device, const RendererAPI::SamplerDesc& desc);
		~VulkanSampler();

		VkSampler& GetVkSampler();
		
	private:
		TRAP::Ref<VulkanDevice> m_device;
		
		//Native handle of the underlying resource
		VkSampler m_vkSampler;
	};
}

#endif /*_TRAP_VULKANSAMPLER_H_*/
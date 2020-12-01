#ifndef _TRAP_VULKANRENDERPASS_H_
#define _TRAP_VULKANRENDERPASS_H_

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(TRAP::Ref<VulkanDevice> device);
		~VulkanRenderPass();

		VkRenderPass& GetVkRenderPass();

	private:
		VkRenderPass m_renderPass;

		//TODO RenderPassDesc

		TRAP::Ref<VulkanDevice> m_device;
	};
}

#endif /*_TRAP_VULKANRENDERPASS_H_*/
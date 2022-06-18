#ifndef _TRAP_VULKANRENDERPASS_H_
#define _TRAP_VULKANRENDERPASS_H_

namespace TRAP::Graphics::API::Vulkan
{
	class CommandBuffer;
	class Swapchain;
	class Surface;
	class Device;

	class RenderPass
	{
	public:
		RenderPass(const Scope<Device>& device, Surface& surface);
		~RenderPass();
		RenderPass(const RenderPass&) = default;
		RenderPass& operator=(const RenderPass&) = default;
		RenderPass(RenderPass&&) = default;
		RenderPass& operator=(RenderPass&&) = default;

		VkRenderPass& GetRenderPass();

		void SetClearColor(const VkClearColorValue& color);
		void StartRenderPass(const Scope<CommandBuffer>& graphicsCommandBuffer);
		void StartRenderPass(const Scope<Swapchain>& swapchain, CommandBuffer& graphicsCommandBuffer);
		void EndRenderPass(const Scope<CommandBuffer>& graphicsCommandBuffer);

	private:
		VkRenderPass m_renderPass;
		Device* m_device;
		std::vector<VkClearValue> m_clearValues; //0 = Color
		bool m_recording;
	};
}

#endif /*_TRAP_VULKANRENDERPASS_H_*/
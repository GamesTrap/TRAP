#ifndef _TRAP_COMMANDBUFFER_H_
#define _TRAP_COMMANDBUFFER_H_

namespace TRAP::Graphics::API::Vulkan
{
	class Pipeline;

	class CommandBuffer
	{
	public:
		explicit CommandBuffer(VkCommandBuffer& commandBuffer);

		void BindPipeline(Pipeline& pipeline, VkPipelineBindPoint bindPoint);
		
		void StartRecording();
		void EndRecording();
		bool IsRecording() const;
		void SetViewport(VkViewport& viewport) const;
		void SetScissor(VkRect2D& scissor) const;

		VkCommandBuffer& GetCommandBuffer();
		
	private:
		VkCommandBuffer m_commandBuffer;
		bool m_recording;
	};
}

#endif /*_TRAP_COMMANDBUFFER_H_*/
#ifndef _TRAP_VULKANCOMMANDBUFFER_H_
#define _TRAP_VULKANCOMMANDBUFFER_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace TRAP::Graphics::API
{
	class VulkanPipeline;
	class VulkanCommandSignature;
	class VulkanQueryPool;
	class VulkanDescriptorSet;
	class VulkanCommandPool;
	class VulkanDevice;
	class VulkanQueue;

	class VulkanCommandBuffer
	{
	public:
		~VulkanCommandBuffer();

		VkCommandBuffer& GetVkCommandBuffer();
		RendererAPI::QueueType GetQueueType() const;
		bool IsSecondary() const;

		void BindPushConstants(const TRAP::Ref<VulkanRootSignature>& rootSignature, const char* name, const void* constants) const;
		void BindPushConstantsByIndex(const TRAP::Ref<VulkanRootSignature>& rootSignature, uint32_t paramIndex, const void* constants) const;
		void BindDescriptorSet(uint32_t index, VulkanDescriptorSet& descriptorSet);
		void BindIndexBuffer(const TRAP::Ref<VulkanBuffer>& buffer, RendererAPI::IndexType indexType, uint64_t offset) const;
		void BindVertexBuffer(const std::vector<TRAP::Ref<VulkanBuffer>>& buffers, const std::vector<uint32_t>& strides, const std::vector<uint64_t>& offsets) const;
		void BindPipeline(const TRAP::Ref<VulkanPipeline>& pipeline) const;

		void AddDebugMarker(float r, float g, float b, const char* name) const;
		void BeginDebugMarker(float r, float g, float b, const char* name) const;
		void EndDebugMarker() const;

		void Begin();
		void End();

		void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) const;
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const;

		void Draw(uint32_t vertexCount, uint32_t firstVertex) const;
		void DrawInstances(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) const;
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex) const;
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t firstInstance, uint32_t firstVertex) const;
		void ExecuteIndirect(const TRAP::Ref<VulkanCommandSignature>& cmdSignature,
		                     uint32_t maxCommandCount,
		                     const TRAP::Ref<VulkanBuffer>& indirectBuffer,
		                     uint64_t bufferOffset,
		                     const TRAP::Ref<VulkanBuffer>& counterBuffer,
		                     uint64_t counterBufferOffset) const;

		void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const;

		void UpdateBuffer(const TRAP::Ref<VulkanBuffer>& buffer, uint64_t dstOffset, const TRAP::Ref<VulkanBuffer>& srcBuffer, uint64_t srcOffset, uint64_t size) const;
		void UpdateSubresource(const TRAP::Ref<VulkanTexture>& texture, const TRAP::Ref<VulkanBuffer>& srcBuffer, const VulkanRenderer::SubresourceDesc& subresourceDesc) const;

		void ResetQueryPool(const TRAP::Ref<VulkanQueryPool>& queryPool, uint32_t startQuery, uint32_t queryCount) const;
		void BeginQuery(const TRAP::Ref<VulkanQueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const;
		void EndQuery(const TRAP::Ref<VulkanQueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const;
		void ResolveQuery(const TRAP::Ref<VulkanQueryPool>& queryPool, const TRAP::Ref<VulkanBuffer>& readBackBuffer, uint32_t startQuery, uint32_t queryCount) const;

		void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
		                     const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                     const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const;
		
	private:
		friend VulkanCommandPool;
		
		VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<VulkanQueue> queue, VkCommandPool& commandPool, bool secondary);

		VkCommandBuffer m_vkCommandBuffer;

		TRAP::Ref<VulkanDevice> m_device;
		TRAP::Ref<VulkanQueue> m_queue;
		VkCommandPool& m_vkCommandPool;
		bool m_secondary;
		
		VkRenderPass m_activeRenderPass;
		VkPipelineLayout m_boundPipelineLayout;
	};
}

#endif /*_TRAP_VULKANCOMMANDBUFFER_H_*/
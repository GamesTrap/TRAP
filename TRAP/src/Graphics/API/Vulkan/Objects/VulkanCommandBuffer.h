#ifndef _TRAP_VULKANCOMMANDBUFFER_H_
#define _TRAP_VULKANCOMMANDBUFFER_H_

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandBuffer.h"
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

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		~VulkanCommandBuffer() override;

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VulkanCommandBuffer(const VulkanCommandBuffer&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanCommandBuffer(VulkanCommandBuffer&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) = delete;

		VkCommandBuffer& GetVkCommandBuffer();
		RendererAPI::QueueType GetQueueType() const;
		bool IsSecondary() const;

		void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, const char* name,
		                       const void* constants) const override;
		void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, uint32_t paramIndex,
		                              const void* constants) const override;
		void BindDescriptorSet(uint32_t index, DescriptorSet& descriptorSet) override;
		void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType,
		                     uint64_t offset) const override;
		void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers, const std::vector<uint32_t>& strides,
		                      const std::vector<uint64_t>& offsets) const override;
		void BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const override;
		void BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
		                       const TRAP::Ref<RenderTarget>& depthStencil,
							   const RendererAPI::LoadActionsDesc* loadActions,
							   const std::vector<uint32_t>& colorArraySlices,
		                       const std::vector<uint32_t>& colorMipSlices, uint32_t depthArraySlice,
							   uint32_t depthMipSlice) override;

		void AddDebugMarker(float r, float g, float b, const char* name) const override;
		void BeginDebugMarker(float r, float g, float b, const char* name) const override;
		void EndDebugMarker() const override;

		void Begin() override;
		void End() override;

		void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) const override;
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;

		void Draw(uint32_t vertexCount, uint32_t firstVertex) const override;
		void DrawInstanced(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount,
		                   uint32_t firstInstance) const override;
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex) const override;
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount,
		                          uint32_t firstInstance, uint32_t firstVertex) const override;
		void ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature, uint32_t maxCommandCount,
		                     const TRAP::Ref<Buffer>& indirectBuffer, uint64_t bufferOffset,
		                     const TRAP::Ref<Buffer>& counterBuffer, uint64_t counterBufferOffset) const override;

		void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const override;

		void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, uint64_t dstOffset, const TRAP::Ref<Buffer>& srcBuffer,
		                  uint64_t srcOffset, uint64_t size) const override;
		void UpdateSubresource(const TRAP::Ref<TRAP::Graphics::TextureBase>& texture, const TRAP::Ref<Buffer>& srcBuffer,
		                       const RendererAPI::SubresourceDataDesc& subresourceDesc) const override;

		void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, uint32_t startQuery,
		                    uint32_t queryCount) const override;
		void BeginQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const override;
		void EndQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const override;
		void ResolveQuery(const TRAP::Ref<QueryPool>& queryPool, const TRAP::Ref<Buffer>& readBackBuffer,
		                  uint32_t startQuery, uint32_t queryCount) const override;

		void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
		                     const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
		                     const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const override;

		void SetStencilReferenceValue(uint32_t val) const override;

		//TODO
		/*void SetShadingRate(RendererAPI::ShadingRate shadingRate, TRAP::Ref<TRAP::Graphics::TextureBase> texture,
		                      RendererAPI::ShadingRateCombiner postRasterizerRate,
							  RendererAPI::ShadingRateCombiner finalRate);*/

		void Clear(TRAP::Math::Vec4 color, uint32_t width, uint32_t height) override;
		void Clear(float depth, uint32_t stencil, uint32_t width, uint32_t height) override;
		void Clear(float depth, uint32_t width, uint32_t height) override;
		void Clear(uint32_t stencil, uint32_t width, uint32_t height) override;

		VkRenderPass GetActiveVkRenderPass() const;

	private:
		friend VulkanCommandPool;

		VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<Queue> queue, VkCommandPool& commandPool,
		                    bool secondary);

		template<typename T>
		static std::size_t HashAlg(const T* mem, std::size_t size, const std::size_t prev = 2166136261U)
		{
			//Intentionally uint32_t instead of std::size_t, so the behavior is the same regardless of size.
			uint32_t result = static_cast<uint32_t>(prev);

			while (size--)
				result = (result * 16777619) ^ *mem++;

			return static_cast<std::size_t>(result);
		}

		TRAP::Ref<API::VulkanDevice> m_device;

		VkCommandBuffer m_vkCommandBuffer;

		VkCommandPool& m_vkCommandPool;
		bool m_secondary;

		VkRenderPass m_activeRenderPass;
		VkPipelineLayout m_boundPipelineLayout;
	};
}

#endif /*_TRAP_VULKANCOMMANDBUFFER_H_*/
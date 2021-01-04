#ifndef _TRAP_COMMANDBUFFER_H_
#define _TRAP_COMMANDBUFFER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class QueryPool;
	class Buffer;
	class RenderTarget;
	class RootSignature;

	namespace API
	{
		class VulkanDevice;
		class VulkanCommandSignature;
	}

	class CommandBuffer
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual TRAP::Ref<API::VulkanQueue> GetQueue() const = 0; //TODO

		virtual void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, const char* name, const void* constants) const = 0;
		virtual void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, uint32_t paramIndex, const void* constants) const = 0;
		virtual void BindDescriptorSet(uint32_t index, API::VulkanDescriptorSet& descriptorSet) = 0; //TODO
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType, uint64_t offset) const = 0;
		virtual void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers, const std::vector<uint32_t>& strides, const std::vector<uint64_t>& offsets) const = 0;
		virtual void BindPipeline(const TRAP::Ref<API::VulkanPipeline>& pipeline) const = 0; //TODO
		virtual void BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
			const TRAP::Ref<RenderTarget>& depthStencil,
			const RendererAPI::LoadActionsDesc* loadActions,
			const std::vector<uint32_t>& colorArraySlices,
			const std::vector<uint32_t>& colorMipSlices,
			uint32_t depthArraySlice,
			uint32_t depthMipSlice) = 0;

		virtual void AddDebugMarker(float r, float g, float b, const char* name) const = 0;
		virtual void BeginDebugMarker(float r, float g, float b, const char* name) const = 0;
		virtual void EndDebugMarker() const = 0;
		
		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) const = 0;
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;

		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex) const = 0;
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t firstInstance) const = 0;
		virtual void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex) const = 0;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t firstInstance, uint32_t firstVertex) const = 0;
		virtual void ExecuteIndirect(const TRAP::Ref<API::VulkanCommandSignature>& cmdSignature,
			uint32_t maxCommandCount,
			const TRAP::Ref<Buffer>& indirectBuffer,
			uint64_t bufferOffset,
			const TRAP::Ref<Buffer>& counterBuffer,
			uint64_t counterBufferOffset) const = 0; //TODO
		
		virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const = 0;

		virtual void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, uint64_t dstOffset, const TRAP::Ref<Buffer>& srcBuffer, uint64_t srcOffset, uint64_t size) const = 0;
		virtual void UpdateVirtualTexture(const TRAP::Ref<API::VulkanTexture>& virtualTexture) = 0; //TODO

		virtual void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, uint32_t startQuery, uint32_t queryCount) const = 0;
		virtual void BeginQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const = 0;
		virtual void EndQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const = 0;
		virtual void ResolveQuery(const TRAP::Ref<QueryPool>& queryPool, const TRAP::Ref<Buffer>& readBackBuffer, uint32_t startQuery, uint32_t queryCount) const = 0;
		
		virtual void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
			const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
			const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const = 0;
		
		virtual void SetStencilReferenceValue(uint32_t val) const = 0;
	
	protected:
		CommandBuffer();

		TRAP::Ref<API::VulkanQueue> m_queue; //TODO
	};
}

#endif /*_TRAP_COMMANDBUFFER_H_*/
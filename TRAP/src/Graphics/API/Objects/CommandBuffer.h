#ifndef TRAP_COMMANDBUFFER_H
#define TRAP_COMMANDBUFFER_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class DescriptorSet;
	class CommandSignature;
	class Pipeline;
	class QueryPool;
	class Buffer;
	class RenderTarget;
	class RootSignature;
	class Texture;

	class CommandBuffer
	{
	public:
		/// @brief Destructor.
		virtual ~CommandBuffer();

		/// @brief Copy constructor.
		consteval CommandBuffer(const CommandBuffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval CommandBuffer& operator=(const CommandBuffer&) noexcept = delete;
		/// @brief Move constructor.
		CommandBuffer(CommandBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		CommandBuffer& operator=(CommandBuffer&&) noexcept = default;

		/// @brief Retrieve the queue used by the command buffer.
		/// @return Queue used by the command buffer.
		[[nodiscard]] TRAP::Ref<Queue> GetQueue() const noexcept;

		/// @brief Bind push constant buffer data to the command buffer.
		/// @param rootSignature Root signature containing the push constant block.
		/// @param name Name of the push constant block.
		/// @param constants Pointer to the constant buffer data.
		/// @param constantsLength Length in bytes of the constant buffer data.
		/// @note There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		virtual void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, std::string_view name,
		                               const void* constants, usize constantsLength) const = 0;
		/// @brief Bind push constant buffer data to the command buffer.
		/// @param rootSignature Root signature containing the push constant block.
		/// @param paramIndex Index of the push constant block in the RootSignatures descriptors array.
		/// @param constants Pointer to the constant buffer data.
		/// @param constantsLength Length in bytes of the constant buffer data.
		virtual void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, u32 paramIndex,
		                                      const void* constants, usize constantsLength) const = 0;
		/// @brief Bind a descriptor set to the command buffer.
		/// @param index Index for which descriptor set to bind.
		/// @param descriptorSet Descriptor set to bind.
		virtual void BindDescriptorSet(u32 index, DescriptorSet& descriptorSet) = 0;
		/// @brief Bind an index buffer to the command buffer.
		/// @param buffer Index buffer to bind.
		/// @param indexType Data type used by the index buffer.
		/// @param offset Starting offset in bytes to use for index buffer.
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType,
		                             u64 offset) const = 0;
		/// @brief Bind vertex buffer(s) to the command buffer.
		/// @param buffers Vertex buffer(s) to bind.
		/// @param strides Stride in bytes of each vertex buffer.
		/// @param offsets Starting offsets in bytes to use for each vertex buffer.
		virtual void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers,
		                              const std::vector<u32>& strides,
									  const std::vector<u64>& offsets) const = 0;
		/// @brief Bind a pipeline to the command buffer.
		/// @param pipeline Pipeline to bind.
		virtual void BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const = 0;
		/// @brief Bind render target(s) to the command buffer.
		/// @param renderTargets Render target(s) to bind.
		/// @param depthStencil Optional depth stencil target to bind.
		/// @param loadActions Optional load actions for each render target.
		/// @param colorArraySlices Optional color array slices for each render target.
		/// @param colorMipSlices Optional color mip slices for each render target.
		/// @param depthArraySlice Optional depth array slice for the depth stencil target.
		/// @param depthMipSlice Optional depth mip slice for the depth stencil target.
		/// @param shadingRate Optional shading rate texture to use.
		/// @note This functions ends the currently running render pass and starts a new one.
		virtual void BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
			                           const TRAP::Ref<RenderTarget>& depthStencil,
			                           const RendererAPI::LoadActionsDesc* loadActions,
			                           const std::vector<u32>* colorArraySlices,
			                           const std::vector<u32>* colorMipSlices,
			                           u32 depthArraySlice, u32 depthMipSlice,
									   const TRAP::Ref<RenderTarget>& shadingRate = nullptr) = 0;

		/// @brief Add a debug marker to the command buffer.
		/// @param color Color for the debug marker.
		/// @param name Name of the marker.
		virtual void AddDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const = 0;
		/// @brief Start a debug marker region.
		/// @param color Color for the debug marker.
		/// @param name Name of the marker.
		virtual void BeginDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const = 0;
		/// @brief End the currently running debug marker region.
		virtual void EndDebugMarker() const = 0;

		/// @brief Begin command buffer recording.
		/// @param oneTimeSubmit Whether each recording should only be submitted once. If true
		///                      then the command buffer will be reset after submission to initial state.
		virtual void Begin(bool oneTimeSubmit = false) = 0;
		/// @brief End command buffer recording.
		/// @note This function also ends the currently running render pass.
		virtual void End() = 0;

		/// @brief Set a new viewport region.
		/// @param x X position for the upper left corner of the viewport.
		/// @param y Y position for the upper left corner of the viewport.
		/// @param width New viewport width.
		/// @param height New viewport height.
		/// @param minDepth Min depth value.
		/// @param maxDepth Max depth value.
		virtual void SetViewport(f32 x, f32 y, f32 width, f32 height,
		                         f32 minDepth, f32 maxDepth) const = 0;
		/// @brief Set a new scissor region.
		/// @param x X offset for the scissor region.
		/// @param y Y offset for the scissor region.
		/// @param width Width for the scissor region.
		/// @param height Height for the scissor region.
		virtual void SetScissor(u32 x, u32 y, u32 width, u32 height) const = 0;

		/// @brief Draw a number of primitives.
		/// @param vertexCount How many vertices to draw.
		/// @param firstVertex Offset to the first vertex to draw.
		virtual void Draw(u32 vertexCount, u32 firstVertex) const = 0;
		/// @brief Draw multiple instances of primitives.
		/// @param vertexCount How many vertices to draw.
		/// @param firstVertex Offset to the first vertex to draw.
		/// @param instanceCount How many instances to draw.
		/// @param firstInstance Offset to the first instance to draw.
		virtual void DrawInstanced(u32 vertexCount, u32 firstVertex, u32 instanceCount,
		                           u32 firstInstance) const = 0;
		/// @brief Draw indexed primitives.
		/// @param indexCount How many indices to draw.
		/// @param firstIndex Offset to the first index to draw.
		/// @param firstVertex Offset to the first vertex to draw.
		virtual void DrawIndexed(u32 indexCount, u32 firstIndex, i32 firstVertex) const = 0;
		/// @brief Draw multiple instances of indexed primitives.
		/// @param indexCount How many indices to draw.
		/// @param firstIndex Offset to the first index to draw.
		/// @param instanceCount How many instances to draw.
		/// @param firstInstance Offset to the first instance to draw.
		/// @param firstVertex Offset to the first vertex to draw.
		virtual void DrawIndexedInstanced(u32 indexCount, u32 firstIndex, u32 instanceCount,
		                                  u32 firstInstance, i32 firstVertex) const = 0;
		/// @brief Draw primitives.
		/// Draw data is provided by the indirect buffer.
		/// Command signature provides information about which type of drawing to perform
		/// (Non-instanced & non-indexed, indexed, instances, indexed & instanced or dispatch).
		/// @param cmdSignature Command signature containing extra data.
		/// @param maxCommandCount Max number of draws to execute.
		/// @param indirectBuffer Buffer containing draw parameters.
		/// @param bufferOffset Byte offset into indirect buffer to start reading from.
		/// @param counterBuffer Buffer containing the draw count.
		/// @param counterBufferOffset Byte offset into counter buffer to start reading from.
		virtual void ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature, u32 maxCommandCount,
			                         const TRAP::Ref<Buffer>& indirectBuffer, u64 bufferOffset,
			                         const TRAP::Ref<Buffer>& counterBuffer, u64 counterBufferOffset) const = 0;

		/// @brief Dispatch compute work.
		/// @param groupCountX Number of local work groups to dispatch in the X dimension.
		/// @param groupCountY Number of local work groups to dispatch in the Y dimension.
		/// @param groupCountZ Number of local work groups to dispatch in the Z dimension.
		virtual void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) const = 0;

		/// @brief Update a buffer with new data.
		/// @param buffer Buffer to update.
		/// @param dstOffset Offset in the buffer to start writing to.
		/// @param srcBuffer Source buffer to read data from.
		/// @param srcOffset Offset in the source buffer to start reading from.
		/// @param size Size of the data to copy.
		virtual void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, u64 dstOffset,
		                          const TRAP::Ref<Buffer>& srcBuffer, u64 srcOffset, u64 size) const = 0;
		/// @brief Update a texture partially with new data.
		/// @param texture Texture to update.
		/// @param srcBuffer Source buffer to read data from.
		/// @param subresourceDesc Subresource description.
		virtual void UpdateSubresource(const Texture* texture,
		                               const TRAP::Ref<Buffer>& srcBuffer,
									   const RendererAPI::SubresourceDataDesc& subresourceDesc) const = 0;
		/// @brief Copy a texture partially into a buffer.
		/// @param dstBuffer Destination to copy data into.
		/// @param texture Source texture to copy from.
		/// @param subresourceDesc Subresource description.
		virtual void CopySubresource(const Buffer* dstBuffer, const Texture* texture,
		                             const RendererAPI::SubresourceDataDesc& subresourceDesc) const = 0;

		/// @brief Reset a query pool.
		/// @param queryPool Query pool to reset.
		/// @param startQuery Initial query index to reset.
		/// @param queryCount Number of queries to reset.
		virtual void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, u32 startQuery,
		                            u32 queryCount) const = 0;
		/// @brief Begin a new query.
		/// @param queryPool Query pool to begin a new query in.
		/// @param desc Query desc.
		virtual void BeginQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const = 0;
		/// @brief End a query.
		/// @param queryPool Query pool to begin a new query in.
		/// @param desc Query desc.
		virtual void EndQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const = 0;
		/// @brief Retrieve the results of a query.
		/// @param queryPool Query pool containing the query results.
		/// @param readBackBuffer Buffer to write results to.
		/// @param startQuery Initial query index.
		/// @param queryCount Number of queries to read.
		virtual void ResolveQuery(const TRAP::Ref<QueryPool>& queryPool, const TRAP::Ref<Buffer>& readBackBuffer,
		                          u32 startQuery, u32 queryCount) const = 0;

		/// @brief Add resource barriers (memory dependency) to the command buffer.
		/// @param bufferBarriers Optional buffer barriers.
		/// @param textureBarriers Optional texture barriers.
		/// @param renderTargetBarriers Optional render target barriers.
		virtual void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* bufferBarriers,
			                         const std::vector<RendererAPI::TextureBarrier>* textureBarriers,
			                         const std::vector<RendererAPI::RenderTargetBarrier>* renderTargetBarriers) const = 0;
		/// @brief Add a resource barrier (memory dependency) to the command buffer.
		/// @param bufferBarrier Optional buffer barrier.
		/// @param textureBarrier Optional texture barrier.
		/// @param renderTargetBarrier Optional render target barrier.
		virtual void ResourceBarrier(const RendererAPI::BufferBarrier* bufferBarrier,
			                         const RendererAPI::TextureBarrier* textureBarrier,
			                         const RendererAPI::RenderTargetBarrier* renderTargetBarrier) const = 0;

		/// @brief Set the stencil reference value for the command buffer.
		/// @param val New value to use as stencil reference.
		virtual void SetStencilReferenceValue(u32 val) const = 0;

		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		virtual void SetShadingRate(RendererAPI::ShadingRate shadingRate,
		                            RendererAPI::ShadingRateCombiner postRasterizerRate,
							        RendererAPI::ShadingRateCombiner finalRate) const = 0;

		/// @brief Clear the currently used color attachment.
		/// @param color Color to clear the color attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		virtual void Clear(const RendererAPI::Color& color, u32 width, u32 height) const = 0;
		/// @brief Clear the currently used depth and stencil attachment.
		/// @param depth Depth value to clear the depth attachment with.
		/// @param stencil Stencil value to clear the stencil attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		virtual void Clear(f32 depth, u32 stencil, u32 width, u32 height) const = 0;
		/// @brief Clear the currently used depth attachment.
		/// @param depth Depth value to clear the depth attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		virtual void Clear(f32 depth, u32 width, u32 height) const = 0;
		/// @brief Clear the currently used stencil attachment.
		/// @param stencil Stencil value to clear the stencil attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		virtual void Clear(u32 stencil, u32 width, u32 height) const = 0;

	protected:
		/// @brief Constructor.
		explicit CommandBuffer(TRAP::Ref<Queue> queue);

		TRAP::Ref<Queue> m_queue = nullptr;
	};
}

#endif /*TRAP_COMMANDBUFFER_H*/
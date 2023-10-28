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
		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~CommandBuffer();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		CommandBuffer(const CommandBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CommandBuffer& operator=(const CommandBuffer&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CommandBuffer(CommandBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CommandBuffer& operator=(CommandBuffer&&) noexcept = default;

		/// <summary>
		/// Retrieve the queue used by the command buffer.
		/// </summary>
		/// <returns>Queue used by the command buffer.</returns>
		[[nodiscard]] TRAP::Ref<Queue> GetQueue() const noexcept;

		/// <summary>
		/// Bind push constant buffer data to the command buffer.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="rootSignature">Root signature containing the push constant block.</param>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constants">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		virtual void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, std::string_view name,
		                               const void* constants, usize constantsLength) const = 0;
		/// <summary>
		/// Bind push constant buffer data to the command buffer.
		/// </summary>
		/// <param name="rootSignature">Root signature containing the push constant block.</param>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constants">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		virtual void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, u32 paramIndex,
		                                      const void* constants, usize constantsLength) const = 0;
		/// <summary>
		/// Bind a descriptor set to the command buffer.
		/// </summary>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="descriptorSet">Descriptor set to bind.</param>
		virtual void BindDescriptorSet(u32 index, DescriptorSet& descriptorSet) = 0;
		/// <summary>
		/// Bind an index buffer to the command buffer.
		/// </summary>
		/// <param name="buffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="offset">Starting offset in bytes to use for index buffer.</param>
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType,
		                             u64 offset) const = 0;
		/// <summary>
		/// Bind vertex buffer(s) to the command buffer.
		/// </summary>
		/// <param name="buffers">Vertex buffer(s) to bind.</param>
		/// <param name="strides">Stride in bytes of each vertex buffer.</param>
		/// <param name="offsets">Starting offsets in bytes to use for each vertex buffer.</param>
		virtual void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers,
		                              const std::vector<u32>& strides,
									  const std::vector<u64>& offsets) const = 0;
		/// <summary>
		/// Bind a pipeline to the command buffer.
		/// </summary>
		/// <param name="pipeline">Pipeline to bind.</param>
		virtual void BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const = 0;
		/// <summary>
		/// Bind render target(s) to the command buffer.
		///
		/// Note: This functions ends the currently running render pass and starts a new one.
		/// </summary>
		/// <param name="renderTargets">Render target(s) to bind.</param>
		/// <param name="depthStencil">Optional depth stencil target to bind.</param>
		/// <param name="loadActions">Optional load actions for each render target.</param>
		/// <param name="colorArraySlices">Optional color array slices for each render target.</param>
		/// <param name="colorMipSlices">Optional color mip slices for each render target.</param>
		/// <param name="depthArraySlice">Optional depth array slice for the depth stencil target.</param>
		/// <param name="depthMipSlice">Optional depth mip slice for the depth stencil target.</param>
		virtual void BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
			                           const TRAP::Ref<RenderTarget>& depthStencil,
			                           const RendererAPI::LoadActionsDesc* loadActions,
			                           const std::vector<u32>* colorArraySlices,
			                           const std::vector<u32>* colorMipSlices,
			                           u32 depthArraySlice, u32 depthMipSlice,
									   const TRAP::Ref<RenderTarget>& shadingRate = nullptr) = 0;

		/// <summary>
		/// Add a debug marker to the command buffer.
		/// </summary>
		/// <param name="color">Color for the debug marker.</param>
		/// <param name="name">Name of the marker.</param>
		virtual void AddDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const = 0;
		/// <summary>
		/// Start a debug marker region.
		/// </summary>
		/// <param name="color">Color for the debug marker.</param>
		/// <param name="name">Name of the marker.</param>
		virtual void BeginDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const = 0;
		/// <summary>
		/// End the currently running debug marker region.
		/// </summary>
		virtual void EndDebugMarker() const = 0;

		/// <summary>
		/// Begin command buffer recording.
		/// </summary>
		virtual void Begin() = 0;
		/// <summary>
		/// End command buffer recording.
		///
		/// Note: This function also ends the currently running render pass.
		/// </summary>
		virtual void End() = 0;

		/// <summary>
		/// Set a new viewport region.
		/// </summary>
		/// <param name="x">X position for the upper left corner of the viewport.</param>
		/// <param name="y">Y position for the upper left corner of the viewport.</param>
		/// <param name="width">New viewport width.</param>
		/// <param name="height">New viewport height.</param>
		/// <param name="minDepth">Min depth value.</param>
		/// <param name="maxDepth">Max depth value.</param>
		virtual void SetViewport(f32 x, f32 y, f32 width, f32 height,
		                         f32 minDepth, f32 maxDepth) const = 0;
		/// <summary>
		/// Set a new scissor region.
		/// </summary>
		/// <param name="x">X offset for the scissor region.</param>
		/// <param name="y">Y offset for the scissor region.</param>
		/// <param name="width">Width for the scissor region.</param>
		/// <param name="height">Height for the scissor region.</param>
		virtual void SetScissor(u32 x, u32 y, u32 width, u32 height) const = 0;

		/// <summary>
		/// Draw a number of primitives.
		/// </summary>
		/// <param name="vertexCount">How many vertices to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		virtual void Draw(u32 vertexCount, u32 firstVertex) const = 0;
		/// <summary>
		/// Draw multiple instances of primitives.
		/// </summary>
		/// <param name="vertexCount">How many vertices to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		/// <param name="instanceCount">How many instances to draw.</param>
		/// <param name="firstInstance">Offset to the first instance to draw.</param>
		virtual void DrawInstanced(u32 vertexCount, u32 firstVertex, u32 instanceCount,
		                           u32 firstInstance) const = 0;
		/// <summary>
		/// Draw indexed primitives.
		/// </summary>
		/// <param name="indexCount">How many indices to draw.</param>
		/// <param name="firstIndex">Offset to the first index to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		virtual void DrawIndexed(u32 indexCount, u32 firstIndex, i32 firstVertex) const = 0;
		/// <summary>
		/// Draw multiple instances of indexed primitives.
		/// </summary>
		/// <param name="indexCount">How many indices to draw.</param>
		/// <param name="firstIndex">Offset to the first index to draw.</param>
		/// <param name="instanceCount">How many instances to draw.</param>
		/// <param name="firstInstance">Offset to the first instance to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		virtual void DrawIndexedInstanced(u32 indexCount, u32 firstIndex, u32 instanceCount,
		                                  u32 firstInstance, i32 firstVertex) const = 0;
		/// <summary>
		/// Draw primitives.
		/// Draw data is provided by the indirect buffer.
		/// Command signature provides information about which type of drawing to perform
		/// (Non-instanced & non-indexed, indexed, instances, indexed & instanced or dispatch).
		/// </summary>
		/// <param name="cmdSignature">Command signature containing extra data.</param>
		/// <param name="maxCommandCount">Max number of draws to execute.</param>
		/// <param name="indirectBuffer">Buffer containing draw parameters.</param>
		/// <param name="bufferOffset">Byte offset into indirect buffer to start reading from.</param>
		/// <param name="counterBuffer">Buffer containing the draw count.</param>
		/// <param name="counterBufferOffset">Byte offset into counter buffer to start reading from.</param>
		virtual void ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature, u32 maxCommandCount,
			                         const TRAP::Ref<Buffer>& indirectBuffer, u64 bufferOffset,
			                         const TRAP::Ref<Buffer>& counterBuffer, u64 counterBufferOffset) const = 0;

		/// <summary>
		/// Dispatch compute work.
		/// </summary>
		/// <param name="groupCountX">Number of local work groups to dispatch in the X dimension.</param>
		/// <param name="groupCountY">Number of local work groups to dispatch in the Y dimension.</param>
		/// <param name="groupCountZ">Number of local work groups to dispatch in the Z dimension.</param>
		virtual void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) const = 0;

		/// <summary>
		/// Update a buffer with new data.
		/// </summary>
		/// <param name="buffer">Buffer to update.</param>
		/// <param name="dstOffset">Offset in the buffer to start writing to.</param>
		/// <param name="srcBuffer">Source buffer to read data from.</param>
		/// <param name="srcOffset">Offset in the source buffer to start reading from.</param>
		/// <param name="size">Size of the data to copy.</param>
		virtual void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, u64 dstOffset,
		                          const TRAP::Ref<Buffer>& srcBuffer, u64 srcOffset, u64 size) const = 0;
		/// <summary>
		/// Update a texture partially with new data.
		/// </summary>
		/// <param name="texture">Texture to update.</param>
		/// <param name="srcBuffer">Source buffer to read data from.</param>
		/// <param name="subresourceDesc">Subresource description.</param>
		virtual void UpdateSubresource(const Texture* texture,
		                               const TRAP::Ref<Buffer>& srcBuffer,
									   const RendererAPI::SubresourceDataDesc& subresourceDesc) const = 0;
		/// <summary>
		/// Copy a texture partially into a buffer.
		/// </summary>
		/// <param name="dstBuffer">Destination to copy data into.</param>
		/// <param name="texture">Source texture to copy from.</param>
		/// <param name="subresourceDesc">Subresource description.</param>
		virtual void CopySubresource(const Buffer* dstBuffer, const Texture* texture,
		                             const RendererAPI::SubresourceDataDesc& subresourceDesc) const = 0;

		/// <summary>
		/// Reset a query pool.
		/// </summary>
		/// <param name="queryPool">Query pool to reset.</param>
		/// <param name="startQuery">Initial query index to reset.</param>
		/// <param name="queryCount">Number of queries to reset.</param>
		virtual void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, u32 startQuery,
		                            u32 queryCount) const = 0;
		/// <summary>
		/// Begin a new query.
		/// </summary>
		/// <param name="queryPool">Query pool to begin a new query in.</param>
		/// <param name="desc">Query desc.</param>
		virtual void BeginQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const = 0;
		/// <summary>
		/// End a query.
		/// </summary>
		/// <param name="queryPool">Query pool to begin a new query in.</param>
		/// <param name="desc">Query desc.</param>
		virtual void EndQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const = 0;
		/// <summary>
		/// Retrieve the results of a query.
		/// </summary>
		/// <param name="queryPool">Query pool containing the query results.</param>
		/// <param name="readBackBuffer">Buffer to write results to.</param>
		/// <param name="startQuery">Initial query index.</param>
		/// <param name="queryCount">Number of queries to read.</param>
		virtual void ResolveQuery(const TRAP::Ref<QueryPool>& queryPool, const TRAP::Ref<Buffer>& readBackBuffer,
		                          u32 startQuery, u32 queryCount) const = 0;

		/// <summary>
		/// Add resource barriers (memory dependency) to the command buffer.
		/// </summary>
		/// <param name="bufferBarriers">Optional buffer barriers.</param>
		/// <param name="textureBarriers">Optional texture barriers.</param>
		/// <param name="renderTargetBarriers">Optional render target barriers.</param>
		virtual void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* bufferBarriers,
			                         const std::vector<RendererAPI::TextureBarrier>* textureBarriers,
			                         const std::vector<RendererAPI::RenderTargetBarrier>* renderTargetBarriers) const = 0;
		/// <summary>
		/// Add a resource barrier (memory dependency) to the command buffer.
		/// </summary>
		/// <param name="bufferBarrier">Optional buffer barrier.</param>
		/// <param name="textureBarrier">Optional texture barrier.</param>
		/// <param name="renderTargetBarrier">Optional render target barrier.</param>
		virtual void ResourceBarrier(const RendererAPI::BufferBarrier* bufferBarrier,
			                         const RendererAPI::TextureBarrier* textureBarrier,
			                         const RendererAPI::RenderTargetBarrier* renderTargetBarrier) const = 0;

		/// <summary>
		/// Set the stencil reference value for the command buffer.
		/// </summary>
		/// <param name="val">New value to use as stencil reference.</param>
		virtual void SetStencilReferenceValue(u32 val) const = 0;

		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		virtual void SetShadingRate(RendererAPI::ShadingRate shadingRate,
		                            RendererAPI::ShadingRateCombiner postRasterizerRate,
							        RendererAPI::ShadingRateCombiner finalRate) const = 0;

		/// <summary>
		/// Clear the currently used color attachment.
		/// </summary>
		/// <param name="color">Color to clear the color attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(const RendererAPI::Color& color, u32 width, u32 height) const = 0;
		/// <summary>
		/// Clear the currently used depth and stencil attachment.
		/// </summary>
		/// <param name="depth">Depth value to clear the depth attachment with.</param>
		/// <param name="stencil">Stencil value to clear the stencil attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(f32 depth, u32 stencil, u32 width, u32 height) const = 0;
		/// <summary>
		/// Clear the currently used depth attachment.
		/// </summary>
		/// <param name="depth">Depth value to clear the depth attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(f32 depth, u32 width, u32 height) const = 0;
		/// <summary>
		/// Clear the currently used stencil attachment.
		/// </summary>
		/// <param name="stencil">Stencil value to clear the stencil attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(u32 stencil, u32 width, u32 height) const = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		explicit CommandBuffer(TRAP::Ref<Queue> queue);

		TRAP::Ref<Queue> m_queue = nullptr;
	};
}

#endif /*TRAP_COMMANDBUFFER_H*/
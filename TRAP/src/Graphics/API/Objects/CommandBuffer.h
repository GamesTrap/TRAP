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
		CommandBuffer(const CommandBuffer&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		CommandBuffer& operator=(const CommandBuffer&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		CommandBuffer(CommandBuffer&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		CommandBuffer& operator=(CommandBuffer&&) = default;

		/// <summary>
		/// Retrieve the queue used by the command buffer.
		/// </summary>
		/// <returns>Queue used by the command buffer.</returns>
		virtual TRAP::Ref<Queue> GetQueue() const;

		/// <summary>
		/// Bind push constant buffer data to the command buffer.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="rootSignature">Root signature containing the push constant block.</param>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constants">Pointer to the constant buffer data.</param>
		virtual void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, const char* name,
		                               const void* constants) const = 0;
		/// <summary>
		/// Bind push constant buffer data to the command buffer.
		/// </summary>
		/// <param name="rootSignature">Root signature containing the push constant block.</param>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constants">Pointer to the constant buffer data.</param>
		virtual void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, uint32_t paramIndex,
		                                      const void* constants) const = 0;
		/// <summary>
		/// Bind a descriptor set to the command buffer.
		/// </summary>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="descriptorSet">Descriptor set to bind.</param>
		virtual void BindDescriptorSet(uint32_t index, DescriptorSet& descriptorSet) = 0;
		/// <summary>
		/// Bind an index buffer to the command buffer.
		/// </summary>
		/// <param name="buffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="offset">Starting offset in bytes to use for index buffer.</param>
		virtual void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType,
		                             uint64_t offset) const = 0;
		/// <summary>
		/// Bind vertex buffer(s) to the command buffer.
		/// </summary>
		/// <param name="buffers">Vertex buffer(s) to bind.</param>
		/// <param name="strides">Stride in bytes of each vertex buffer.</param>
		/// <param name="offsets">Starting offsets in bytes to use for each vertex buffer.</param>
		virtual void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers,
		                              const std::vector<uint32_t>& strides,
									  const std::vector<uint64_t>& offsets) const = 0;
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
			                           const std::vector<uint32_t>* colorArraySlices,
			                           const std::vector<uint32_t>* colorMipSlices,
			                           uint32_t depthArraySlice, uint32_t depthMipSlice) = 0;

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
		virtual void SetViewport(float x, float y, float width, float height,
		                         float minDepth, float maxDepth) const = 0;
		/// <summary>
		/// Set a new scissor region.
		/// </summary>
		/// <param name="x">X offset for the scissor region.</param>
		/// <param name="y">Y offset for the scissor region.</param>
		/// <param name="width">Width for the scissor region.</param>
		/// <param name="height">Height for the scissor region.</param>
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;

		/// <summary>
		/// Draw a number of primitives.
		/// </summary>
		/// <param name="vertexCount">How many vertices to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex) const = 0;
		/// <summary>
		/// Draw multiple instances of primitives.
		/// </summary>
		/// <param name="vertexCount">How many vertices to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		/// <param name="instanceCount">How many instances to draw.</param>
		/// <param name="firstInstance">Offset to the first instance to draw.</param>
		virtual void DrawInstanced(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount,
		                           uint32_t firstInstance) const = 0;
		/// <summary>
		/// Draw indexed primitives.
		/// </summary>
		/// <param name="indexCount">How many indices to draw.</param>
		/// <param name="firstIndex">Offset to the first index to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		virtual void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t firstVertex) const = 0;
		/// <summary>
		/// Draw multiple instances of indexed primitives.
		/// </summary>
		/// <param name="indexCount">How many indices to draw.</param>
		/// <param name="firstIndex">Offset to the first index to draw.</param>
		/// <param name="instanceCount">How many instances to draw.</param>
		/// <param name="firstInstance">Offset to the first instance to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount,
		                                  uint32_t firstInstance, uint32_t firstVertex) const = 0;
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
		virtual void ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature, uint32_t maxCommandCount,
			                         const TRAP::Ref<Buffer>& indirectBuffer, uint64_t bufferOffset,
			                         const TRAP::Ref<Buffer>& counterBuffer, uint64_t counterBufferOffset) const = 0;

		/// <summary>
		/// Dispatch compute work.
		/// </summary>
		/// <param name="groupCountX">Number of local work groups to dispatch in the X dimension.</param>
		/// <param name="groupCountY">Number of local work groups to dispatch in the Y dimension.</param>
		/// <param name="groupCountZ">Number of local work groups to dispatch in the Z dimension.</param>
		virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const = 0;

		/// <summary>
		/// Update a buffer with new data.
		/// </summary>
		/// <param name="buffer">Buffer to update.</param>
		/// <param name="dstOffset">Offset in the buffer to start writing to.</param>
		/// <param name="srcBuffer">Source buffer to read data from.</param>
		/// <param name="srcOffset">Offset in the source buffer to start reading from.</param>
		/// <param name="size">Size of the data to copy.</param>
		virtual void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, uint64_t dstOffset,
		                          const TRAP::Ref<Buffer>& srcBuffer, uint64_t srcOffset, uint64_t size) const = 0;
		/// <summary>
		/// Update a texture partially with new data.
		/// </summary>
		/// <param name="texture">Texture to update.</param>
		/// <param name="srcBuffer">Source buffer to read data from.</param>
		/// <param name="subresourceDesc">Subresource description.</param>
		virtual void UpdateSubresource(Texture* texture,
		                               const TRAP::Ref<Buffer>& srcBuffer,
									   const RendererAPI::SubresourceDataDesc& subresourceDesc) const = 0;

		/// <summary>
		/// Reset a query pool.
		/// </summary>
		/// <param name="queryPool">Query pool to reset.</param>
		/// <param name="startQuery">Initial query index to reset.</param>
		/// <param name="queryCount">Number of queries to reset.</param>
		virtual void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, uint32_t startQuery,
		                            uint32_t queryCount) const = 0;
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
		                          uint32_t startQuery, uint32_t queryCount) const = 0;

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
		virtual void SetStencilReferenceValue(uint32_t val) const = 0;

		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="texture">Unused by Vulkan.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		virtual void SetShadingRate(RendererAPI::ShadingRate shadingRate,
						            Ref<TRAP::Graphics::Texture> texture,
		                            RendererAPI::ShadingRateCombiner postRasterizerRate,
							        RendererAPI::ShadingRateCombiner finalRate) const = 0;

		/// <summary>
		/// Clear the currently used color attachment.
		/// </summary>
		/// <param name="color">Color to clear the color attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(TRAP::Math::Vec4 color, uint32_t width, uint32_t height) const = 0;
		/// <summary>
		/// Clear the currently used depth and stencil attachment.
		/// </summary>
		/// <param name="depth">Depth value to clear the depth attachment with.</param>
		/// <param name="stencil">Stencil value to clear the stencil attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(float depth, uint32_t stencil, uint32_t width, uint32_t height) const = 0;
		/// <summary>
		/// Clear the currently used depth attachment.
		/// </summary>
		/// <param name="depth">Depth value to clear the depth attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(float depth, uint32_t width, uint32_t height) const = 0;
		/// <summary>
		/// Clear the currently used stencil attachment.
		/// </summary>
		/// <param name="stencil">Stencil value to clear the stencil attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		virtual void Clear(uint32_t stencil, uint32_t width, uint32_t height) const = 0;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		CommandBuffer();

		TRAP::Ref<Queue> m_queue;
	};
}

#endif /*TRAP_COMMANDBUFFER_H*/
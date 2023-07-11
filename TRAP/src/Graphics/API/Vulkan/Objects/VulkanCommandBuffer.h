#ifndef TRAP_VULKANCOMMANDBUFFER_H
#define TRAP_VULKANCOMMANDBUFFER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/CommandBuffer.h"
#include "Graphics/API/Objects/Queue.h"

namespace TRAP::Graphics::API
{
	class VulkanPipeline;
	class VulkanCommandSignature;
	class VulkanQueryPool;
	class VulkanDescriptorSet;
	class VulkanCommandPool;
	class VulkanDevice;
	class VulkanTexture;

	class VulkanCommandBuffer final : public CommandBuffer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="device">Vulkan device.</param>
		/// <param name="queue">Queue to use for resource barriers.</param>
		/// <param name="commandPool">Command pool to retrieve command buffer from.</param>
		/// <param name="secondary">Is this a secondary command buffer?</param>
		VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<Queue> queue, VkCommandPool& commandPool,
		                    bool secondary);

		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanCommandBuffer() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanCommandBuffer(const VulkanCommandBuffer&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) noexcept = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		constexpr VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept = delete;

		/// <summary>
		/// Retrieve the VkCommandBuffer handle.
		/// </summary>
		/// <returns>VkCommandBuffer handle.</returns>
		[[nodiscard]] constexpr VkCommandBuffer GetVkCommandBuffer() const noexcept;
		/// <summary>
		/// Retrieve the queue type used for resource barriers.
		/// </summary>
		/// <returns>Queue type used for resource barriers.</returns>
		[[nodiscard]] RendererAPI::QueueType GetQueueType() const;
		/// <summary>
		/// Retrieve whether this is a secondary command buffer.
		/// </summary>
		/// <returns>True of this is a secondary command buffer, false otherwise.</returns>
		[[nodiscard]] constexpr bool IsSecondary() const noexcept;

		/// <summary>
		/// Bind push constant buffer data to the command buffer.
		/// Note: There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		/// </summary>
		/// <param name="rootSignature">Root signature containing the push constant block.</param>
		/// <param name="name">Name of the push constant block.</param>
		/// <param name="constants">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, std::string_view name,
		                       const void* constants, std::size_t constantsLength) const override;
		/// <summary>
		/// Bind push constant buffer data to the command buffer.
		/// </summary>
		/// <param name="rootSignature">Root signature containing the push constant block.</param>
		/// <param name="paramIndex">Index of the push constant block in the RootSignatures descriptors array.</param>
		/// <param name="constants">Pointer to the constant buffer data.</param>
		/// <param name="constantsLength">Length in bytes of the constant buffer data.</param>
		void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, uint32_t paramIndex,
		                              const void* constants, std::size_t constantsLength) const override;
		/// <summary>
		/// Bind a descriptor set to the command buffer.
		/// </summary>
		/// <param name="index">Index for which descriptor set to bind.</param>
		/// <param name="descriptorSet">Descriptor set to bind.</param>
		void BindDescriptorSet(uint32_t index, DescriptorSet& descriptorSet) override;
		/// <summary>
		/// Bind an index buffer to the command buffer.
		/// </summary>
		/// <param name="buffer">Index buffer to bind.</param>
		/// <param name="indexType">Data type used by the index buffer.</param>
		/// <param name="offset">Starting offset in bytes to use for index buffer.</param>
		void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType,
		                     uint64_t offset) const override;
		/// <summary>
		/// Bind vertex buffer(s) to the command buffer.
		/// </summary>
		/// <param name="buffers">Vertex buffer(s) to bind.</param>
		/// <param name="strides">Stride in bytes of each vertex buffer.</param>
		/// <param name="offsets">Starting offsets in bytes to use for each vertex buffer.</param>
		void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers, const std::vector<uint32_t>& strides,
		                      const std::vector<uint64_t>& offsets) const override;
		/// <summary>
		/// Bind a pipeline to the command buffer.
		/// </summary>
		/// <param name="pipeline">Pipeline to bind.</param>
		void BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const override;
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
		/// <param name="shadingRate">Optional shading rate texture.</param>
		void BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
		                       const TRAP::Ref<RenderTarget>& depthStencil,
							   const RendererAPI::LoadActionsDesc* loadActions,
							   const std::vector<uint32_t>* colorArraySlices,
		                       const std::vector<uint32_t>* colorMipSlices,
							   uint32_t depthArraySlice,
							   uint32_t depthMipSlice,
							   const TRAP::Ref<RenderTarget>& shadingRate = nullptr) override;

		/// <summary>
		/// Add a debug marker to the command buffer.
		/// </summary>
		/// <param name="color">Color for the debug marker.</param>
		/// <param name="name">Name of the marker.</param>
		void AddDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const override;
		/// <summary>
		/// Start a debug marker region.
		/// </summary>
		/// <param name="color">Color for the debug marker.</param>
		/// <param name="name">Name of the marker.</param>
		void BeginDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const override;
		/// <summary>
		/// End the currently running debug marker region.
		/// </summary>
		void EndDebugMarker() const override;

		/// <summary>
		/// Begin command buffer recording.
		/// </summary>
		void Begin() override;
		/// <summary>
		/// End command buffer recording.
		///
		/// Note: This function also ends the currently running render pass.
		/// </summary>
		void End() override;

		/// <summary>
		/// Set a new viewport region.
		/// </summary>
		/// <param name="x">X position for the upper left corner of the viewport.</param>
		/// <param name="y">Y position for the upper left corner of the viewport.</param>
		/// <param name="width">New viewport width.</param>
		/// <param name="height">New viewport height.</param>
		/// <param name="minDepth">Min depth value.</param>
		/// <param name="maxDepth">Max depth value.</param>
		void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) const override;
		/// <summary>
		/// Set a new scissor region.
		/// </summary>
		/// <param name="x">X offset for the scissor region.</param>
		/// <param name="y">Y offset for the scissor region.</param>
		/// <param name="width">Width for the scissor region.</param>
		/// <param name="height">Height for the scissor region.</param>
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;

		/// <summary>
		/// Draw a number of primitives.
		/// </summary>
		/// <param name="vertexCount">How many vertices to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		void Draw(uint32_t vertexCount, uint32_t firstVertex) const override;
		/// <summary>
		/// Draw multiple instances of primitives.
		/// </summary>
		/// <param name="vertexCount">How many vertices to draw.</param>
		/// <param name="firstVertex">Offset to the first vertex to draw.</param>
		/// <param name="instanceCount">How many instances to draw.</param>
		/// <param name="firstInstance">Offset to the first instance to draw.</param>
		void DrawInstanced(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount,
		                   uint32_t firstInstance) const override;
		/// <summary>
		/// Draw indexed primitives.
		/// </summary>
		/// <param name="indexCount">How many indices to draw.</param>
		/// <param name="firstIndex">Offset to the first index to draw.</param>
		/// <param name="vertexOffset">Offset to the first vertex to draw.</param>
		void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset) const override;
		/// <summary>
		/// Draw multiple instances of indexed primitives.
		/// </summary>
		/// <param name="indexCount">How many indices to draw.</param>
		/// <param name="firstIndex">Offset to the first index to draw.</param>
		/// <param name="instanceCount">How many instances to draw.</param>
		/// <param name="firstInstance">Offset to the first instance to draw.</param>
		/// <param name="vertexOffset">Offset to the first vertex to draw.</param>
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount,
		                          uint32_t firstInstance, int32_t vertexOffset) const override;
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
		void ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature, uint32_t maxCommandCount,
		                     const TRAP::Ref<Buffer>& indirectBuffer, uint64_t bufferOffset,
		                     const TRAP::Ref<Buffer>& counterBuffer, uint64_t counterBufferOffset) const override;

		/// <summary>
		/// Dispatch compute work.
		/// </summary>
		/// <param name="groupCountX">Number of local work groups to dispatch in the X dimension.</param>
		/// <param name="groupCountY">Number of local work groups to dispatch in the Y dimension.</param>
		/// <param name="groupCountZ">Number of local work groups to dispatch in the Z dimension.</param>
		void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) const override;

		/// <summary>
		/// Update a buffer with new data.
		/// </summary>
		/// <param name="buffer">Buffer to update.</param>
		/// <param name="dstOffset">Offset in the buffer to start writing to.</param>
		/// <param name="srcBuffer">Source buffer to read data from.</param>
		/// <param name="srcOffset">Offset in the source buffer to start reading from.</param>
		/// <param name="size">Size of the data to copy.</param>
		void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, uint64_t dstOffset, const TRAP::Ref<Buffer>& srcBuffer,
		                  uint64_t srcOffset, uint64_t size) const override;
		/// <summary>
		/// Update a texture partially with new data.
		/// </summary>
		/// <param name="texture">Texture to update.</param>
		/// <param name="srcBuffer">Source buffer to read data from.</param>
		/// <param name="subresourceDesc">Subresource description.</param>
		void UpdateSubresource(const TRAP::Graphics::Texture* texture, const TRAP::Ref<Buffer>& srcBuffer,
		                       const RendererAPI::SubresourceDataDesc& subresourceDesc) const override;
		/// <summary>
		/// Copy a texture partially into a buffer.
		/// </summary>
		/// <param name="dstBuffer">Destination to copy data into.</param>
		/// <param name="texture">Source texture to copy from.</param>
		/// <param name="subresourceDesc">Subresource description.</param>
		void CopySubresource(const Buffer* dstBuffer, const Texture* texture,
		                     const RendererAPI::SubresourceDataDesc& subresourceDesc) const override;

		/// <summary>
		/// Reset a query pool.
		/// </summary>
		/// <param name="queryPool">Query pool to reset.</param>
		/// <param name="startQuery">Initial query index to reset.</param>
		/// <param name="queryCount">Number of queries to reset.</param>
		void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, uint32_t startQuery,
		                    uint32_t queryCount) const override;
		/// <summary>
		/// Begin a new query.
		/// </summary>
		/// <param name="queryPool">Query pool to begin a new query in.</param>
		/// <param name="desc">Query desc.</param>
		void BeginQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const override;
		/// <summary>
		/// End a query.
		/// </summary>
		/// <param name="queryPool">Query pool to begin a new query in.</param>
		/// <param name="desc">Query desc.</param>
		void EndQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const override;
		/// <summary>
		/// Retrieve the results of a query.
		/// </summary>
		/// <param name="queryPool">Query pool containing the query results.</param>
		/// <param name="readBackBuffer">Buffer to write results to.</param>
		/// <param name="startQuery">Initial query index.</param>
		/// <param name="queryCount">Number of queries to read.</param>
		void ResolveQuery(const TRAP::Ref<QueryPool>& queryPool, const TRAP::Ref<Buffer>& readBackBuffer,
		                  uint32_t startQuery, uint32_t queryCount) const override;

		/// <summary>
		/// Add resource barriers (memory dependency) to the command buffer.
		/// </summary>
		/// <param name="bufferBarriers">Optional buffer barriers.</param>
		/// <param name="textureBarriers">Optional texture barriers.</param>
		/// <param name="renderTargetBarriers">Optional render target barriers.</param>
		void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* bufferBarriers,
		                     const std::vector<RendererAPI::TextureBarrier>* textureBarriers,
		                     const std::vector<RendererAPI::RenderTargetBarrier>* renderTargetBarriers) const override;
		/// <summary>
		/// Add a resource barrier (memory dependency) to the command buffer.
		/// </summary>
		/// <param name="bufferBarrier">Optional buffer barrier.</param>
		/// <param name="textureBarrier">Optional texture barrier.</param>
		/// <param name="renderTargetBarrier">Optional render target barrier.</param>
		void ResourceBarrier(const RendererAPI::BufferBarrier* bufferBarrier,
		                     const RendererAPI::TextureBarrier* textureBarrier,
		                     const RendererAPI::RenderTargetBarrier* renderTargetBarrier) const override;

		/// <summary>
		/// Set the stencil reference value for the command buffer.
		/// </summary>
		/// <param name="val">New value to use as stencil reference.</param>
		void SetStencilReferenceValue(uint32_t val) const override;

		/// <summary>
		/// Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// </summary>
		/// <param name="shadingRate">Shading rate to use.</param>
		/// <param name="postRasterizerRate">Shading rate combiner to use.</param>
		/// <param name="finalRate">Shading rate combiner to use.</param>
		void SetShadingRate(RendererAPI::ShadingRate shadingRate,
		                    RendererAPI::ShadingRateCombiner postRasterizerRate,
							RendererAPI::ShadingRateCombiner finalRate) const override;

		/// <summary>
		/// Clear the currently used color attachment.
		/// </summary>
		/// <param name="color">Color to clear the color attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		void Clear(const RendererAPI::Color& color, uint32_t width, uint32_t height) const override;
		/// <summary>
		/// Clear the currently used depth and stencil attachment.
		/// </summary>
		/// <param name="depth">Depth value to clear the depth attachment with.</param>
		/// <param name="stencil">Stencil value to clear the stencil attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		void Clear(float depth, uint32_t stencil, uint32_t width, uint32_t height) const override;
		/// <summary>
		/// Clear the currently used depth attachment.
		/// </summary>
		/// <param name="depth">Depth value to clear the depth attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		void Clear(float depth, uint32_t width, uint32_t height) const override;
		/// <summary>
		/// Clear the currently used stencil attachment.
		/// </summary>
		/// <param name="stencil">Stencil value to clear the stencil attachment with.</param>
		/// <param name="width">Width of the area to clear.</param>
		/// <param name="height">Height of the area to clear.</param>
		void Clear(uint32_t stencil, uint32_t width, uint32_t height) const override;

		/// <summary>
		/// Resolve a multisample color texture to a non-multisample color texture.
		/// </summary>
		/// <param name="srcImage">Source multisample color texture to resolve.</param>
		/// <param name="srcState">Source texture state.</param>
		/// <param name="dstImage">Destination non-multisample color texture to resolve into.</param>
		/// <param name="dstState">Destination texture state.</param>
		void ResolveImage(const Ref<API::VulkanTexture>& srcImage, RendererAPI::ResourceState srcState,
		                  const Ref<API::VulkanTexture>& dstImage, RendererAPI::ResourceState dstState) const;

		/// <summary>
		/// Retrieve the currently active VkRenderPass.
		/// </summary>
		/// <returns>Currently active VkRenderPass.</returns>
		[[nodiscard]] constexpr VkRenderPass GetActiveVkRenderPass() const noexcept;

	private:
		/// <summary>
		/// Hash the provided data.
		/// </summary>
		/// <param name="mem">Pointer to data to hash.</param>
		/// <param name="size">Size of data to hash.</param>
		/// <param name="prev">Previous hash value.</param>
		static std::size_t HashAlg(const auto* mem, std::size_t size, const std::size_t prev = 2166136261U)
		{
			ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

			TRAP_ASSERT(mem, "VulkanCommandBuffer::HashAlg(): mem is nullptr!");
			TRAP_ASSERT(size, "VulkanCommandBuffer::HashAlg(): size is 0!");

			//Intentionally uint32_t instead of std::size_t, so the behavior is the same regardless of size.
			uint32_t result = static_cast<uint32_t>(prev);

			while (size--)
				result = static_cast<uint32_t>((result * 16777619) ^ *mem++);

			return NumericCast<std::size_t>(result);
		}

		TRAP::Ref<API::VulkanDevice> m_device;

		VkCommandBuffer m_vkCommandBuffer;

		VkCommandPool& m_vkCommandPool;
		bool m_secondary;

		VkRenderPass m_activeRenderPass;
		VkPipelineLayout m_boundPipelineLayout;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkCommandBuffer TRAP::Graphics::API::VulkanCommandBuffer::GetVkCommandBuffer() const noexcept
{
	return m_vkCommandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::API::VulkanCommandBuffer::IsSecondary() const noexcept
{
	return m_secondary;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkRenderPass TRAP::Graphics::API::VulkanCommandBuffer::GetActiveVkRenderPass() const noexcept
{
	return m_activeRenderPass;
}

#endif /*TRAP_VULKANCOMMANDBUFFER_H*/
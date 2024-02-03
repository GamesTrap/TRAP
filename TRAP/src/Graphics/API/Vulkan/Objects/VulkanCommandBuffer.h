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
		/// @brief Constructor.
		/// @param device Vulkan device.
		/// @param queue Queue to use for resource barriers.
		/// @param commandPool Command pool to retrieve command buffer from.
		/// @param secondary Is this a secondary command buffer?
		VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<Queue> queue, VkCommandPool& commandPool,
		                    bool secondary);

		/// @brief Destructor.
		~VulkanCommandBuffer() override;

		/// @brief Copy constructor.
		consteval VulkanCommandBuffer(const VulkanCommandBuffer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) noexcept = delete;
		/// @brief Move constructor.
		VulkanCommandBuffer(VulkanCommandBuffer&&) noexcept = default;
		/// @brief Move assignment operator.
		constexpr VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept = delete;

		/// @brief Retrieve the VkCommandBuffer handle.
		/// @return VkCommandBuffer handle.
		[[nodiscard]] constexpr VkCommandBuffer GetVkCommandBuffer() const noexcept;
		/// @brief Retrieve the queue type used for resource barriers.
		/// @return Queue type used for resource barriers.
		[[nodiscard]] RendererAPI::QueueType GetQueueType() const;
		/// @brief Retrieve whether this is a secondary command buffer.
		/// @return True of this is a secondary command buffer, false otherwise.
		[[nodiscard]] constexpr bool IsSecondary() const noexcept;

		/// @brief Bind push constant buffer data to the command buffer.
		/// @param rootSignature Root signature containing the push constant block.
		/// @param name Name of the push constant block.
		/// @param constants Constant buffer data.
		/// @note There is an optimized function which uses the index into the RootSignature
		///       instead of the name of the push constant block.
		void BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature, std::string_view name,
		                       std::span<const u8> constants) const override;
		/// @brief Bind push constant buffer data to the command buffer.
		/// @param rootSignature Root signature containing the push constant block.
		/// @param paramIndex Index of the push constant block in the RootSignatures descriptors array.
		/// @param constants Constant buffer data.
		void BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature, u32 paramIndex,
		                              std::span<const u8> constants) const override;
		/// @brief Bind a descriptor set to the command buffer.
		/// @param index Index for which descriptor set to bind.
		/// @param descriptorSet Descriptor set to bind.
		void BindDescriptorSet(u32 index, DescriptorSet& descriptorSet) override;
		/// @brief Bind an index buffer to the command buffer.
		/// @param buffer Index buffer to bind.
		/// @param indexType Data type used by the index buffer.
		/// @param offset Starting offset in bytes to use for index buffer.
		void BindIndexBuffer(const TRAP::Ref<Buffer>& buffer, RendererAPI::IndexType indexType,
		                     u64 offset) const override;
		/// @brief Bind vertex buffer(s) to the command buffer.
		/// @param buffers Vertex buffer(s) to bind.
		/// @param strides Stride in bytes of each vertex buffer.
		/// @param offsets Starting offsets in bytes to use for each vertex buffer.
		void BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers, const std::vector<u32>& strides,
		                      const std::vector<u64>& offsets) const override;
		/// @brief Bind a pipeline to the command buffer.
		/// @param pipeline Pipeline to bind.
		void BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const override;
		/// @brief Bind render target(s) to the command buffer.
		/// @param renderTargets Render target(s) to bind.
		/// @param depthStencil Optional depth stencil target to bind.
		/// @param loadActions Optional load actions for each render target.
		/// @param colorArraySlices Optional color array slices for each render target.
		/// @param colorMipSlices Optional color mip slices for each render target.
		/// @param depthArraySlice Optional depth array slice for the depth stencil target.
		/// @param depthMipSlice Optional depth mip slice for the depth stencil target.
		/// @param shadingRate Optional shading rate texture.
		/// @note This functions ends the currently running render pass and starts a new one.
		void BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
		                       const TRAP::Ref<RenderTarget>& depthStencil,
							   const RendererAPI::LoadActionsDesc* loadActions,
							   const std::vector<u32>* colorArraySlices,
		                       const std::vector<u32>* colorMipSlices,
							   u32 depthArraySlice,
							   u32 depthMipSlice,
							   const TRAP::Ref<RenderTarget>& shadingRate = nullptr) override;

		/// @brief Add a debug marker to the command buffer.
		/// @param color Color for the debug marker.
		/// @param name Name of the marker.
		void AddDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const override;
		/// @brief Start a debug marker region.
		/// @param color Color for the debug marker.
		/// @param name Name of the marker.
		void BeginDebugMarker(const TRAP::Math::Vec3& color, std::string_view name) const override;
		/// @brief End the currently running debug marker region.
		void EndDebugMarker() const override;

		/// @brief Begin command buffer recording.
		/// @param oneTimeSubmit Whether each recording should only be submitted once. If true
		///                      then the command buffer will be reset after submission to initial state.
		void Begin(bool oneTimeSubmit = false) override;
		/// @brief End command buffer recording.
		/// @note This function also ends the currently running render pass.
		void End() override;

		/// @brief Set a new viewport region.
		/// @param x X position for the upper left corner of the viewport.
		/// @param y Y position for the upper left corner of the viewport.
		/// @param width New viewport width.
		/// @param height New viewport height.
		/// @param minDepth Min depth value.
		/// @param maxDepth Max depth value.
		void SetViewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth, f32 maxDepth) const override;
		/// @brief Set a new scissor region.
		/// @param x X offset for the scissor region.
		/// @param y Y offset for the scissor region.
		/// @param width Width for the scissor region.
		/// @param height Height for the scissor region.
		void SetScissor(u32 x, u32 y, u32 width, u32 height) const override;

		/// @brief Draw a number of primitives.
		/// @param vertexCount How many vertices to draw.
		/// @param firstVertex Offset to the first vertex to draw.
		void Draw(u32 vertexCount, u32 firstVertex) const override;
		/// @brief Draw multiple instances of primitives.
		/// @param vertexCount How many vertices to draw.
		/// @param firstVertex Offset to the first vertex to draw.
		/// @param instanceCount How many instances to draw.
		/// @param firstInstance Offset to the first instance to draw.
		void DrawInstanced(u32 vertexCount, u32 firstVertex, u32 instanceCount,
		                   u32 firstInstance) const override;
		/// @brief Draw indexed primitives.
		/// @param indexCount How many indices to draw.
		/// @param firstIndex Offset to the first index to draw.
		/// @param vertexOffset Offset to the first vertex to draw.
		void DrawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset) const override;
		/// @brief Draw multiple instances of indexed primitives.
		/// @param indexCount How many indices to draw.
		/// @param firstIndex Offset to the first index to draw.
		/// @param instanceCount How many instances to draw.
		/// @param firstInstance Offset to the first instance to draw.
		/// @param vertexOffset Offset to the first vertex to draw.
		void DrawIndexedInstanced(u32 indexCount, u32 firstIndex, u32 instanceCount,
		                          u32 firstInstance, i32 vertexOffset) const override;
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
		void ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature, u32 maxCommandCount,
		                     const TRAP::Ref<Buffer>& indirectBuffer, u64 bufferOffset,
		                     const TRAP::Ref<Buffer>& counterBuffer, u64 counterBufferOffset) const override;

		/// @brief Dispatch compute work.
		/// @param groupCountX Number of local work groups to dispatch in the X dimension.
		/// @param groupCountY Number of local work groups to dispatch in the Y dimension.
		/// @param groupCountZ Number of local work groups to dispatch in the Z dimension.
		void Dispatch(u32 groupCountX, u32 groupCountY, u32 groupCountZ) const override;

		/// @brief Update a buffer with new data.
		/// @param buffer Buffer to update.
		/// @param dstOffset Offset in the buffer to start writing to.
		/// @param srcBuffer Source buffer to read data from.
		/// @param srcOffset Offset in the source buffer to start reading from.
		/// @param size Size of the data to copy.
		void UpdateBuffer(const TRAP::Ref<Buffer>& buffer, u64 dstOffset, const TRAP::Ref<Buffer>& srcBuffer,
		                  u64 srcOffset, u64 size) const override;
		/// @brief Update a texture partially with new data.
		/// @param texture Texture to update.
		/// @param srcBuffer Source buffer to read data from.
		/// @param subresourceDesc Subresource description.
		void UpdateSubresource(const TRAP::Graphics::Texture* texture, const TRAP::Ref<Buffer>& srcBuffer,
		                       const RendererAPI::SubresourceDataDesc& subresourceDesc) const override;
		/// @brief Copy a texture partially into a buffer.
		/// @param dstBuffer Destination to copy data into.
		/// @param texture Source texture to copy from.
		/// @param subresourceDesc Subresource description.
		void CopySubresource(const Buffer* dstBuffer, const Texture* texture,
		                     const RendererAPI::SubresourceDataDesc& subresourceDesc) const override;

		/// @brief Reset a query pool.
		/// @param queryPool Query pool to reset.
		/// @param startQuery Initial query index to reset.
		/// @param queryCount Number of queries to reset.
		void ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool, u32 startQuery,
		                    u32 queryCount) const override;
		/// @brief Begin a new query.
		/// @param queryPool Query pool to begin a new query in.
		/// @param desc Query desc.
		void BeginQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const override;
		/// @brief End a query.
		/// @param queryPool Query pool to begin a new query in.
		/// @param desc Query desc.
		void EndQuery(const TRAP::Ref<QueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const override;
		/// @brief Retrieve the results of a query.
		/// @param queryPool Query pool containing the query results.
		/// @param readBackBuffer Buffer to write results to.
		/// @param startQuery Initial query index.
		/// @param queryCount Number of queries to read.
		void ResolveQuery(const TRAP::Ref<QueryPool>& queryPool, const TRAP::Ref<Buffer>& readBackBuffer,
		                  u32 startQuery, u32 queryCount) const override;

		/// @brief Add resource barriers (memory dependency) to the command buffer.
		/// @param bufferBarriers Optional buffer barriers.
		/// @param textureBarriers Optional texture barriers.
		/// @param renderTargetBarriers Optional render target barriers.
		void ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* bufferBarriers,
		                     const std::vector<RendererAPI::TextureBarrier>* textureBarriers,
		                     const std::vector<RendererAPI::RenderTargetBarrier>* renderTargetBarriers) const override;
		/// @brief Add a resource barrier (memory dependency) to the command buffer.
		/// @param bufferBarrier Optional buffer barrier.
		/// @param textureBarrier Optional texture barrier.
		/// @param renderTargetBarrier Optional render target barrier.
		void ResourceBarrier(const RendererAPI::BufferBarrier* bufferBarrier,
		                     const RendererAPI::TextureBarrier* textureBarrier,
		                     const RendererAPI::RenderTargetBarrier* renderTargetBarrier) const override;

		/// @brief Set the stencil reference value for the command buffer.
		/// @param val New value to use as stencil reference.
		void SetStencilReferenceValue(u32 val) const override;

		/// @brief Set the pipeline fragment shading rate and combiner operation for the command buffer.
		/// @param shadingRate Shading rate to use.
		/// @param postRasterizerRate Shading rate combiner to use.
		/// @param finalRate Shading rate combiner to use.
		void SetShadingRate(RendererAPI::ShadingRate shadingRate,
		                    RendererAPI::ShadingRateCombiner postRasterizerRate,
							RendererAPI::ShadingRateCombiner finalRate) const override;

		/// @brief Clear the currently used color attachment.
		/// @param color Color to clear the color attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		void Clear(const RendererAPI::Color& color, u32 width, u32 height) const override;
		/// @brief Clear the currently used depth and stencil attachment.
		/// @param depth Depth value to clear the depth attachment with.
		/// @param stencil Stencil value to clear the stencil attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		void Clear(f32 depth, u32 stencil, u32 width, u32 height) const override;
		/// @brief Clear the currently used depth attachment.
		/// @param depth Depth value to clear the depth attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		void Clear(f32 depth, u32 width, u32 height) const override;
		/// @brief Clear the currently used stencil attachment.
		/// @param stencil Stencil value to clear the stencil attachment with.
		/// @param width Width of the area to clear.
		/// @param height Height of the area to clear.
		void Clear(u32 stencil, u32 width, u32 height) const override;

		/// @brief Resolve a multisample color texture to a non-multisample color texture.
		/// @param srcImage Source multisample color texture to resolve.
		/// @param srcState Source texture state.
		/// @param dstImage Destination non-multisample color texture to resolve into.
		/// @param dstState Destination texture state.
		void ResolveImage(const Ref<API::VulkanTexture>& srcImage, RendererAPI::ResourceState srcState,
		                  const Ref<API::VulkanTexture>& dstImage, RendererAPI::ResourceState dstState) const;

		/// @brief Retrieve the currently active VkRenderPass.
		/// @return Currently active VkRenderPass.
		[[nodiscard]] constexpr VkRenderPass GetActiveVkRenderPass() const noexcept;

	private:
		/// @brief Hash the provided data.
		/// @param mem Pointer to data to hash.
		/// @param size Size of data to hash.
		/// @param prev Previous hash value.
		static usize HashAlg(const auto* mem, usize size, const usize prev = 2166136261U)
		{
			ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
			                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

			TRAP_ASSERT(mem, "VulkanCommandBuffer::HashAlg(): mem is nullptr!");
			TRAP_ASSERT(size, "VulkanCommandBuffer::HashAlg(): size is 0!");

			//Intentionally u32 instead of usize, so the behavior is the same regardless of size.
			u32 result = static_cast<u32>(prev);

			while (size--)
				result = static_cast<u32>((result * 16777619) ^ *mem++);

			return NumericCast<usize>(result);
		}

		TRAP::Ref<API::VulkanDevice> m_device;

		VkCommandBuffer m_vkCommandBuffer = VK_NULL_HANDLE;

		VkCommandPool& m_vkCommandPool;
		bool m_secondary;

		VkRenderPass m_activeRenderPass = VK_NULL_HANDLE;
		VkPipelineLayout m_boundPipelineLayout = VK_NULL_HANDLE;
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
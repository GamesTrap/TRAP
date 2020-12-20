#include "TRAPPCH.h"
#include "VulkanCommandBuffer.h"

#include "VulkanTexture.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanBuffer.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRootSignature.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanQueue.h"

TRAP::Graphics::API::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	if(m_vkCommandBuffer)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Destroying CommandBuffer");
#endif
		
		vkFreeCommandBuffers(m_device->GetVkDevice(), m_vkCommandPool, 1, &m_vkCommandBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer::VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device,
                                                              TRAP::Ref<VulkanQueue> queue,
                                                              VkCommandPool& commandPool,
                                                              const bool secondary)
	: m_vkCommandBuffer(VK_NULL_HANDLE),
	  m_device(std::move(device)),
	  m_queue(std::move(queue)),
	  m_vkCommandPool(commandPool),
	  m_secondary(secondary),
	  m_activeRenderPass(VK_NULL_HANDLE),
	  m_boundPipelineLayout(VK_NULL_HANDLE)
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Creating CommandBuffer");
#endif

	VkCommandBufferAllocateInfo info = VulkanInits::CommandBufferAllocateInfo(commandPool, secondary);

	VkCall(vkAllocateCommandBuffers(m_device->GetVkDevice(), &info, &m_vkCommandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBuffer& TRAP::Graphics::API::VulkanCommandBuffer::GetVkCommandBuffer()
{
	return m_vkCommandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanCommandBuffer::GetQueueType() const
{
	return m_queue->GetQueueType();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanCommandBuffer::IsSecondary() const
{
	return m_secondary;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstants(const TRAP::Ref<VulkanRootSignature>& rootSignature,
                                                                 const char* name,
                                                                 const void* constants) const
{
	TRAP_ASSERT(constants);
	TRAP_ASSERT(rootSignature);
	TRAP_ASSERT(name);

	const RendererAPI::DescriptorInfo* desc = rootSignature->GetDescriptor(name);
	TRAP_ASSERT(desc);
	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant);

	vkCmdPushConstants(m_vkCommandBuffer, rootSignature->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstantsByIndex(const TRAP::Ref<VulkanRootSignature>& rootSignature,
	const uint32_t paramIndex,
	const void* constants) const
{
	TRAP_ASSERT(constants);
	TRAP_ASSERT(rootSignature);
	TRAP_ASSERT(paramIndex >= 0 && paramIndex < rootSignature->GetDescriptorCount());

	const RendererAPI::DescriptorInfo* desc = &rootSignature->GetDescriptors()[paramIndex];
	TRAP_ASSERT(desc);
	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant);

	vkCmdPushConstants(m_vkCommandBuffer, rootSignature->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindDescriptorSet(const uint32_t index, VulkanDescriptorSet& descriptorSet)
{
	TRAP_ASSERT(!descriptorSet.GetVkDescriptorSets().empty());
	TRAP_ASSERT(descriptorSet.GetMaxSets() > index);

	const TRAP::Ref<VulkanRootSignature> rootSignature = descriptorSet.GetRootSignature();

	if(m_boundPipelineLayout != rootSignature->GetVkPipelineLayout())
	{
		m_boundPipelineLayout = rootSignature->GetVkPipelineLayout();

		//Vulkan requires to bind all descriptor sets up to the highest set number even if they are empty
		//Example: If shader uses only set 2, we still have to bind empty sets for set = 0 and set = 1
		for(uint32_t setIndex = 0; setIndex < static_cast<uint32_t>(RendererAPI::DescriptorUpdateFrequency::DESCRIPTOR_UPDATE_FREQUENCY_COUNT); ++setIndex)
		{
			if(rootSignature->GetVkEmptyDescriptorSets()[setIndex] != VK_NULL_HANDLE)
				vkCmdBindDescriptorSets(m_vkCommandBuffer,
				                        VkPipelineBindPointTranslator[static_cast<uint32_t>(rootSignature->GetPipelineType())],
				                        rootSignature->GetVkPipelineLayout(),
				                        setIndex,
				                        1,
				                        &rootSignature->GetVkEmptyDescriptorSets()[setIndex],
				                        0,
				                        nullptr);
		}
	}

	vkCmdBindDescriptorSets(m_vkCommandBuffer,
	                        VkPipelineBindPointTranslator[static_cast<uint32_t>(rootSignature->GetPipelineType())],
	                        rootSignature->GetVkPipelineLayout(),
	                        static_cast<uint32_t>(descriptorSet.GetUpdateFrequency()),
	                        1,
	                        &descriptorSet.GetVkDescriptorSets()[index],
	                        descriptorSet.GetDynamicOffsetCount(),
	                        descriptorSet.GetDynamicOffsetCount() ? &descriptorSet.GetDynamicSizeOffsets()[index].Offset : nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindIndexBuffer(const TRAP::Ref<VulkanBuffer>& buffer, const RendererAPI::IndexType indexType, const uint64_t offset) const
{
	TRAP_ASSERT(buffer);
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	const VkIndexType vkIndexType = ((RendererAPI::IndexType::UInt16 == indexType) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	vkCmdBindIndexBuffer(m_vkCommandBuffer, buffer->GetVkBuffer(), offset, vkIndexType);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindVertexBuffer(const std::vector<TRAP::Ref<VulkanBuffer>>& buffers,
                                                                const std::vector<uint32_t>& strides,
                                                                const std::vector<uint64_t>& offsets) const
{
	TRAP_ASSERT(!buffers.empty());
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);
	TRAP_ASSERT(!strides.empty());

	const uint32_t maxBuffers = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.maxVertexInputBindings;
	const uint32_t cappedBufferCount = buffers.size() > maxBuffers ? maxBuffers : static_cast<uint32_t>(buffers.size());

	//No upper bound for this, so use 64 for now
	TRAP_ASSERT(cappedBufferCount < 64);

	std::array<VkBuffer, 64> vkBuffers{};
	std::array<VkDeviceSize, 64> vkOffsets{};

	for(uint32_t i = 0; i < cappedBufferCount; ++i)
	{
		vkBuffers[i] = buffers[i]->GetVkBuffer();
		vkOffsets[i] = (offsets.size() > i) ? offsets[i] : 0;
	}

	vkCmdBindVertexBuffers(m_vkCommandBuffer, 0, cappedBufferCount, vkBuffers.data(), vkOffsets.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::AddDebugMarker(const float r, const float g, const float b, const char* name) const
{
	VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(r, g, b, name);

	vkCmdInsertDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginDebugMarker(const float r, const float g, const float b, const char* name) const
{
	VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(r, g, b, name);

	vkCmdBeginDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndDebugMarker() const
{
	vkCmdEndDebugUtilsLabelEXT(m_vkCommandBuffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Begin()
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	VkCommandBufferBeginInfo beginInfo = VulkanInits::CommandBufferBeginInfo();

	VkCall(vkBeginCommandBuffer(m_vkCommandBuffer, &beginInfo));

	//Reset CPU side data
	m_boundPipelineLayout = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::End()
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	if (m_activeRenderPass)
		vkCmdEndRenderPass(m_vkCommandBuffer);
	m_activeRenderPass = VK_NULL_HANDLE;

	VkCall(vkEndCommandBuffer(m_vkCommandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetViewport(const float x,
                                                           const float y,
                                                           const float width,
                                                           const float height,
                                                           const float minDepth,
                                                           const float maxDepth) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	VkViewport viewport{};
	viewport.x = x;
	viewport.y = y + height;
	viewport.width = width;
	viewport.height = -height;
	viewport.minDepth = minDepth;
	viewport.maxDepth = maxDepth;

	vkCmdSetViewport(m_vkCommandBuffer, 0, 1, &viewport);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetScissor(const uint32_t x,
                                                          const uint32_t y,
                                                          const uint32_t width,
                                                          const uint32_t height) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	VkRect2D rect{};
	rect.offset.x = x;
	rect.offset.y = y;
	rect.extent.width = width;
	rect.extent.height = height;

	vkCmdSetScissor(m_vkCommandBuffer, 0, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Draw(const uint32_t vertexCount, const uint32_t firstVertex) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, 1, firstVertex, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawInstances(const uint32_t vertexCount,
                                                             const uint32_t firstVertex,
                                                             const uint32_t instanceCount,
                                                             const uint32_t firstInstance) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexed(const uint32_t indexCount,
                                                           const uint32_t firstIndex,
                                                           const uint32_t firstVertex) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, 1, firstIndex, firstVertex, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexedInstanced(const uint32_t indexCount,
                                                                    const uint32_t firstIndex,
                                                                    const uint32_t instanceCount,
                                                                    const uint32_t firstInstance,
                                                                    const uint32_t firstVertex) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, instanceCount, firstIndex, firstVertex, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Dispatch(const uint32_t groupCountX,
                                                        const uint32_t groupCountY,
                                                        const uint32_t groupCountZ) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDispatch(m_vkCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateBuffer(const TRAP::Ref<VulkanBuffer>& buffer,
                                                            const uint64_t dstOffset,
                                                            const TRAP::Ref<VulkanBuffer>& srcBuffer,
                                                            const uint64_t srcOffset,
                                                            const uint64_t size) const
{
	TRAP_ASSERT(srcBuffer);
	TRAP_ASSERT(srcBuffer->GetVkBuffer());
	TRAP_ASSERT(buffer);
	TRAP_ASSERT(buffer->GetVkBuffer());
	TRAP_ASSERT(srcOffset + size <= srcBuffer->GetSize());
	TRAP_ASSERT(dstOffset + size <= buffer->GetSize());

	VkBufferCopy region{};
	region.srcOffset = srcOffset;
	region.dstOffset = dstOffset;
	region.size = static_cast<VkDeviceSize>(size);

	vkCmdCopyBuffer(m_vkCommandBuffer, srcBuffer->GetVkBuffer(), buffer->GetVkBuffer(), 1, &region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateSubresource(const TRAP::Ref<VulkanTexture>& texture,
                                                                 const TRAP::Ref<VulkanBuffer>& srcBuffer,
                                                                 const VulkanRenderer::SubresourceDesc& subresourceDesc) const
{
	const uint32_t width = TRAP::Math::Max(1u, texture->GetWidth() >> subresourceDesc.MipLevel);
	const uint32_t height = TRAP::Math::Max(1u, texture->GetHeight() >> subresourceDesc.MipLevel);
	const uint32_t depth = TRAP::Math::Max(1u, texture->GetDepth() >> subresourceDesc.MipLevel);
	const RendererAPI::ImageFormat format = texture->GetImageFormat();
	const uint32_t numBlocksWide = subresourceDesc.RowPitch / (RendererAPI::ImageFormatBitSizeOfBlock(format) >> 3);
	const uint32_t numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);

	VkBufferImageCopy copy{};
	copy.bufferOffset = subresourceDesc.SrcOffset;
	copy.bufferRowLength = numBlocksWide * RendererAPI::ImageFormatWidthOfBlock(format);
	copy.bufferImageHeight = numBlocksHigh * RendererAPI::ImageFormatHeightOfBlock(format);
	copy.imageSubresource.aspectMask = static_cast<VkImageAspectFlags>(texture->GetAspectMask());
	copy.imageSubresource.mipLevel = subresourceDesc.MipLevel;
	copy.imageSubresource.baseArrayLayer = subresourceDesc.ArrayLayer;
	copy.imageSubresource.layerCount = 1;
	copy.imageOffset.x = 0;
	copy.imageOffset.y = 0;
	copy.imageOffset.z = 0;
	copy.imageExtent.width = width;
	copy.imageExtent.height = height;
	copy.imageExtent.depth = depth;

	vkCmdCopyBufferToImage(m_vkCommandBuffer, srcBuffer->GetVkBuffer(), texture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
}
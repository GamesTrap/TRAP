#include "TRAPPCH.h"
#include "VulkanCommandBuffer.h"

#include "VulkanFrameBuffer.h"
#include "VulkanRenderTarget.h"
#include "VulkanPipeline.h"
#include "VulkanCommandSignature.h"
#include "VulkanQueryPool.h"
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

TRAP::Ref<TRAP::Graphics::API::VulkanQueue> TRAP::Graphics::API::VulkanCommandBuffer::GetQueue() const
{
	return m_queue;
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

void TRAP::Graphics::API::VulkanCommandBuffer::BindPipeline(const TRAP::Ref<VulkanPipeline>& pipeline) const
{
	TRAP_ASSERT(pipeline);
	TRAP_ASSERT(m_vkCommandBuffer);

	const VkPipelineBindPoint pipelineBindPoint = VkPipelineBindPointTranslator[static_cast<uint32_t>(pipeline->GetPipelineType())];
	vkCmdBindPipeline(m_vkCommandBuffer, pipelineBindPoint, pipeline->GetVkPipeline());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<VulkanRenderTarget>>& renderTargets,
																 const TRAP::Ref<VulkanRenderTarget>& depthStencil,
																 const RendererAPI::LoadActionsDesc* loadActions,
																 const std::vector<uint32_t>& colorArraySlices,
																 const std::vector<uint32_t>& colorMipSlices,
																 uint32_t depthArraySlice,
																 uint32_t depthMipSlice)
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	if(m_activeRenderPass)
	{
		vkCmdEndRenderPass(m_vkCommandBuffer);
		m_activeRenderPass = VK_NULL_HANDLE;
	}

	if (renderTargets.empty() && !depthStencil)
		return;

	std::size_t renderPassHash = 0;
	std::size_t frameBufferHash = 0;

	std::vector<RendererAPI::RenderTargetBarrier> barriers;
	barriers.reserve(8 + 1);
	
	//Generate hash for RenderPass and FrameBuffer
	//NOTE:
	//RenderPass does not care about underlying VkImageView.
	//It only cares about the format and sample count of the attachments.
	//We hash those two values to generate the RenderPass Hash.
	//FrameBuffer is the actual array of all the VkImageViews
	//We hash the texture id associated with the RenderTarget to generate the FrameBuffer Hash.
	for(uint32_t i = 0; i < renderTargets.size(); ++i)
	{
		std::array<uint32_t, 3> hashValues =
		{
			static_cast<uint32_t>(renderTargets[i]->GetImageFormat()),
			static_cast<uint32_t>(renderTargets[i]->GetSampleCount()),
			loadActions ? static_cast<uint32_t>(loadActions->LoadActionsColor[i]) : 0
		};
		
		renderPassHash = HashAlg<uint32_t>(hashValues.data(), 3, renderPassHash);
		const uint32_t ID = renderTargets[i]->GetID();
		frameBufferHash = HashAlg<uint32_t>(&ID, 1, frameBufferHash);

		if(0 == renderTargets[i]->m_used++)
			barriers.push_back({ renderTargets[i], RendererAPI::ResourceState::Undefined, RendererAPI::ResourceState::RenderTarget });
	}
	if(depthStencil)
	{
		std::array<uint32_t, 4> hashValues =
		{
			static_cast<uint32_t>(depthStencil->GetImageFormat()),
			static_cast<uint32_t>(depthStencil->GetSampleCount()),
			loadActions ? static_cast<uint32_t>(loadActions->LoadActionDepth) : 0,
			loadActions ? static_cast<uint32_t>(loadActions->LoadActionStencil) : 0
		};
		renderPassHash = HashAlg<uint32_t>(hashValues.data(), 4, renderPassHash);
		const uint32_t ID = depthStencil->GetID();
		frameBufferHash = HashAlg<uint32_t>(&ID, 1, frameBufferHash);

		if (0 == depthStencil->m_used++)
			barriers.push_back({ depthStencil, RendererAPI::ResourceState::Undefined, RendererAPI::ResourceState::DepthWrite });
	}
	if (!colorArraySlices.empty())
		frameBufferHash = HashAlg<uint32_t>(colorArraySlices.data(), renderTargets.size(), frameBufferHash);
	if (!colorMipSlices.empty())
		frameBufferHash = HashAlg<uint32_t>(colorMipSlices.data(), renderTargets.size(), frameBufferHash);
	if (depthArraySlice != -1)
		frameBufferHash = HashAlg<uint32_t>(&depthArraySlice, 1, frameBufferHash);
	if (depthMipSlice != -1)
		frameBufferHash = HashAlg<uint32_t>(&depthMipSlice, 1, frameBufferHash);

	RendererAPI::SampleCount sampleCount = RendererAPI::SampleCount::SampleCount1;

	VulkanRenderer::RenderPassMap& renderPassMap = VulkanRenderer::GetRenderPassMap();
	VulkanRenderer::FrameBufferMap& frameBufferMap = VulkanRenderer::GetFrameBufferMap();

	const VulkanRenderer::RenderPassMapIt node = renderPassMap.find(renderPassHash);
	const VulkanRenderer::FrameBufferMapIt frameBufferNode = frameBufferMap.find(frameBufferHash);

	TRAP::Ref<VulkanRenderPass> renderPass = nullptr;
	TRAP::Ref<VulkanFrameBuffer> frameBuffer = nullptr;

	//If a RenderPass of this combination already exists just use it or create a new one
	if (node != renderPassMap.end())
		renderPass = node->second;
	else
	{
		std::array<RendererAPI::ImageFormat, 8> colorFormats{};
		RendererAPI::ImageFormat depthStencilFormat = RendererAPI::ImageFormat::Undefined;
		for (uint32_t i = 0; i < renderTargets.size(); ++i)
			colorFormats[i] = renderTargets[i]->GetImageFormat();
		if (depthStencil)
		{
			depthStencilFormat = depthStencil->GetImageFormat();
			sampleCount = depthStencil->GetSampleCount();
		}
		else if (!renderTargets.empty())
			sampleCount = renderTargets[0]->GetSampleCount();

		VulkanRenderer::RenderPassDesc desc{};
		desc.RenderTargetCount = static_cast<uint32_t>(renderTargets.size());
		desc.SampleCount = sampleCount;
		desc.ColorFormats = std::vector<RendererAPI::ImageFormat>(colorFormats.begin(), colorFormats.end());
		desc.DepthStencilFormat = depthStencilFormat;
		desc.LoadActionsColor = loadActions ? std::vector<RendererAPI::LoadActionType>(loadActions->LoadActionsColor.begin(), loadActions->LoadActionsColor.end()) : std::vector<RendererAPI::LoadActionType>();
		desc.LoadActionDepth = loadActions ? loadActions->LoadActionDepth : RendererAPI::LoadActionType::DontCare;
		desc.LoadActionStencil = loadActions ? loadActions->LoadActionStencil : RendererAPI::LoadActionType::DontCare;
		renderPass = TRAP::MakeRef<VulkanRenderPass>(m_device, desc);

		//No need of a lock here since this map is per thread
		renderPassMap.insert({ {renderPassHash, renderPass} });
	}

	//If a FrameBuffer of this combination already exists just use it or create a new one
	if (frameBufferNode != frameBufferMap.end())
		frameBuffer = frameBufferNode->second;
	else
	{
		VulkanRenderer::FrameBufferDesc desc{};
		desc.RenderTargets = renderTargets;
		desc.DepthStencil = depthStencil;
		desc.RenderPass = renderPass;
		desc.ColorArraySlices = colorArraySlices;
		desc.ColorMipSlices = colorMipSlices;
		desc.DepthArraySlice = depthArraySlice;
		desc.DepthMipSlice = depthMipSlice;
		frameBuffer = TRAP::MakeRef<VulkanFrameBuffer>(m_device, desc);

		//No need of a lock here since this map is per thread
		frameBufferMap.insert({ {frameBufferHash, frameBuffer} });
	}

	VkRect2D renderArea{};
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent.width = frameBuffer->GetWidth();
	renderArea.extent.height = frameBuffer->GetHeight();

	std::vector<VkClearValue> clearValues;
	clearValues.reserve(8 + 1);
	if(loadActions)
	{
		for(uint32_t i = 0; i < renderTargets.size(); ++i)
		{
			RendererAPI::ClearValue clearValue = loadActions->ClearColorValues[i];
			VkClearValue val{};
			val.color = { {clearValue.R, clearValue.G, clearValue.B, clearValue.A} };
			clearValues.push_back(val);
		}
		if(depthStencil)
		{
			VkClearValue val{};
			val.depthStencil = { loadActions->ClearDepth.Depth, loadActions->ClearDepth.Stencil };
			clearValues.push_back(val);
		}
	}

	if (!barriers.empty())
		ResourceBarrier({}, {}, barriers);

	VkRenderPassBeginInfo beginInfo = VulkanInits::RenderPassBeginInfo(renderPass->GetVkRenderPass(), frameBuffer->GetVkFrameBuffer(), renderArea, clearValues);

	vkCmdBeginRenderPass(m_vkCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_activeRenderPass = renderPass->GetVkRenderPass();
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

void TRAP::Graphics::API::VulkanCommandBuffer::ExecuteIndirect(const TRAP::Ref<VulkanCommandSignature>& cmdSignature,
                                                               const uint32_t maxCommandCount,
                                                               const TRAP::Ref<VulkanBuffer>& indirectBuffer,
                                                               const uint64_t bufferOffset,
                                                               const TRAP::Ref<VulkanBuffer>& counterBuffer,
                                                               const uint64_t counterBufferOffset) const
{
	if (cmdSignature->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDraw)
	{
		if (counterBuffer)
		{
			vkCmdDrawIndirectCount(m_vkCommandBuffer,
				indirectBuffer->GetVkBuffer(),
				bufferOffset,
				counterBuffer->GetVkBuffer(),
				counterBufferOffset,
				maxCommandCount,
				cmdSignature->GetStride());
		}
		else
			vkCmdDrawIndirect(m_vkCommandBuffer, indirectBuffer->GetVkBuffer(), bufferOffset, maxCommandCount, cmdSignature->GetStride());
	}
	else if (cmdSignature->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDrawIndex)
	{
		if (counterBuffer)
		{
			vkCmdDrawIndexedIndirectCount(m_vkCommandBuffer,
				indirectBuffer->GetVkBuffer(),
				bufferOffset,
				counterBuffer->GetVkBuffer(),
				counterBufferOffset,
				maxCommandCount,
				cmdSignature->GetStride());
		}
		else
			vkCmdDrawIndexedIndirect(m_vkCommandBuffer, indirectBuffer->GetVkBuffer(), bufferOffset, maxCommandCount, cmdSignature->GetStride());
	}
	else if (cmdSignature->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDispatch)
		vkCmdDispatchIndirect(m_vkCommandBuffer, indirectBuffer->GetVkBuffer(), bufferOffset);
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
	const RendererAPI::ImageFormat fmt = texture->GetImageFormat();
	if(RendererAPI::ImageFormatIsSinglePlane(fmt))
	{
		const uint32_t width = TRAP::Math::Max<uint32_t>(1u, texture->GetWidth() >> subresourceDesc.MipLevel);
		const uint32_t height = TRAP::Math::Max<uint32_t>(1u, texture->GetHeight() >> subresourceDesc.MipLevel);
		const uint32_t depth = TRAP::Math::Max<uint32_t>(1u, texture->GetDepth() >> subresourceDesc.MipLevel);
		const uint32_t numBlocksWide = subresourceDesc.RowPitch / (RendererAPI::ImageFormatBitSizeOfBlock(fmt) >> 3);
		const uint32_t numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);
		
		VkBufferImageCopy copy{};
		copy.bufferOffset = subresourceDesc.SrcOffset;
		copy.bufferRowLength = numBlocksWide * RendererAPI::ImageFormatWidthOfBlock(fmt);
		copy.bufferImageHeight = numBlocksHigh * RendererAPI::ImageFormatHeightOfBlock(fmt);
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
	else
	{
		const uint32_t width = texture->GetWidth();
		const uint32_t height = texture->GetHeight();
		const uint32_t depth = texture->GetDepth();
		const uint32_t numOfPlanes = RendererAPI::ImageFormatNumOfPlanes(fmt);

		uint64_t offset = subresourceDesc.SrcOffset;
		std::vector<VkBufferImageCopy> bufferImagesCopy(3);

		for(uint32_t i = 0; i < numOfPlanes; ++i)
		{
			VkBufferImageCopy& copy = bufferImagesCopy[i];
			copy.bufferOffset = offset;
			copy.bufferRowLength = 0;
			copy.bufferImageHeight = 0;
			copy.imageSubresource.aspectMask = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_PLANE_0_BIT << i);
			copy.imageSubresource.mipLevel = subresourceDesc.MipLevel;
			copy.imageSubresource.baseArrayLayer = subresourceDesc.ArrayLayer;
			copy.imageSubresource.layerCount = 1;
			copy.imageOffset.x = 0;
			copy.imageOffset.y = 0;
			copy.imageOffset.z = 0;
			copy.imageExtent.width = RendererAPI::ImageFormatPlaneWidth(fmt, i, width);
			copy.imageExtent.height = RendererAPI::ImageFormatPlaneHeight(fmt, i, height);
			copy.imageExtent.depth = depth;
			offset += copy.imageExtent.width * copy.imageExtent.height * RendererAPI::ImageFormatPlaneSizeOfBlock(fmt, i);
		}

		vkCmdCopyBufferToImage(m_vkCommandBuffer, srcBuffer->GetVkBuffer(), texture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, numOfPlanes, bufferImagesCopy.data());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateVirtualTexture(const TRAP::Ref<VulkanTexture>& virtualTexture)
{
	if(virtualTexture->GetSVT()->Visibility)
	{
		virtualTexture->ReleasePage();
		virtualTexture->FillVirtualTexture(*this);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResetQueryPool(const TRAP::Ref<VulkanQueryPool>& queryPool,
                                                              const uint32_t startQuery,
                                                              const uint32_t queryCount) const
{
	vkCmdResetQueryPool(m_vkCommandBuffer, queryPool->GetVkQueryPool(), startQuery, queryCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginQuery(const TRAP::Ref<VulkanQueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const
{
	const VkQueryType type = queryPool->GetVkQueryType();
	switch(type)
	{
	case VK_QUERY_TYPE_TIMESTAMP:
		vkCmdWriteTimestamp(m_vkCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPool->GetVkQueryPool(), desc.Index);
		break;

	case VK_QUERY_TYPE_PIPELINE_STATISTICS:
		break;

	case VK_QUERY_TYPE_OCCLUSION:
		break;
		
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndQuery(const TRAP::Ref<VulkanQueryPool>& queryPool, const RendererAPI::QueryDesc& desc) const
{
	BeginQuery(queryPool, desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResolveQuery(const TRAP::Ref<VulkanQueryPool>& queryPool,
                                                            const TRAP::Ref<VulkanBuffer>& readBackBuffer,
                                                            const uint32_t startQuery,
                                                            const uint32_t queryCount) const
{
	VkQueryResultFlags flags = VK_QUERY_RESULT_64_BIT;
	flags |= VK_QUERY_RESULT_WAIT_BIT;

	vkCmdCopyQueryPoolResults(m_vkCommandBuffer, queryPool->GetVkQueryPool(), startQuery, queryCount, readBackBuffer->GetVkBuffer(), 0, sizeof(uint64_t), flags);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>& bufferBarriers,
	const std::vector<RendererAPI::TextureBarrier>& textureBarriers,
	const std::vector<RendererAPI::RenderTargetBarrier>& renderTargetBarriers) const
{
	std::vector<VkImageMemoryBarrier> iBarriers;
	if (!textureBarriers.empty() || !renderTargetBarriers.empty())
		iBarriers.resize(textureBarriers.size() + renderTargetBarriers.size());
	uint32_t imageBarrierCount = 0;

	std::vector<VkBufferMemoryBarrier> bBarriers;
	if (!bufferBarriers.empty())
		bBarriers.resize(bufferBarriers.size());
	uint32_t bufferBarrierCount = 0;

	VkAccessFlags srcAccessFlags = 0;
	VkAccessFlags dstAccessFlags = 0;

	for (const auto& trans : bufferBarriers)
	{
		const TRAP::Ref<VulkanBuffer>& buffer = trans.Buffer;
		VkBufferMemoryBarrier* bufferBarrier;

		if (trans.CurrentState == RendererAPI::ResourceState::UnorderedAccess &&
			trans.NewState == RendererAPI::ResourceState::UnorderedAccess)
		{
			bufferBarrier = &bBarriers[bufferBarrierCount++];
			bufferBarrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferBarrier->pNext = nullptr;

			bufferBarrier->srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			bufferBarrier->dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		}
		else
		{
			bufferBarrier = &bBarriers[bufferBarrierCount++];
			bufferBarrier->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferBarrier->pNext = nullptr;

			bufferBarrier->srcAccessMask = ResourceStateToVkAccessFlags(trans.CurrentState);
			bufferBarrier->dstAccessMask = ResourceStateToVkAccessFlags(trans.NewState);
		}

		if(bufferBarrier)
		{
			bufferBarrier->buffer = buffer->GetVkBuffer();
			bufferBarrier->size = VK_WHOLE_SIZE;
			bufferBarrier->offset = 0;

			if(trans.Acquire)
			{
				bufferBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
				bufferBarrier->dstQueueFamilyIndex = m_queue->GetQueueFamilyIndex();
			}
			else if(trans.Release)
			{
				bufferBarrier->srcQueueFamilyIndex = m_queue->GetQueueFamilyIndex();
				bufferBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
			}
			else
			{
				bufferBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				bufferBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			}

			srcAccessFlags |= bufferBarrier->srcAccessMask;
			dstAccessFlags |= bufferBarrier->dstAccessMask;
		}
	}

	for (const auto& trans : textureBarriers)
	{
		const TRAP::Ref<VulkanTexture>& texture = trans.Texture;
		VkImageMemoryBarrier* imageBarrier;

		if (trans.CurrentState == RendererAPI::ResourceState::UnorderedAccess &&
			trans.NewState == RendererAPI::ResourceState::UnorderedAccess)
		{
			imageBarrier = &iBarriers[imageBarrierCount++];
			imageBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier->pNext = nullptr;

			imageBarrier->srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			imageBarrier->dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
			imageBarrier->oldLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageBarrier->newLayout = VK_IMAGE_LAYOUT_GENERAL;
		}
		else
		{
			imageBarrier = &iBarriers[imageBarrierCount++];
			imageBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier->pNext = nullptr;

			imageBarrier->srcAccessMask = ResourceStateToVkAccessFlags(trans.CurrentState);
			imageBarrier->dstAccessMask = ResourceStateToVkAccessFlags(trans.NewState);
			imageBarrier->oldLayout = ResourceStateToVkImageLayout(trans.CurrentState);
			imageBarrier->newLayout = ResourceStateToVkImageLayout(trans.NewState);
		}

		if(imageBarrier)
		{
			imageBarrier->image = texture->GetVkImage();
			imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
			imageBarrier->subresourceRange.baseMipLevel = trans.SubresourceBarrier ? trans.MipLevel : 0;
			imageBarrier->subresourceRange.levelCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
			imageBarrier->subresourceRange.baseArrayLayer = trans.SubresourceBarrier ? trans.ArrayLayer : 0;
			imageBarrier->subresourceRange.layerCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

			if(trans.Acquire)
			{
				imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
				imageBarrier->dstQueueFamilyIndex = m_queue->GetQueueFamilyIndex();
			}
			else if(trans.Release)
			{
				imageBarrier->srcQueueFamilyIndex = m_queue->GetQueueFamilyIndex();
				imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
			}
			else
			{
				imageBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			}

			srcAccessFlags |= imageBarrier->srcAccessMask;
			dstAccessFlags |= imageBarrier->dstAccessMask;
		}
	}

	for (const auto& trans : renderTargetBarriers)
	{
		const TRAP::Ref<VulkanTexture> texture = trans.RenderTarget->m_texture;
		VkImageMemoryBarrier* imageBarrier;

		if (trans.CurrentState == RendererAPI::ResourceState::UnorderedAccess &&
			trans.NewState == RendererAPI::ResourceState::UnorderedAccess)
		{
			imageBarrier = &iBarriers[imageBarrierCount++];
			imageBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier->pNext = nullptr;

			imageBarrier->srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			imageBarrier->dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
			imageBarrier->oldLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageBarrier->newLayout = VK_IMAGE_LAYOUT_GENERAL;
		}
		else
		{
			imageBarrier = &iBarriers[imageBarrierCount++];
			imageBarrier->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier->pNext = nullptr;

			imageBarrier->srcAccessMask = ResourceStateToVkAccessFlags(trans.CurrentState);
			imageBarrier->dstAccessMask = ResourceStateToVkAccessFlags(trans.NewState);
			imageBarrier->oldLayout = trans.RenderTarget->m_used++ ? ResourceStateToVkImageLayout(trans.CurrentState) : VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier->newLayout = ResourceStateToVkImageLayout(trans.NewState);
		}

		if(imageBarrier)
		{
			imageBarrier->image = texture->GetVkImage();
			imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
			imageBarrier->subresourceRange.baseMipLevel = trans.SubresourceBarrier ? trans.MipLevel : 0;
			imageBarrier->subresourceRange.levelCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
			imageBarrier->subresourceRange.baseArrayLayer = trans.SubresourceBarrier ? trans.ArrayLayer : 0;
			imageBarrier->subresourceRange.layerCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

			if(trans.Acquire)
			{
				imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
				imageBarrier->dstQueueFamilyIndex = m_queue->GetQueueFamilyIndex();
			}
			else if(trans.Release)
			{
				imageBarrier->srcQueueFamilyIndex = m_queue->GetQueueFamilyIndex();
				imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
			}
			else
			{
				imageBarrier->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			}

			srcAccessFlags |= imageBarrier->srcAccessMask;
			dstAccessFlags |= imageBarrier->dstAccessMask;
		}
	}

	const VkPipelineStageFlags srcStageMask = DetermineVkPipelineStageFlags(srcAccessFlags, m_queue->GetQueueType());
	const VkPipelineStageFlags dstStageMask = DetermineVkPipelineStageFlags(dstAccessFlags, m_queue->GetQueueType());

	if(bufferBarrierCount || imageBarrierCount)
		vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, bufferBarrierCount, bBarriers.data(), imageBarrierCount, iBarriers.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetStencilReferenceValue(const uint32_t val) const
{
	vkCmdSetStencilReference(m_vkCommandBuffer, VK_STENCIL_FRONT_AND_BACK, val);
}
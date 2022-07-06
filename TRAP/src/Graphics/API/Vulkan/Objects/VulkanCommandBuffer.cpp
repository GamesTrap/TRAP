#include "TRAPPCH.h"
#include "VulkanCommandBuffer.h"

#include "VulkanFrameBuffer.h"
#include "VulkanRenderTarget.h"
#include "VulkanPipeline.h"
#include "VulkanCommandSignature.h"
#include "VulkanQueryPool.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanBuffer.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRootSignature.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanQueue.h"
#include "VulkanTexture.h"

TRAP::Graphics::API::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	TRAP_ASSERT(m_vkCommandBuffer);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Destroying CommandBuffer");
#endif

	vkFreeCommandBuffers(m_device->GetVkDevice(), m_vkCommandPool, 1, &m_vkCommandBuffer);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer::VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<Queue> queue,
                                                              VkCommandPool& commandPool, const bool secondary)
	: m_device(std::move(device)),
	  m_vkCommandBuffer(VK_NULL_HANDLE),
	  m_vkCommandPool(commandPool),
	  m_secondary(secondary),
	  m_activeRenderPass(VK_NULL_HANDLE),
	  m_boundPipelineLayout(VK_NULL_HANDLE)
{
	m_queue = std::move(queue);

	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Creating CommandBuffer");
#endif

	VkCommandBufferAllocateInfo info = VulkanInits::CommandBufferAllocateInfo(commandPool, secondary);

	VkCall(vkAllocateCommandBuffers(m_device->GetVkDevice(), &info, &m_vkCommandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------//

VkCommandBuffer TRAP::Graphics::API::VulkanCommandBuffer::GetVkCommandBuffer() const
{
	return m_vkCommandBuffer;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanCommandBuffer::GetQueueType() const
{
	return dynamic_cast<VulkanQueue*>(m_queue.get())->GetQueueType();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanCommandBuffer::IsSecondary() const
{
	return m_secondary;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature,
                                                                 const char* name, const void* constants) const
{
	TRAP_ASSERT(constants);
	TRAP_ASSERT(rootSignature);
	TRAP_ASSERT(name);

	VulkanRootSignature* rSig = dynamic_cast<VulkanRootSignature*>(rootSignature.get());

	const RendererAPI::DescriptorInfo* desc = rSig->GetDescriptor(name);

	if(!desc)
	{
		TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Unable to find PushConstants with name: ", name, "!");
		return;
	}

	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant);

	vkCmdPushConstants(m_vkCommandBuffer, rSig->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature,
	                                                                    const uint32_t paramIndex,
																		const void* constants) const
{
	TRAP_ASSERT(constants);
	TRAP_ASSERT(rootSignature);
	TRAP_ASSERT(paramIndex < rootSignature->GetDescriptorCount());

	const RendererAPI::DescriptorInfo* desc = &rootSignature->GetDescriptors()[paramIndex];

	if(!desc)
	{
		TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Unable to find PushConstants with RootSignature index: ", paramIndex, "!");
		return;
	}

	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant);

	vkCmdPushConstants(m_vkCommandBuffer,
	                   dynamic_cast<VulkanRootSignature*>(rootSignature.get())->GetVkPipelineLayout(),
					   desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindDescriptorSet(const uint32_t index, DescriptorSet& descriptorSet)
{
	VulkanDescriptorSet* dSet = dynamic_cast<VulkanDescriptorSet*>(&descriptorSet);
	TRAP_ASSERT(!dSet->GetVkDescriptorSets().empty());
	TRAP_ASSERT(dSet->GetMaxSets() > index);

	const TRAP::Ref<VulkanRootSignature> rootSignature = dSet->GetRootSignature();

	if(m_boundPipelineLayout != rootSignature->GetVkPipelineLayout())
	{
		m_boundPipelineLayout = rootSignature->GetVkPipelineLayout();

		//Vulkan requires to bind all descriptor sets up to the highest set number even if they are empty
		//Example: If shader uses only set 2, we still have to bind empty sets for set = 0 and set = 1
		for(uint32_t setIndex = 0; setIndex < RendererAPI::MaxDescriptorSets; ++setIndex)
		{
			if(rootSignature->GetVkEmptyDescriptorSets()[setIndex] != VK_NULL_HANDLE)
				vkCmdBindDescriptorSets(m_vkCommandBuffer,
				                        VkPipelineBindPointTranslator[static_cast<uint32_t>(rootSignature->GetPipelineType())],
				                        rootSignature->GetVkPipelineLayout(), setIndex, 1,
				                        &rootSignature->GetVkEmptyDescriptorSets()[setIndex], 0, nullptr);
		}
	}

	vkCmdBindDescriptorSets(m_vkCommandBuffer,
	                        VkPipelineBindPointTranslator[static_cast<uint32_t>(rootSignature->GetPipelineType())],
	                        rootSignature->GetVkPipelineLayout(), dSet->GetSet(),
	                        1, &dSet->GetVkDescriptorSets()[index], dSet->GetDynamicOffsetCount(),
							dSet->GetDynamicOffsetCount() ? &dSet->GetDynamicSizeOffsets()[index].Offset : nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindIndexBuffer(const TRAP::Ref<Buffer>& buffer,
                                                               const RendererAPI::IndexType indexType,
															   const uint64_t offset) const
{
	TRAP_ASSERT(buffer);
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	const VkIndexType vkIndexType = ((RendererAPI::IndexType::UInt16 == indexType) ? VK_INDEX_TYPE_UINT16 :
	                                                                                 VK_INDEX_TYPE_UINT32);
	vkCmdBindIndexBuffer(m_vkCommandBuffer, dynamic_cast<VulkanBuffer*>(buffer.get())->GetVkBuffer(), offset,
	                     vkIndexType);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers,
                                                                const std::vector<uint32_t>& strides,
                                                                const std::vector<uint64_t>& offsets) const
{
	TRAP_ASSERT(!buffers.empty());
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);
	TRAP_ASSERT(!strides.empty());

	const uint32_t maxBuffers = RendererAPI::GPUSettings.MaxVertexInputBindings;
	const uint32_t cappedBufferCount = buffers.size() > maxBuffers ? maxBuffers :
	                                                                 static_cast<uint32_t>(buffers.size());

	//Capped the number of buffers to the maximum allowed by the device
	std::vector<VkBuffer> vkBuffers(cappedBufferCount);
	std::vector<VkDeviceSize> vkOffsets(cappedBufferCount);

	for(uint32_t i = 0; i < cappedBufferCount; ++i)
	{
		vkBuffers[i] = dynamic_cast<VulkanBuffer*>(buffers[i].get())->GetVkBuffer();
		vkOffsets[i] = (offsets.size() > i) ? offsets[i] : 0;
	}

	vkCmdBindVertexBuffers(m_vkCommandBuffer, 0, cappedBufferCount, vkBuffers.data(), vkOffsets.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const
{
	TRAP_ASSERT(pipeline);
	TRAP_ASSERT(m_vkCommandBuffer);

	VulkanPipeline* pipe = dynamic_cast<VulkanPipeline*>(pipeline.get());
	const VkPipelineBindPoint pipelineBindPoint = VkPipelineBindPointTranslator[static_cast<uint32_t>(pipe->GetPipelineType())];
	vkCmdBindPipeline(m_vkCommandBuffer, pipelineBindPoint, pipe->GetVkPipeline());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
																 const TRAP::Ref<RenderTarget>& depthStencil,
																 const RendererAPI::LoadActionsDesc* loadActions,
																 std::vector<uint32_t>* colorArraySlices,
																 std::vector<uint32_t>* colorMipSlices,
																 uint32_t depthArraySlice, uint32_t depthMipSlice)
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
	std::vector<RendererAPI::StoreActionType> colorStoreActions(8);
	RendererAPI::StoreActionType depthStoreAction{};
	RendererAPI::StoreActionType stencilStoreAction{};

	//Generate hash for RenderPass and FrameBuffer
	//NOTE:
	//RenderPass does not care about underlying VkImageView.
	//It only cares about the format and sample count of the attachments.
	//We hash those two values to generate the RenderPass Hash.
	//FrameBuffer is the actual array of all the VkImageViews
	//We hash the texture id associated with the RenderTarget to generate the FrameBuffer Hash.
	for(std::size_t i = 0; i < renderTargets.size(); ++i)
	{
		const VulkanTexture* vkTex = dynamic_cast<VulkanTexture*>(renderTargets[i]->GetTexture());
		if(vkTex->IsLazilyAllocated())
			colorStoreActions[i] = RendererAPI::StoreActionType::DontCare;
		else if(loadActions)
			colorStoreActions[i] = loadActions->StoreActionsColor[i];

		std::array<uint32_t, 4> hashValues =
		{
			static_cast<uint32_t>(renderTargets[i]->GetImageFormat()),
			static_cast<uint32_t>(renderTargets[i]->GetSampleCount()),
			loadActions ? static_cast<uint32_t>(loadActions->LoadActionsColor[i]) : 0,
			static_cast<uint32_t>(colorStoreActions[i])
		};

		renderPassHash = HashAlg<uint32_t>(hashValues.data(), 4, renderPassHash);
		const uint32_t ID = dynamic_cast<VulkanRenderTarget*>(renderTargets[i].get())->GetID();
		frameBufferHash = HashAlg<uint32_t>(&ID, 1, frameBufferHash);
	}
	if(depthStencil)
	{
		VulkanRenderTarget* dStencil = dynamic_cast<VulkanRenderTarget*>(depthStencil.get());
		const VulkanTexture* vkTex = dynamic_cast<VulkanTexture*>(dStencil->GetTexture());

		if(vkTex->IsLazilyAllocated())
		{
			depthStoreAction = RendererAPI::StoreActionType::DontCare;
			stencilStoreAction = RendererAPI::StoreActionType::DontCare;
		}
		else if(loadActions)
		{
			depthStoreAction = loadActions->StoreActionDepth;
			stencilStoreAction = loadActions->StoreActionStencil;
		}

		std::array<uint32_t, 6> hashValues =
		{
			static_cast<uint32_t>(dStencil->GetImageFormat()),
			static_cast<uint32_t>(dStencil->GetSampleCount()),
			loadActions ? static_cast<uint32_t>(loadActions->LoadActionDepth) : 0,
			loadActions ? static_cast<uint32_t>(loadActions->LoadActionStencil) : 0,
			static_cast<uint32_t>(depthStoreAction),
			static_cast<uint32_t>(stencilStoreAction)
		};
		renderPassHash = HashAlg<uint32_t>(hashValues.data(), 6, renderPassHash);
		const uint32_t ID = dStencil->GetID();
		frameBufferHash = HashAlg<uint32_t>(&ID, 1, frameBufferHash);

	}
	if (colorArraySlices)
		frameBufferHash = HashAlg<uint32_t>(colorArraySlices->data(), renderTargets.size(), frameBufferHash);
	if (colorMipSlices)
		frameBufferHash = HashAlg<uint32_t>(colorMipSlices->data(), renderTargets.size(), frameBufferHash);
	if (depthArraySlice != std::numeric_limits<uint32_t>::max())
		frameBufferHash = HashAlg<uint32_t>(&depthArraySlice, 1, frameBufferHash);
	if (depthMipSlice != std::numeric_limits<uint32_t>::max())
		frameBufferHash = HashAlg<uint32_t>(&depthMipSlice, 1, frameBufferHash);

	RendererAPI::SampleCount sampleCount = RendererAPI::SampleCount::One;

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
		std::array<TRAP::Graphics::API::ImageFormat, 8> colorFormats{};
		TRAP::Graphics::API::ImageFormat depthStencilFormat = TRAP::Graphics::API::ImageFormat::Undefined;
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
		desc.ColorFormats = std::vector<TRAP::Graphics::API::ImageFormat>(colorFormats.begin(), colorFormats.end());
		desc.DepthStencilFormat = depthStencilFormat;
		desc.LoadActionsColor = loadActions ?
		                        std::vector<RendererAPI::LoadActionType>(loadActions->LoadActionsColor.begin(),
								                                         loadActions->LoadActionsColor.end()) :
								std::vector<RendererAPI::LoadActionType>();
		desc.LoadActionDepth = loadActions ? loadActions->LoadActionDepth : RendererAPI::LoadActionType::DontCare;
		desc.LoadActionStencil = loadActions ? loadActions->LoadActionStencil :
		                         RendererAPI::LoadActionType::DontCare;
		desc.StoreActionsColor = colorStoreActions;
		desc.StoreActionDepth = depthStoreAction;
		desc.StoreActionStencil = stencilStoreAction;
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
		if(colorArraySlices)
			desc.ColorArraySlices = *colorArraySlices;
		if(colorMipSlices)
			desc.ColorMipSlices = *colorMipSlices;
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
		for(std::size_t i = 0; i < renderTargets.size(); ++i)
		{
			TRAP::Math::Vec4 clearColor = loadActions->ClearColorValues[i];
			VkClearValue val{};
			val.color = { {clearColor.x, clearColor.y, clearColor.z, clearColor.w} };
			clearValues.push_back(val);
		}
		if(depthStencil)
		{
			VkClearValue val{};
			val.depthStencil = { loadActions->ClearDepth,
			                     loadActions->ClearStencil };
			clearValues.push_back(val);
		}
	}

	VkRenderPassBeginInfo beginInfo = VulkanInits::RenderPassBeginInfo(renderPass->GetVkRenderPass(),
	                                                                   frameBuffer->GetVkFrameBuffer(), renderArea,
																	   clearValues);

	vkCmdBeginRenderPass(m_vkCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_activeRenderPass = renderPass->GetVkRenderPass();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::AddDebugMarker(const TRAP::Math::Vec3& color, const char* name) const
{
#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(!VulkanRenderer::s_debugUtilsExtension)
		return;

	VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(color.x, color.y, color.z, name);
	vkCmdInsertDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
#else
	if(!VulkanRenderer::s_debugReportExtension)
		return;

	VkDebugMarkerMarkerInfoEXT markerInfo = VulkanInits::DebugMarkerMarkerInfo(color.x, color.y, color.z, name);
	vkCmdDebugMarkerInsertEXT(m_vkCommandBuffer, &markerInfo);
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(RendererAPI::s_aftermathSupport)
		vkCmdSetCheckpointNV(m_vkCommandBuffer, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginDebugMarker(const TRAP::Math::Vec3& color, const char* name) const
{
#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(!VulkanRenderer::s_debugUtilsExtension)
		return;

	VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(color.x, color.y, color.z, name);
	vkCmdBeginDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
#elif !defined(USE_RENDER_DOC)
	if(!VulkanRenderer::s_debugReportExtension)
		return;

	VkDebugMarkerMarkerInfoEXT markerInfo = VulkanInits::DebugMarkerMarkerInfo(color.x, color.y, color.z, name);
	vkCmdDebugMarkerBeginEXT(m_vkCommandBuffer, &markerInfo);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndDebugMarker() const
{
#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(!VulkanRenderer::s_debugUtilsExtension)
		return;

	vkCmdEndDebugUtilsLabelEXT(m_vkCommandBuffer);
#elif !defined(USE_RENDER_DOC)
	if(!VulkanRenderer::s_debugReportExtension)
		return;

	vkCmdDebugMarkerEndEXT(m_vkCommandBuffer);
#endif
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

void TRAP::Graphics::API::VulkanCommandBuffer::SetViewport(const float x, const float y, const float width,
                                                           const float height, const float minDepth,
														   const float maxDepth) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	if (width == 0.0f || height == 0.0f)
		return;

	VkViewport viewport;
	viewport.x = x;
	viewport.y = y + height;
	viewport.width = width;
	viewport.height = -height;
	viewport.minDepth = minDepth;
	viewport.maxDepth = maxDepth;

	vkCmdSetViewport(m_vkCommandBuffer, 0, 1, &viewport);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetScissor(const uint32_t x, const uint32_t y, const uint32_t width,
                                                          const uint32_t height) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	VkRect2D rect;
	rect.offset.x = static_cast<int32_t>(x);
	rect.offset.y = static_cast<int32_t>(y);
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

void TRAP::Graphics::API::VulkanCommandBuffer::DrawInstanced(const uint32_t vertexCount, const uint32_t firstVertex,
                                                             const uint32_t instanceCount,
                                                             const uint32_t firstInstance) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexed(const uint32_t indexCount, const uint32_t firstIndex,
                                                           const uint32_t firstVertex) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, 1, firstIndex, static_cast<int32_t>(firstVertex), 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexedInstanced(const uint32_t indexCount,
																    const uint32_t firstIndex,
                                                                    const uint32_t instanceCount,
                                                                    const uint32_t firstInstance,
                                                                    const uint32_t firstVertex) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, instanceCount, firstIndex, static_cast<int32_t>(firstVertex), firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature,
                                                               const uint32_t maxCommandCount,
                                                               const TRAP::Ref<Buffer>& indirectBuffer,
                                                               const uint64_t bufferOffset,
                                                               const TRAP::Ref<Buffer>& counterBuffer,
                                                               const uint64_t counterBufferOffset) const
{
	VulkanBuffer* iBuffer = dynamic_cast<VulkanBuffer*>(indirectBuffer.get());
	VulkanCommandSignature* cSig = dynamic_cast<VulkanCommandSignature*>(cmdSignature.get());

	if (cSig->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDraw)
	{
		if (counterBuffer)
		{
			vkCmdDrawIndirectCountKHR(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset,
				                      dynamic_cast<VulkanBuffer*>(counterBuffer.get())->GetVkBuffer(),
				                      counterBufferOffset, maxCommandCount, cSig->GetStride());
		}
		else
			vkCmdDrawIndirect(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset, maxCommandCount,
			                  cSig->GetStride());
	}
	else if (cSig->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDrawIndex)
	{
		if (counterBuffer)
		{
			vkCmdDrawIndexedIndirectCountKHR(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset,
				                             dynamic_cast<VulkanBuffer*>(counterBuffer.get())->GetVkBuffer(),
				                             counterBufferOffset, maxCommandCount, cSig->GetStride());
		}
		else
			vkCmdDrawIndexedIndirect(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset, maxCommandCount,
			                         cSig->GetStride());
	}
	else if (cSig->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDispatch)
		vkCmdDispatchIndirect(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Dispatch(const uint32_t groupCountX, const uint32_t groupCountY,
                                                        const uint32_t groupCountZ) const
{
	TRAP_ASSERT(m_vkCommandBuffer != VK_NULL_HANDLE);

	vkCmdDispatch(m_vkCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateBuffer(const TRAP::Ref<Buffer>& buffer,
															const uint64_t dstOffset,
                                                            const TRAP::Ref<Buffer>& srcBuffer,
                                                            const uint64_t srcOffset, const uint64_t size) const
{
	VulkanBuffer* sBuffer = dynamic_cast<VulkanBuffer*>(srcBuffer.get());
	VulkanBuffer* buf = dynamic_cast<VulkanBuffer*>(buffer.get());

	TRAP_ASSERT(srcBuffer);
	TRAP_ASSERT(sBuffer->GetVkBuffer());
	TRAP_ASSERT(buffer);
	TRAP_ASSERT(buf->GetVkBuffer());
	TRAP_ASSERT(srcOffset + size <= srcBuffer->GetSize());
	TRAP_ASSERT(dstOffset + size <= buffer->GetSize());

	VkBufferCopy region{};
	region.srcOffset = srcOffset;
	region.dstOffset = dstOffset;
	region.size = static_cast<VkDeviceSize>(size);

	vkCmdCopyBuffer(m_vkCommandBuffer, sBuffer->GetVkBuffer(), buf->GetVkBuffer(), 1, &region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateSubresource(TRAP::Graphics::Texture* texture,
                                                                 const TRAP::Ref<Buffer>& srcBuffer,
                                                                 const RendererAPI::SubresourceDataDesc& subresourceDesc) const
{
	VkBuffer buffer = dynamic_cast<VulkanBuffer*>(srcBuffer.get())->GetVkBuffer();
	auto* vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(texture);

	const TRAP::Graphics::API::ImageFormat fmt = texture->GetImageFormat();
	if(TRAP::Graphics::API::ImageFormatIsSinglePlane(fmt))
	{
		const uint32_t width = TRAP::Math::Max<uint32_t>(1u, texture->GetWidth() >> subresourceDesc.MipLevel);
		const uint32_t height = TRAP::Math::Max<uint32_t>(1u, texture->GetHeight() >> subresourceDesc.MipLevel);
		const uint32_t depth = TRAP::Math::Max<uint32_t>(1u, texture->GetDepth() >> subresourceDesc.MipLevel);
		const uint32_t numBlocksWide = subresourceDesc.RowPitch / (TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt) >> 3);
		const uint32_t numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);

		VkBufferImageCopy copy{};
		copy.bufferOffset = subresourceDesc.SrcOffset;
		copy.bufferRowLength = numBlocksWide * TRAP::Graphics::API::ImageFormatWidthOfBlock(fmt);
		copy.bufferImageHeight = numBlocksHigh * TRAP::Graphics::API::ImageFormatHeightOfBlock(fmt);
		copy.imageSubresource.aspectMask = texture->GetAspectMask();
		copy.imageSubresource.mipLevel = subresourceDesc.MipLevel;
		copy.imageSubresource.baseArrayLayer = subresourceDesc.ArrayLayer;
		copy.imageSubresource.layerCount = 1;
		copy.imageOffset.x = 0;
		copy.imageOffset.y = 0;
		copy.imageOffset.z = 0;
		copy.imageExtent.width = width;
		copy.imageExtent.height = height;
		copy.imageExtent.depth = depth;

		vkCmdCopyBufferToImage(m_vkCommandBuffer, buffer, vkTexture->GetVkImage(),
		                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
	}
	else
	{
		const uint32_t width = texture->GetWidth();
		const uint32_t height = texture->GetHeight();
		const uint32_t depth = texture->GetDepth();
		const uint32_t numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(fmt);

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
			copy.imageExtent.width = TRAP::Graphics::API::ImageFormatPlaneWidth(fmt, i, width);
			copy.imageExtent.height = TRAP::Graphics::API::ImageFormatPlaneHeight(fmt, i, height);
			copy.imageExtent.depth = depth;
			offset += static_cast<uint64_t>(copy.imageExtent.width) * copy.imageExtent.height *
			          TRAP::Graphics::API::ImageFormatPlaneSizeOfBlock(fmt, i);
		}

		vkCmdCopyBufferToImage(m_vkCommandBuffer, buffer, vkTexture->GetVkImage(),
		                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, numOfPlanes, bufferImagesCopy.data());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool,
                                                              const uint32_t startQuery,
															  const uint32_t queryCount) const
{
	vkCmdResetQueryPool(m_vkCommandBuffer, dynamic_cast<VulkanQueryPool*>(queryPool.get())->GetVkQueryPool(),
	                    startQuery, queryCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginQuery(const TRAP::Ref<QueryPool>& queryPool,
                                                          const RendererAPI::QueryDesc& desc) const
{
	VulkanQueryPool* qPool = dynamic_cast<VulkanQueryPool*>(queryPool.get());

	const VkQueryType type = qPool->GetVkQueryType();
	switch(type)
	{
	case VK_QUERY_TYPE_TIMESTAMP:
		vkCmdWriteTimestamp(m_vkCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, qPool->GetVkQueryPool(),
		                    desc.Index);
		break;

	case VK_QUERY_TYPE_PIPELINE_STATISTICS:
	case VK_QUERY_TYPE_OCCLUSION:
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndQuery(const TRAP::Ref<QueryPool>& queryPool,
                                                        const RendererAPI::QueryDesc& desc) const
{
	BeginQuery(queryPool, desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResolveQuery(const TRAP::Ref<QueryPool>& queryPool,
                                                            const TRAP::Ref<Buffer>& readBackBuffer,
                                                            const uint32_t startQuery,
															const uint32_t queryCount) const
{
	VkQueryResultFlags flags = VK_QUERY_RESULT_64_BIT;
	flags |= VK_QUERY_RESULT_WAIT_BIT;

	vkCmdCopyQueryPoolResults(m_vkCommandBuffer,
	                          dynamic_cast<VulkanQueryPool*>(queryPool.get())->GetVkQueryPool(),
	                          startQuery, queryCount,
	                          dynamic_cast<VulkanBuffer*>(readBackBuffer.get())->GetVkBuffer(),
	                          0, sizeof(uint64_t), flags);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* bufferBarriers,
	                                                           const std::vector<RendererAPI::TextureBarrier>* textureBarriers,
	                                                           const std::vector<RendererAPI::RenderTargetBarrier>* renderTargetBarriers) const
{
	std::vector<VkImageMemoryBarrier> iBarriers;
	std::size_t iBarriersCount = 0;
	if (textureBarriers)
		iBarriersCount += textureBarriers->size();
	if(renderTargetBarriers)
		iBarriersCount += renderTargetBarriers->size();
	iBarriers.resize(iBarriersCount);
	uint32_t imageBarrierCount = 0;

	std::vector<VkBufferMemoryBarrier> bBarriers;
	if (bufferBarriers)
		bBarriers.resize(bufferBarriers->size());
	uint32_t bufferBarrierCount = 0;

	VkAccessFlags srcAccessFlags = 0;
	VkAccessFlags dstAccessFlags = 0;

	VulkanQueue* queue = dynamic_cast<VulkanQueue*>(m_queue.get());
	if(bufferBarriers)
	{
		for (const auto& trans : *bufferBarriers)
		{
			const TRAP::Ref<Buffer>& buffer = trans.Buffer;
			VkBufferMemoryBarrier* bufferBarrier = nullptr;

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
				bufferBarrier->buffer = dynamic_cast<VulkanBuffer*>(buffer.get())->GetVkBuffer();
				bufferBarrier->size = VK_WHOLE_SIZE;
				bufferBarrier->offset = 0;

				if(trans.Acquire)
				{
					bufferBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
					bufferBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
				}
				else if(trans.Release)
				{
					bufferBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
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
	}

	if(textureBarriers)
	{
		for (const auto& trans : *textureBarriers)
		{
			const TRAP::Graphics::Texture* texture = trans.Texture;
			auto* vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(trans.Texture);
			VkImageMemoryBarrier* imageBarrier = nullptr;

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
				imageBarrier->image = vkTexture->GetVkImage();
				imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
				imageBarrier->subresourceRange.baseMipLevel = trans.SubresourceBarrier ? trans.MipLevel : 0;
				imageBarrier->subresourceRange.levelCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
				imageBarrier->subresourceRange.baseArrayLayer = trans.SubresourceBarrier ? trans.ArrayLayer : 0;
				imageBarrier->subresourceRange.layerCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

				if(trans.Acquire)
				{
					imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
					imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
				}
				else if(trans.Release)
				{
					imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
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
	}

	if(renderTargetBarriers)
	{
		for (const auto& trans : *renderTargetBarriers)
		{
			TRAP::Graphics::Texture* texture = dynamic_cast<VulkanRenderTarget*>(trans.RenderTarget.get())->m_texture.get();
			auto* vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(texture);
			VkImageMemoryBarrier* imageBarrier = nullptr;

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
				imageBarrier->image = vkTexture->GetVkImage();
				imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
				imageBarrier->subresourceRange.baseMipLevel = trans.SubresourceBarrier ? trans.MipLevel : 0;
				imageBarrier->subresourceRange.levelCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
				imageBarrier->subresourceRange.baseArrayLayer = trans.SubresourceBarrier ? trans.ArrayLayer : 0;
				imageBarrier->subresourceRange.layerCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

				if(trans.Acquire)
				{
					imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(trans.QueueType)];
					imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
				}
				else if(trans.Release)
				{
					imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
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
	}

	const VkPipelineStageFlags srcStageMask = DetermineVkPipelineStageFlags(srcAccessFlags, queue->GetQueueType());
	const VkPipelineStageFlags dstStageMask = DetermineVkPipelineStageFlags(dstAccessFlags, queue->GetQueueType());

	if(bufferBarrierCount || imageBarrierCount)
		vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, bufferBarrierCount,
		                     bBarriers.data(), imageBarrierCount, iBarriers.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const RendererAPI::BufferBarrier* bufferBarrier,
	                                                           const RendererAPI::TextureBarrier* textureBarrier,
	                                                           const RendererAPI::RenderTargetBarrier* renderTargetBarrier) const
{
	std::vector<VkImageMemoryBarrier> iBarriers;
	std::size_t iBarriersCount = 0;
	if (textureBarrier)
		iBarriersCount++;
	if(renderTargetBarrier)
		iBarriersCount++;
	iBarriers.resize(iBarriersCount);
	uint32_t imageBarrierCount = 0;

	VkBufferMemoryBarrier bBarrier;

	VkAccessFlags srcAccessFlags = 0;
	VkAccessFlags dstAccessFlags = 0;

	VulkanQueue* queue = dynamic_cast<VulkanQueue*>(m_queue.get());
	if(bufferBarrier)
	{
		const TRAP::Ref<Buffer>& buffer = bufferBarrier->Buffer;

		if (bufferBarrier->CurrentState == RendererAPI::ResourceState::UnorderedAccess &&
			bufferBarrier->NewState == RendererAPI::ResourceState::UnorderedAccess)
		{
			bBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bBarrier.pNext = nullptr;

			bBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
			bBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		}
		else
		{
			bBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bBarrier.pNext = nullptr;

			bBarrier.srcAccessMask = ResourceStateToVkAccessFlags(bufferBarrier->CurrentState);
			bBarrier.dstAccessMask = ResourceStateToVkAccessFlags(bufferBarrier->NewState);
		}

		bBarrier.buffer = dynamic_cast<VulkanBuffer*>(buffer.get())->GetVkBuffer();
		bBarrier.size = VK_WHOLE_SIZE;
		bBarrier.offset = 0;

		if(bufferBarrier->Acquire)
		{
			bBarrier.srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(bufferBarrier->QueueType)];
			bBarrier.dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
		}
		else if(bufferBarrier->Release)
		{
			bBarrier.srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
			bBarrier.dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(bufferBarrier->QueueType)];
		}
		else
		{
			bBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		}

		srcAccessFlags |= bBarrier.srcAccessMask;
		dstAccessFlags |= bBarrier.dstAccessMask;
	}

	if(textureBarrier)
	{
		TRAP::Graphics::Texture* texture = textureBarrier->Texture;
		auto* vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(texture);
		VkImageMemoryBarrier* imageBarrier = nullptr;

		if (textureBarrier->CurrentState == RendererAPI::ResourceState::UnorderedAccess &&
			textureBarrier->NewState == RendererAPI::ResourceState::UnorderedAccess)
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

			imageBarrier->srcAccessMask = ResourceStateToVkAccessFlags(textureBarrier->CurrentState);
			imageBarrier->dstAccessMask = ResourceStateToVkAccessFlags(textureBarrier->NewState);
			imageBarrier->oldLayout = ResourceStateToVkImageLayout(textureBarrier->CurrentState);
			imageBarrier->newLayout = ResourceStateToVkImageLayout(textureBarrier->NewState);
		}

		if(imageBarrier)
		{
			imageBarrier->image = vkTexture->GetVkImage();
			imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
			imageBarrier->subresourceRange.baseMipLevel = textureBarrier->SubresourceBarrier ? textureBarrier->MipLevel : 0;
			imageBarrier->subresourceRange.levelCount = textureBarrier->SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
			imageBarrier->subresourceRange.baseArrayLayer = textureBarrier->SubresourceBarrier ? textureBarrier->ArrayLayer : 0;
			imageBarrier->subresourceRange.layerCount = textureBarrier->SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

			if(textureBarrier->Acquire)
			{
				imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(textureBarrier->QueueType)];
				imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
			}
			else if(textureBarrier->Release)
			{
				imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
				imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(textureBarrier->QueueType)];
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

	if(renderTargetBarrier)
	{
		TRAP::Graphics::Texture* texture = dynamic_cast<VulkanRenderTarget*>(renderTargetBarrier->RenderTarget.get())->m_texture.get();
		auto* vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(texture);
		VkImageMemoryBarrier* imageBarrier = nullptr;

		if (renderTargetBarrier->CurrentState == RendererAPI::ResourceState::UnorderedAccess &&
			renderTargetBarrier->NewState == RendererAPI::ResourceState::UnorderedAccess)
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

			imageBarrier->srcAccessMask = ResourceStateToVkAccessFlags(renderTargetBarrier->CurrentState);
			imageBarrier->dstAccessMask = ResourceStateToVkAccessFlags(renderTargetBarrier->NewState);
			imageBarrier->oldLayout = ResourceStateToVkImageLayout(renderTargetBarrier->CurrentState);
			imageBarrier->newLayout = ResourceStateToVkImageLayout(renderTargetBarrier->NewState);
		}

		if(imageBarrier)
		{
			imageBarrier->image = vkTexture->GetVkImage();
			imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
			imageBarrier->subresourceRange.baseMipLevel = renderTargetBarrier->SubresourceBarrier ? renderTargetBarrier->MipLevel : 0;
			imageBarrier->subresourceRange.levelCount = renderTargetBarrier->SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
			imageBarrier->subresourceRange.baseArrayLayer = renderTargetBarrier->SubresourceBarrier ? renderTargetBarrier->ArrayLayer : 0;
			imageBarrier->subresourceRange.layerCount = renderTargetBarrier->SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

			if(renderTargetBarrier->Acquire)
			{
				imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(renderTargetBarrier->QueueType)];
				imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
			}
			else if(renderTargetBarrier->Release)
			{
				imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
				imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[static_cast<uint32_t>(renderTargetBarrier->QueueType)];
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

	const VkPipelineStageFlags srcStageMask = DetermineVkPipelineStageFlags(srcAccessFlags, queue->GetQueueType());
	const VkPipelineStageFlags dstStageMask = DetermineVkPipelineStageFlags(dstAccessFlags, queue->GetQueueType());

	if(bufferBarrier || imageBarrierCount)
		vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, bufferBarrier ? 1 : 0,
		                     &bBarrier, imageBarrierCount, iBarriers.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetStencilReferenceValue(const uint32_t val) const
{
	vkCmdSetStencilReference(m_vkCommandBuffer, VK_STENCIL_FRONT_AND_BACK, val);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetShadingRate(RendererAPI::ShadingRate rate,
															  Texture* /*texture*/,
															  RendererAPI::ShadingRateCombiner postRasterizerState,
															  RendererAPI::ShadingRateCombiner finalRate) const
{
	//Texture would be used for https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#primsrast-fragment-shading-rate-attachment

	TRAP_ASSERT(static_cast<uint32_t>(RendererAPI::GPUSettings.ShadingRateCaps), "Shading rate is not supported!");
	if(static_cast<uint32_t>(RendererAPI::GPUSettings.ShadingRateCaps) == 0) //VRS is not supported
		return;

	if(static_cast<bool>(RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerDraw))
	{
		std::array<VkFragmentShadingRateCombinerOpKHR, 2> combiner
		{
			ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(postRasterizerState),
			ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(finalRate)
		};
		VkExtent2D fragmentSize = ShadingRateToVkExtent2D(rate);

		vkCmdSetFragmentShadingRateKHR(m_vkCommandBuffer, &fragmentSize, combiner.data());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const TRAP::Math::Vec4 color, const uint32_t width,
													 const uint32_t height)
{
	VkClearRect rect;
	VkRect2D r;
	r.offset.x = 0;
	r.offset.y = 0;
	r.extent.width = width;
	r.extent.height = height;
	rect.rect = r;
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;

	VkClearAttachment attachment;
	attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.color = {{color.x, color.y, color.z, color.w}};

	vkCmdClearAttachments(m_vkCommandBuffer, 1, &attachment, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const float depth, const uint32_t stencil,
 												     const uint32_t width, const uint32_t height)
{
	VkClearRect rect;
	VkRect2D r;
	r.offset.x = 0;
	r.offset.y = 0;
	r.extent.width = width;
	r.extent.height = height;
	rect.rect = r;
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;

	VkClearAttachment attachment;
	attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.depthStencil = { depth, stencil };

	vkCmdClearAttachments(m_vkCommandBuffer, 1, &attachment, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const float depth, const uint32_t width, const uint32_t height)
{
	VkClearRect rect;
	VkRect2D r;
	r.offset.x = 0;
	r.offset.y = 0;
	r.extent.width = width;
	r.extent.height = height;
	rect.rect = r;
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;

	VkClearAttachment attachment;
	attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.depthStencil = { depth, 0 };

	vkCmdClearAttachments(m_vkCommandBuffer, 1, &attachment, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const uint32_t stencil, const uint32_t width,
                                                     const uint32_t height)
{
	VkClearRect rect;
	VkRect2D r;
	r.offset.x = 0;
	r.offset.y = 0;
	r.extent.width = width;
	r.extent.height = height;
	rect.rect = r;
	rect.baseArrayLayer = 0;
	rect.layerCount = 1;

	VkClearAttachment attachment;
	attachment.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
	attachment.colorAttachment = 0;
	attachment.clearValue.depthStencil = { 1.0f, stencil };

	vkCmdClearAttachments(m_vkCommandBuffer, 1, &attachment, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass TRAP::Graphics::API::VulkanCommandBuffer::GetActiveVkRenderPass() const
{
	return m_activeRenderPass;
}
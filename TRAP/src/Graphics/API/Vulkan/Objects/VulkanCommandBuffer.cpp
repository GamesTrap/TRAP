#include "TRAPPCH.h"
#include "VulkanCommandBuffer.h"

#include "VulkanFrameBuffer.h"
#include "VulkanRenderTarget.h"
#include "VulkanPipeline.h"
#include "VulkanCommandSignature.h"
#include "VulkanQueryPool.h"
#include "VulkanRenderPass.h"
#include "VulkanBuffer.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Objects/DescriptorSet.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRootSignature.h"
#include "VulkanQueue.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanTexture.h"

TRAP::Graphics::API::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Destroying CommandBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(m_device && (m_vkCommandBuffer != nullptr) && (m_vkCommandPool != nullptr))
		vkFreeCommandBuffers(m_device->GetVkDevice(), m_vkCommandPool, 1, &m_vkCommandBuffer);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer::VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<Queue> queue,
                                                              VkCommandPool& commandPool, const bool secondary)
	: CommandBuffer(std::move(queue)),
	  m_device(std::move(device)),
	  m_vkCommandPool(commandPool),
	  m_secondary(secondary)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanCommandBuffer(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_vkCommandPool, "VulkanCommandBuffer(): Vulkan CommandPool is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Creating CommandBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const VkCommandBufferAllocateInfo info = VulkanInits::CommandBufferAllocateInfo(commandPool, secondary);

	VkCall(vkAllocateCommandBuffers(m_device->GetVkDevice(), &info, &m_vkCommandBuffer));
	TRAP_ASSERT(m_vkCommandBuffer, "VulkanCommandBuffer(): Vulkan CommandBuffer is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::QueueType TRAP::Graphics::API::VulkanCommandBuffer::GetQueueType() const
{
	return m_queue->GetQueueType();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstants(const TRAP::Ref<RootSignature>& rootSignature,
                                                                 const std::string_view name, const void* constants,
																 const usize constantsLength) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(rootSignature, "VulkanCommandBuffer::BindPushConstants(): RootSignature is nullptr");
	TRAP_ASSERT(!name.empty(), "VulkanCommandBuffer::BindPushConstants(): Name is empty");
	TRAP_ASSERT(constants, "VulkanCommandBuffer::BindPushConstants(): Constants are nullptr");

	const Ref<VulkanRootSignature> rSig = std::dynamic_pointer_cast<VulkanRootSignature>(rootSignature);

	const RendererAPI::DescriptorInfo* const desc = rSig->GetDescriptor(name);

	if(desc == nullptr)
	{
		TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Unable to find PushConstants with name: ", name, "!");
		return;
	}

	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant, "VulkanCommandBuffer::BindPushConstants(): Descriptor is not a RootConstant!");
	TRAP_ASSERT(desc->Size == constantsLength, "VulkanCommandBuffer::BindPushConstants(): Size of constants don't match that of Descriptor!");

	vkCmdPushConstants(m_vkCommandBuffer, rSig->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstantsByIndex(const TRAP::Ref<RootSignature>& rootSignature,
	                                                                    const u32 paramIndex, const void* constants,
																		const usize constantsLength) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(rootSignature, "VulkanCommandBuffer::BindPushConstantsByIndex(): RootSignature is nullptr");
	TRAP_ASSERT(paramIndex < rootSignature->GetDescriptorCount(), "VulkanCommandBuffer::BindPushConstantsByIndex(): Index out of bounds!");
	TRAP_ASSERT(constants, "VulkanCommandBuffer::BindPushConstantsByIndex(): Constants are nullptr");

	const RendererAPI::DescriptorInfo* const desc = &rootSignature->GetDescriptors()[paramIndex];

	if(desc == nullptr)
	{
		TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Unable to find PushConstants with RootSignature index: ", paramIndex, "!");
		return;
	}

	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant, "VulkanCommandBuffer::BindPushConstantsByIndex(): Descriptor is not a RootConstant!");
	TRAP_ASSERT(desc->Size == constantsLength, "VulkanCommandBuffer::BindPushConstantsByIndex(): Size of constants don't match that of Descriptor!");

	vkCmdPushConstants(m_vkCommandBuffer,
					   std::dynamic_pointer_cast<VulkanRootSignature>(rootSignature)->GetVkPipelineLayout(),
					   desc->VkStages, 0, desc->Size, constants);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindDescriptorSet(const u32 index, DescriptorSet& descriptorSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const VulkanDescriptorSet& dSet = dynamic_cast<VulkanDescriptorSet&>(descriptorSet);
	TRAP_ASSERT(!dSet.GetVkDescriptorSets().empty(), "VulkanCommandBuffer::BindDescriptorSet(): DescriptorSet is empty!");
	TRAP_ASSERT(dSet.GetMaxSets() > index, "VulkanCommandBuffer::BindDescriptorSet(): Index out of bounds!");

	const TRAP::Ref<VulkanRootSignature> rootSignature = dSet.GetRootSignature();

	if(m_boundPipelineLayout != rootSignature->GetVkPipelineLayout())
	{
		m_boundPipelineLayout = rootSignature->GetVkPipelineLayout();

		//Vulkan requires to bind all descriptor sets up to the highest set number even if they are empty
		//Example: If shader uses only set 2, we still have to bind empty sets for set = 0 and set = 1
		for(u32 setIndex = 0; setIndex < RendererAPI::MaxDescriptorSets; ++setIndex)
		{
			if(rootSignature->GetVkEmptyDescriptorSets()[setIndex] != VK_NULL_HANDLE)
				vkCmdBindDescriptorSets(m_vkCommandBuffer,
				                        VkPipelineBindPointTranslator[std::to_underlying(rootSignature->GetPipelineType())],
				                        rootSignature->GetVkPipelineLayout(), setIndex, 1,
				                        &rootSignature->GetVkEmptyDescriptorSets()[setIndex], 0, nullptr);
		}
	}

	vkCmdBindDescriptorSets(m_vkCommandBuffer,
	                        VkPipelineBindPointTranslator[std::to_underlying(rootSignature->GetPipelineType())],
	                        rootSignature->GetVkPipelineLayout(), dSet.GetSet(),
	                        1, &dSet.GetVkDescriptorSets()[index], dSet.GetDynamicOffsetCount(),
							dSet.GetDynamicOffsetCount() != 0u ? &dSet.GetDynamicSizeOffsets()[index].Offset : nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindIndexBuffer(const TRAP::Ref<Buffer>& buffer,
                                                               const RendererAPI::IndexType indexType,
															   const u64 offset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(buffer, "VulkanCommandBuffer::BindIndexBuffer(): Index buffer is nullptr!");

	const VkIndexType vkIndexType = ((RendererAPI::IndexType::UInt16 == indexType) ? VK_INDEX_TYPE_UINT16 :
	                                                                                 VK_INDEX_TYPE_UINT32);
	vkCmdBindIndexBuffer(m_vkCommandBuffer, std::dynamic_pointer_cast<VulkanBuffer>(buffer)->GetVkBuffer(), offset,
	                     vkIndexType);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindVertexBuffer(const std::vector<TRAP::Ref<Buffer>>& buffers,
                                                                const std::vector<u32>& strides,
                                                                const std::vector<u64>& offsets) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(!buffers.empty(), "VulkanCommandBuffer::BindVertexBuffer(): Vertex buffers are empty!");
	TRAP_ASSERT(!strides.empty(), "VulkanCommandBuffer::BindVertexBuffer(): Strides are empty!");

	const u32 maxBuffers = RendererAPI::GPUSettings.MaxVertexInputBindings;
	const u32 cappedBufferCount = (buffers.size() > NumericCast<usize>(maxBuffers)) ?
	                                   maxBuffers : NumericCast<u32>(buffers.size());

	//Capped the number of buffers to the maximum allowed by the device
	std::vector<VkBuffer> vkBuffers(cappedBufferCount);
	std::vector<VkDeviceSize> vkOffsets(cappedBufferCount);

	for(usize i = 0; i < vkBuffers.size(); ++i)
	{
		vkBuffers[i] = std::dynamic_pointer_cast<VulkanBuffer>(buffers[i])->GetVkBuffer();
		vkOffsets[i] = (offsets.size() > i) ? offsets[i] : 0;
	}

	vkCmdBindVertexBuffers(m_vkCommandBuffer, 0, cappedBufferCount, vkBuffers.data(), vkOffsets.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPipeline(const TRAP::Ref<Pipeline>& pipeline) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(pipeline, "VulkanCommandBuffer::BindPipeline(): Pipeline is nullptr!");

	const Ref<VulkanPipeline> pipe = std::dynamic_pointer_cast<VulkanPipeline>(pipeline);
	const VkPipelineBindPoint pipelineBindPoint = VkPipelineBindPointTranslator[std::to_underlying(pipe->GetPipelineType())];
	vkCmdBindPipeline(m_vkCommandBuffer, pipelineBindPoint, pipe->GetVkPipeline());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
																 const TRAP::Ref<RenderTarget>& depthStencil,
																 const RendererAPI::LoadActionsDesc* const loadActions,
																 const std::vector<u32>* const colorArraySlices,
																 const std::vector<u32>* const colorMipSlices,
																 const u32 depthArraySlice,
																 const u32 depthMipSlice,
																 const TRAP::Ref<RenderTarget>& shadingRate)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if(m_activeRenderPass != nullptr)
	{
		vkCmdEndRenderPass(m_vkCommandBuffer);
		m_activeRenderPass = VK_NULL_HANDLE;
	}

	if (renderTargets.empty() && !depthStencil)
		return;

	usize renderPassHash = 0;
	usize frameBufferHash = 0;
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
	for(usize i = 0; i < renderTargets.size(); ++i)
	{
		const Ref<VulkanTexture> vkTex = std::dynamic_pointer_cast<VulkanTexture>(renderTargets[i]->GetTexture());
		if(vkTex->IsLazilyAllocated())
			colorStoreActions[i] = RendererAPI::StoreActionType::DontCare;
		else if(loadActions != nullptr)
			colorStoreActions[i] = loadActions->StoreActionsColor[i];

		const std::array<u32, 4> hashValues =
		{
			std::to_underlying(renderTargets[i]->GetImageFormat()),
			NumericCast<u32>(std::to_underlying(renderTargets[i]->GetSampleCount())),
			loadActions != nullptr ? std::to_underlying(loadActions->LoadActionsColor[i]) : 0u,
			std::to_underlying(colorStoreActions[i])
		};

		renderPassHash = HashAlg<u32>(hashValues.data(), 4, renderPassHash);
		const u64 ID = std::dynamic_pointer_cast<VulkanRenderTarget>(renderTargets[i])->GetID();
		frameBufferHash = HashAlg<u64>(&ID, 1, frameBufferHash);
	}
	if(depthStencil)
	{
		const Ref<VulkanRenderTarget> dStencil = std::dynamic_pointer_cast<VulkanRenderTarget>(depthStencil);
		const Ref<VulkanTexture> vkTex = std::dynamic_pointer_cast<VulkanTexture>(dStencil->GetTexture());

		if(vkTex->IsLazilyAllocated())
		{
			depthStoreAction = RendererAPI::StoreActionType::DontCare;
			stencilStoreAction = RendererAPI::StoreActionType::DontCare;
		}
		else if(loadActions != nullptr)
		{
			depthStoreAction = loadActions->StoreActionDepth;
			stencilStoreAction = loadActions->StoreActionStencil;
		}

		const std::array<u32, 6> hashValues =
		{
			std::to_underlying(dStencil->GetImageFormat()),
			NumericCast<u32>(std::to_underlying(dStencil->GetSampleCount())),
			loadActions != nullptr ? std::to_underlying(loadActions->LoadActionDepth) : 0u,
			loadActions != nullptr ? std::to_underlying(loadActions->LoadActionStencil) : 0u,
			std::to_underlying(depthStoreAction),
			std::to_underlying(stencilStoreAction)
		};
		renderPassHash = HashAlg<u32>(hashValues.data(), 6, renderPassHash);
		const u64 ID = dStencil->GetID();
		frameBufferHash = HashAlg<u64>(&ID, 1, frameBufferHash);
	}
	if (colorArraySlices != nullptr)
		frameBufferHash = HashAlg<u32>(colorArraySlices->data(), renderTargets.size(), frameBufferHash);
	if (colorMipSlices != nullptr)
		frameBufferHash = HashAlg<u32>(colorMipSlices->data(), renderTargets.size(), frameBufferHash);
	if (depthArraySlice != std::numeric_limits<u32>::max())
		frameBufferHash = HashAlg<u32>(&depthArraySlice, 1, frameBufferHash);
	if (depthMipSlice != std::numeric_limits<u32>::max())
		frameBufferHash = HashAlg<u32>(&depthMipSlice, 1, frameBufferHash);
	if(shadingRate)
	{
		const Ref<VulkanTexture> vkTex = std::dynamic_pointer_cast<VulkanTexture>(shadingRate);

		const u32 hashValue = std::to_underlying(shadingRate->GetImageFormat());
		renderPassHash = HashAlg<u32>(&hashValue, 1, renderPassHash);
		const u64 ID = std::dynamic_pointer_cast<VulkanRenderTarget>(shadingRate)->GetID();
		frameBufferHash = HashAlg<u64>(&ID, 1, frameBufferHash);
	}

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
		TRAP::Graphics::API::ImageFormat shadingRateFormat = TRAP::Graphics::API::ImageFormat::Undefined;
		for (u32 i = 0; i < renderTargets.size(); ++i)
			colorFormats[i] = renderTargets[i]->GetImageFormat();
		if (depthStencil)
		{
			depthStencilFormat = depthStencil->GetImageFormat();
			sampleCount = depthStencil->GetSampleCount();
		}
		else if (!renderTargets.empty())
			sampleCount = renderTargets[0]->GetSampleCount();
		if(shadingRate)
			shadingRateFormat = shadingRate->GetImageFormat();

		VulkanRenderer::RenderPassDesc desc{};
		desc.RenderTargetCount = NumericCast<u32>(renderTargets.size());
		desc.SampleCount = sampleCount;
		desc.ColorFormats = std::vector<TRAP::Graphics::API::ImageFormat>(colorFormats.begin(), colorFormats.end());
		desc.DepthStencilFormat = depthStencilFormat;
		desc.ShadingRateFormat = shadingRateFormat;
		desc.LoadActionsColor = loadActions != nullptr ?
		                        std::vector<RendererAPI::LoadActionType>(loadActions->LoadActionsColor.begin(),
								                                         loadActions->LoadActionsColor.end()) :
								std::vector<RendererAPI::LoadActionType>();
		desc.LoadActionDepth = loadActions != nullptr ? loadActions->LoadActionDepth : RendererAPI::LoadActionType::DontCare;
		desc.LoadActionStencil = loadActions != nullptr ? loadActions->LoadActionStencil :
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
		desc.ShadingRate = shadingRate;
		desc.RenderPass = renderPass;
		if(colorArraySlices != nullptr)
			desc.ColorArraySlices = *colorArraySlices;
		if(colorMipSlices != nullptr)
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
	if(loadActions != nullptr)
	{
		for(usize i = 0; i < renderTargets.size(); ++i)
		{
			VkClearValue val{};
			val.color = VulkanInits::ClearColorValue(loadActions->ClearColorValues[i], renderTargets[i]->GetImageFormat());

			clearValues.push_back(val);
		}
		if(depthStencil)
		{
			VkClearValue val{};
			val.depthStencil = { loadActions->ClearDepthStencil.Depth,
			                     loadActions->ClearDepthStencil.Stencil };
			clearValues.push_back(val);
		}
	}

	const VkRenderPassBeginInfo beginInfo = VulkanInits::RenderPassBeginInfo(renderPass->GetVkRenderPass(),
	                                                                         frameBuffer->GetVkFrameBuffer(), renderArea,
																	         clearValues);

	vkCmdBeginRenderPass(m_vkCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_activeRenderPass = renderPass->GetVkRenderPass();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::AddDebugMarker(const TRAP::Math::Vec3& color, const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(name.empty(), "VulkanCommandBuffer::AddDebugMarker(): Name is empty!");

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(!VulkanRenderer::s_debugUtilsExtension)
		return;

	const VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(color.x(), color.y(), color.z(), name);
	vkCmdInsertDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
#else
	if(!VulkanRenderer::s_debugReportExtension)
		return;

	const VkDebugMarkerMarkerInfoEXT markerInfo = VulkanInits::DebugMarkerMarkerInfo(color.x(), color.y(), color.z(), name);
	vkCmdDebugMarkerInsertEXT(m_vkCommandBuffer, &markerInfo);
#endif

#ifdef ENABLE_NSIGHT_AFTERMATH
	if(RendererAPI::s_aftermathSupport)
		vkCmdSetCheckpointNV(m_vkCommandBuffer, name.data());
#endif /*ENABLE_NSIGHT_AFTERMATH*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginDebugMarker(const TRAP::Math::Vec3& color, const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(name.empty(), "VulkanCommandBuffer::BeginDebugMarker(): Name is empty!");

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	if(!VulkanRenderer::s_debugUtilsExtension)
		return;

	const VkDebugUtilsLabelEXT markerInfo = VulkanInits::DebugUtilsLabelExt(color.x(), color.y(), color.z(), name);
	vkCmdBeginDebugUtilsLabelEXT(m_vkCommandBuffer, &markerInfo);
#elif !defined(USE_RENDER_DOC)
	if(!VulkanRenderer::s_debugReportExtension)
		return;

	const VkDebugMarkerMarkerInfoEXT markerInfo = VulkanInits::DebugMarkerMarkerInfo(color.x(), color.y(), color.z(), name);
	vkCmdDebugMarkerBeginEXT(m_vkCommandBuffer, &markerInfo);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndDebugMarker() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

void TRAP::Graphics::API::VulkanCommandBuffer::Begin(const bool oneTimeSubmit)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const VkCommandBufferBeginInfo beginInfo = VulkanInits::CommandBufferBeginInfo(oneTimeSubmit);

	VkCall(vkBeginCommandBuffer(m_vkCommandBuffer, &beginInfo));

	//Reset CPU side data
	m_boundPipelineLayout = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::End()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (m_activeRenderPass != nullptr)
		vkCmdEndRenderPass(m_vkCommandBuffer);
	m_activeRenderPass = VK_NULL_HANDLE;

	VkCall(vkEndCommandBuffer(m_vkCommandBuffer));
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetViewport(const f32 x, const f32 y, const f32 width,
                                                           const f32 height, const f32 minDepth,
														   const f32 maxDepth) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	if (width == 0.0f || height == 0.0f)
		return;

	VkViewport viewport;
	viewport.x = x;
	viewport.y = height >= 0 ? y + height : y;
	viewport.width = width;
	viewport.height = -height;
	viewport.minDepth = minDepth;
	viewport.maxDepth = maxDepth;

	vkCmdSetViewport(m_vkCommandBuffer, 0, 1, &viewport);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetScissor(const u32 x, const u32 y, const u32 width,
                                                          const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkRect2D rect;
	rect.offset.x = NumericCast<i32>(x);
	rect.offset.y = NumericCast<i32>(y);
	rect.extent.width = width;
	rect.extent.height = height;

	vkCmdSetScissor(m_vkCommandBuffer, 0, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Draw(const u32 vertexCount, const u32 firstVertex) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, 1u, firstVertex, 0u);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawInstanced(const u32 vertexCount, const u32 firstVertex,
                                                             const u32 instanceCount,
                                                             const u32 firstInstance) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexed(const u32 indexCount, const u32 firstIndex,
                                                           const i32 vertexOffset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, 1, firstIndex, vertexOffset, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexedInstanced(const u32 indexCount,
																    const u32 firstIndex,
                                                                    const u32 instanceCount,
                                                                    const u32 firstInstance,
                                                                    const i32 vertexOffset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ExecuteIndirect(const TRAP::Ref<CommandSignature>& cmdSignature,
                                                               const u32 maxCommandCount,
                                                               const TRAP::Ref<Buffer>& indirectBuffer,
                                                               const u64 bufferOffset,
                                                               const TRAP::Ref<Buffer>& counterBuffer,
                                                               const u64 counterBufferOffset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(cmdSignature, "VulkanCommandBuffer::ExecuteIndirect(): CommandSignature is nullptr!");
	TRAP_ASSERT(indirectBuffer, "VulkanCommandBuffer::ExecuteIndirect(): IndirectBuffer is nullptr!");

	const Ref<VulkanBuffer> iBuffer = std::dynamic_pointer_cast<VulkanBuffer>(indirectBuffer);
	const Ref<VulkanCommandSignature> cSig = std::dynamic_pointer_cast<VulkanCommandSignature>(cmdSignature);

	if (cSig->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDraw)
	{
		if (counterBuffer)
		{
			vkCmdDrawIndirectCountKHR(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset,
				                      std::dynamic_pointer_cast<VulkanBuffer>(counterBuffer)->GetVkBuffer(),
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
				                             std::dynamic_pointer_cast<VulkanBuffer>(counterBuffer)->GetVkBuffer(),
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

void TRAP::Graphics::API::VulkanCommandBuffer::Dispatch(const u32 groupCountX, const u32 groupCountY,
                                                        const u32 groupCountZ) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	vkCmdDispatch(m_vkCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateBuffer(const TRAP::Ref<Buffer>& buffer,
															const u64 dstOffset,
                                                            const TRAP::Ref<Buffer>& srcBuffer,
                                                            const u64 srcOffset, const u64 size) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const Ref<VulkanBuffer> sBuffer = std::dynamic_pointer_cast<VulkanBuffer>(srcBuffer);
	const Ref<VulkanBuffer> buf = std::dynamic_pointer_cast<VulkanBuffer>(buffer);

	TRAP_ASSERT(srcBuffer, "VulkanCommandBuffer::UpdateBuffer(): Source Buffer is nullptr!");
	TRAP_ASSERT(sBuffer->GetVkBuffer(), "VulkanCommandBuffer::UpdateBuffer(): Source Vulkan Buffer is nullptr!");
	TRAP_ASSERT(buffer, "VulkanCommandBuffer::UpdateBuffer(): Destination Buffer is nullptr!");
	TRAP_ASSERT(buf->GetVkBuffer(), "VulkanCommandBuffer::UpdateBuffer(): Destination Vulkan Buffer is nullptr!");
	TRAP_ASSERT(srcOffset + size <= srcBuffer->GetSize(), "VulkanCommandBuffer::UpdateBuffer(): Source Buffer out of bounds!");
	TRAP_ASSERT(dstOffset + size <= buffer->GetSize(), "VulkanCommandBuffer::UpdateBuffer(): Destination Buffer out of bounds!");

	VkBufferCopy region{};
	region.srcOffset = srcOffset;
	region.dstOffset = dstOffset;
	region.size = static_cast<VkDeviceSize>(size);

	vkCmdCopyBuffer(m_vkCommandBuffer, sBuffer->GetVkBuffer(), buf->GetVkBuffer(), 1, &region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateSubresource(const TRAP::Graphics::Texture* const texture,
                                                                 const TRAP::Ref<Buffer>& srcBuffer,
                                                                 const RendererAPI::SubresourceDataDesc& subresourceDesc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	VkBuffer buffer = std::dynamic_pointer_cast<VulkanBuffer>(srcBuffer)->GetVkBuffer();
	const auto* const vkTexture = dynamic_cast<const TRAP::Graphics::API::VulkanTexture*>(texture);

	const TRAP::Graphics::API::ImageFormat fmt = texture->GetImageFormat();
	if(TRAP::Graphics::API::ImageFormatIsSinglePlane(fmt))
	{
		const u32 width = TRAP::Math::Max<u32>(1u, texture->GetWidth() >> subresourceDesc.MipLevel);
		const u32 height = TRAP::Math::Max<u32>(1u, texture->GetHeight() >> subresourceDesc.MipLevel);
		const u32 depth = TRAP::Math::Max<u32>(1u, texture->GetDepth() >> subresourceDesc.MipLevel);
		const u32 numBlocksWide = subresourceDesc.RowPitch / (TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt) >> 3u);
		const u32 numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);

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
		const u32 width = texture->GetWidth();
		const u32 height = texture->GetHeight();
		const u32 depth = texture->GetDepth();
		const u32 numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(fmt);

		u64 offset = subresourceDesc.SrcOffset;
		std::vector<VkBufferImageCopy> bufferImagesCopy(3);

		for(u32 i = 0; i < numOfPlanes; ++i)
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
			offset += NumericCast<u64>(copy.imageExtent.width) * copy.imageExtent.height *
			          TRAP::Graphics::API::ImageFormatPlaneSizeOfBlock(fmt, i);
		}

		vkCmdCopyBufferToImage(m_vkCommandBuffer, buffer, vkTexture->GetVkImage(),
		                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, numOfPlanes, bufferImagesCopy.data());
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::CopySubresource(const Buffer* const dstBuffer, const Texture* const texture,
                                                               const RendererAPI::SubresourceDataDesc& subresourceDesc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(dstBuffer, "VulkanCommandBuffer::CopySubresource(): Invalid Buffer!");
	TRAP_ASSERT(texture, "VulkanCommandBuffer::CopySubresource(): Invalid Texture!");

	const VulkanTexture* const vkTexture = dynamic_cast<const VulkanTexture*>(texture);
	const VulkanBuffer* const vkBuffer = dynamic_cast<const VulkanBuffer*>(dstBuffer);

	const ImageFormat format = texture->GetImageFormat();
	const bool isSinglePlane = ImageFormatIsSinglePlane(format);

	//TODO Currently all textures can only be single plane (as we dont support formats that have multiple textures inside)
	if(isSinglePlane)
	{
		const u32 width = TRAP::Math::Max(1u, texture->GetWidth() >> subresourceDesc.MipLevel);
		const u32 height = TRAP::Math::Max(1u, texture->GetHeight() >> subresourceDesc.MipLevel);
		const u32 depth = TRAP::Math::Max(1u, texture->GetDepth() >> subresourceDesc.MipLevel);
		const u32 numBlocksWide = subresourceDesc.RowPitch / (ImageFormatBitSizeOfBlock(format) >> 3u);
		const u32 numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);

		VkImageSubresourceLayers layers{};
		layers.aspectMask = texture->GetAspectMask();
		layers.mipLevel = subresourceDesc.MipLevel;
		layers.baseArrayLayer = subresourceDesc.ArrayLayer;
		layers.layerCount = 1;

		const VkBufferImageCopy copy = VulkanInits::ImageCopy(subresourceDesc.SrcOffset,
		                                                      numBlocksWide * ImageFormatWidthOfBlock(format),
															  numBlocksHigh * ImageFormatHeightOfBlock(format),
															  width, height, depth, layers);

		vkCmdCopyImageToBuffer(m_vkCommandBuffer, vkTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		                       vkBuffer->GetVkBuffer(), 1, &copy);
	}
	else
	{
		const u32 width = texture->GetWidth();
		const u32 height = texture->GetHeight();
		const u32 depth = texture->GetDepth();
		const u32 numOfPlanes = ImageFormatNumOfPlanes(format);

		u64 offset = subresourceDesc.SrcOffset;
		std::vector<VkBufferImageCopy> bufferImageCopies(numOfPlanes);

		for(u32 i = 0; i < numOfPlanes; ++i)
		{
			VkImageSubresourceLayers layers{};
			layers.aspectMask = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_PLANE_0_BIT << i);
			layers.mipLevel = subresourceDesc.MipLevel;
			layers.baseArrayLayer = subresourceDesc.ArrayLayer;
			layers.layerCount = 1;

			bufferImageCopies[i] = VulkanInits::ImageCopy(offset, 0, 0,
			                                              ImageFormatPlaneWidth(format, i, width),
														  ImageFormatPlaneHeight(format, i, height),
														  depth, layers);

			offset += NumericCast<u64>(bufferImageCopies[i].imageExtent.width) * bufferImageCopies[i].imageExtent.height *
			          ImageFormatPlaneSizeOfBlock(format, i);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResetQueryPool(const TRAP::Ref<QueryPool>& queryPool,
                                                              const u32 startQuery,
															  const u32 queryCount) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queryPool, "VulkanCommandBuffer::ResetQueryPool(): Invalid QueryPool!");

	vkCmdResetQueryPool(m_vkCommandBuffer, std::dynamic_pointer_cast<VulkanQueryPool>(queryPool)->GetVkQueryPool(),
	                    startQuery, queryCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginQuery(const TRAP::Ref<QueryPool>& queryPool,
                                                          const RendererAPI::QueryDesc& desc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queryPool, "VulkanCommandBuffer::BeginQuery(): Invalid QueryPool!");

	const Ref<VulkanQueryPool> qPool = std::dynamic_pointer_cast<VulkanQueryPool>(queryPool);

	const VkQueryType type = qPool->GetVkQueryType();
	switch(type)
	{
	case VK_QUERY_TYPE_TIMESTAMP:
		vkCmdWriteTimestamp(m_vkCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, qPool->GetVkQueryPool(),
		                    desc.Index);
		break;

	case VK_QUERY_TYPE_PIPELINE_STATISTICS:
		[[fallthrough]];
	case VK_QUERY_TYPE_OCCLUSION:
		[[fallthrough]];
	default:
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::EndQuery(const TRAP::Ref<QueryPool>& queryPool,
                                                        const RendererAPI::QueryDesc& desc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	BeginQuery(queryPool, desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResolveQuery(const TRAP::Ref<QueryPool>& queryPool,
                                                            const TRAP::Ref<Buffer>& readBackBuffer,
                                                            const u32 startQuery,
															const u32 queryCount) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(queryPool, "VulkanCommandBuffer::ResolveQuery(): Invalid QueryPool!");
	TRAP_ASSERT(readBackBuffer, "VulkanCommandBuffer::ResolveQuery(): Invalid ReadBackBuffer!");

	static constexpr VkQueryResultFlags flags = VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT;

	vkCmdCopyQueryPoolResults(m_vkCommandBuffer,
	                          std::dynamic_pointer_cast<VulkanQueryPool>(queryPool)->GetVkQueryPool(),
	                          startQuery, queryCount,
	                          std::dynamic_pointer_cast<VulkanBuffer>(readBackBuffer)->GetVkBuffer(),
	                          0, sizeof(u64), flags);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* const bufferBarriers,
	                                                           const std::vector<RendererAPI::TextureBarrier>* const textureBarriers,
	                                                           const std::vector<RendererAPI::RenderTargetBarrier>* const renderTargetBarriers) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(bufferBarriers || textureBarriers || renderTargetBarriers, "VulkanCommandBuffer::ResourceBarrier(): No barrier specified!");

	std::vector<VkImageMemoryBarrier> iBarriers;
	usize iBarriersCount = 0;
	if (textureBarriers != nullptr)
		iBarriersCount += textureBarriers->size();
	if(renderTargetBarriers != nullptr)
		iBarriersCount += renderTargetBarriers->size();
	iBarriers.resize(iBarriersCount);
	u32 imageBarrierCount = 0;

	std::vector<VkBufferMemoryBarrier> bBarriers;
	if (bufferBarriers != nullptr)
		bBarriers.resize(bufferBarriers->size());
	u32 bufferBarrierCount = 0;

	VkAccessFlags srcAccessFlags = 0;
	VkAccessFlags dstAccessFlags = 0;

	const Ref<VulkanQueue> queue = std::dynamic_pointer_cast<VulkanQueue>(m_queue);
	if(bufferBarriers != nullptr)
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

			if(bufferBarrier != nullptr)
			{
				bufferBarrier->buffer = std::dynamic_pointer_cast<VulkanBuffer>(buffer)->GetVkBuffer();
				bufferBarrier->size = VK_WHOLE_SIZE;
				bufferBarrier->offset = 0;

				if(trans.Acquire)
				{
					bufferBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(trans.QueueType)];
					bufferBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
				}
				else if(trans.Release)
				{
					bufferBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
					bufferBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(trans.QueueType)];
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

	if(textureBarriers != nullptr)
	{
		for (const auto& trans : *textureBarriers)
		{
			const TRAP::Graphics::Texture* const texture = trans.Texture;
			const auto* const vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(trans.Texture);
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

			if(imageBarrier != nullptr)
			{
				imageBarrier->image = vkTexture->GetVkImage();
				imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
				imageBarrier->subresourceRange.baseMipLevel = trans.SubresourceBarrier ? trans.MipLevel : 0;
				imageBarrier->subresourceRange.levelCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
				imageBarrier->subresourceRange.baseArrayLayer = trans.SubresourceBarrier ? trans.ArrayLayer : 0;
				imageBarrier->subresourceRange.layerCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

				if(trans.Acquire)
				{
					imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(trans.QueueType)];
					imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
				}
				else if(trans.Release)
				{
					imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
					imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(trans.QueueType)];
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

	if(renderTargetBarriers != nullptr)
	{
		for (const auto& trans : *renderTargetBarriers)
		{
			const Ref<TRAP::Graphics::Texture> texture = std::dynamic_pointer_cast<VulkanRenderTarget>(trans.RenderTarget)->m_texture;
			const Ref<VulkanTexture> vkTexture = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(texture);
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

			if(imageBarrier != nullptr)
			{
				imageBarrier->image = vkTexture->GetVkImage();
				imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
				imageBarrier->subresourceRange.baseMipLevel = trans.SubresourceBarrier ? trans.MipLevel : 0;
				imageBarrier->subresourceRange.levelCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
				imageBarrier->subresourceRange.baseArrayLayer = trans.SubresourceBarrier ? trans.ArrayLayer : 0;
				imageBarrier->subresourceRange.layerCount = trans.SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

				if(trans.Acquire)
				{
					imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(trans.QueueType)];
					imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
				}
				else if(trans.Release)
				{
					imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
					imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(trans.QueueType)];
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

	if((bufferBarrierCount != 0u) || (imageBarrierCount != 0u))
		vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, bufferBarrierCount,
		                     bBarriers.data(), imageBarrierCount, iBarriers.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const RendererAPI::BufferBarrier* const bufferBarrier,
	                                                           const RendererAPI::TextureBarrier* const textureBarrier,
	                                                           const RendererAPI::RenderTargetBarrier* const renderTargetBarrier) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(bufferBarrier || textureBarrier || renderTargetBarrier, "VulkanCommandBuffer::ResourceBarrier(): No barrier specified!");

	std::vector<VkImageMemoryBarrier> iBarriers;
	usize iBarriersCount = 0;
	if (textureBarrier != nullptr)
		iBarriersCount++;
	if(renderTargetBarrier != nullptr)
		iBarriersCount++;
	iBarriers.resize(iBarriersCount);
	u32 imageBarrierCount = 0;

	VkBufferMemoryBarrier bBarrier;

	VkAccessFlags srcAccessFlags = 0;
	VkAccessFlags dstAccessFlags = 0;

	const Ref<VulkanQueue> queue = std::dynamic_pointer_cast<VulkanQueue>(m_queue);
	if(bufferBarrier != nullptr)
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

		bBarrier.buffer = std::dynamic_pointer_cast<VulkanBuffer>(buffer)->GetVkBuffer();
		bBarrier.size = VK_WHOLE_SIZE;
		bBarrier.offset = 0;

		if(bufferBarrier->Acquire)
		{
			bBarrier.srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(bufferBarrier->QueueType)];
			bBarrier.dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
		}
		else if(bufferBarrier->Release)
		{
			bBarrier.srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
			bBarrier.dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(bufferBarrier->QueueType)];
		}
		else
		{
			bBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		}

		srcAccessFlags |= bBarrier.srcAccessMask;
		dstAccessFlags |= bBarrier.dstAccessMask;
	}

	if(textureBarrier != nullptr)
	{
		const TRAP::Graphics::Texture* const texture = textureBarrier->Texture;
		const auto* const vkTexture = dynamic_cast<const TRAP::Graphics::API::VulkanTexture*>(texture);
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

		if(imageBarrier != nullptr)
		{
			imageBarrier->image = vkTexture->GetVkImage();
			imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
			imageBarrier->subresourceRange.baseMipLevel = textureBarrier->SubresourceBarrier ? textureBarrier->MipLevel : 0;
			imageBarrier->subresourceRange.levelCount = textureBarrier->SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
			imageBarrier->subresourceRange.baseArrayLayer = textureBarrier->SubresourceBarrier ? textureBarrier->ArrayLayer : 0;
			imageBarrier->subresourceRange.layerCount = textureBarrier->SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

			if(textureBarrier->Acquire)
			{
				imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(textureBarrier->QueueType)];
				imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
			}
			else if(textureBarrier->Release)
			{
				imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
				imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(textureBarrier->QueueType)];
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

	if(renderTargetBarrier != nullptr)
	{
		const Ref<TRAP::Graphics::Texture> texture = std::dynamic_pointer_cast<VulkanRenderTarget>(renderTargetBarrier->RenderTarget)->m_texture;
		const Ref<VulkanTexture> vkTexture = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(texture);
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

		if(imageBarrier != nullptr)
		{
			imageBarrier->image = vkTexture->GetVkImage();
			imageBarrier->subresourceRange.aspectMask = texture->GetAspectMask();
			imageBarrier->subresourceRange.baseMipLevel = renderTargetBarrier->SubresourceBarrier ? renderTargetBarrier->MipLevel : 0;
			imageBarrier->subresourceRange.levelCount = renderTargetBarrier->SubresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS;
			imageBarrier->subresourceRange.baseArrayLayer = renderTargetBarrier->SubresourceBarrier ? renderTargetBarrier->ArrayLayer : 0;
			imageBarrier->subresourceRange.layerCount = renderTargetBarrier->SubresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS;

			if(renderTargetBarrier->Acquire)
			{
				imageBarrier->srcQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(renderTargetBarrier->QueueType)];
				imageBarrier->dstQueueFamilyIndex = queue->GetQueueFamilyIndex();
			}
			else if(renderTargetBarrier->Release)
			{
				imageBarrier->srcQueueFamilyIndex = queue->GetQueueFamilyIndex();
				imageBarrier->dstQueueFamilyIndex = m_device->GetQueueFamilyIndices()[std::to_underlying(renderTargetBarrier->QueueType)];
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

	if((bufferBarrier != nullptr) || (imageBarrierCount != 0u))
		vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, bufferBarrier != nullptr ? 1 : 0,
		                     &bBarrier, imageBarrierCount, iBarriers.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetStencilReferenceValue(const u32 val) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	vkCmdSetStencilReference(m_vkCommandBuffer, VK_STENCIL_FRONT_AND_BACK, val);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetShadingRate(const RendererAPI::ShadingRate shadingRate,
															  const RendererAPI::ShadingRateCombiner postRasterizerState,
															  const RendererAPI::ShadingRateCombiner finalRate) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(RendererAPI::GPUSettings.ShadingRateCaps != RendererAPI::ShadingRateCaps::NotSupported,
	            "VulkanCommandBuffer::SetShadingRate(): Shading rate is not supported by this device!");

	if(RendererAPI::GPUSettings.ShadingRateCaps == RendererAPI::ShadingRateCaps::NotSupported) //VRS is not supported
		return;

	const std::array<VkFragmentShadingRateCombinerOpKHR, 2> combiner
	{
		ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(postRasterizerState),
		ShadingRateCombinerToVkFragmentShadingRateCombinerOpKHR(finalRate)
	};

	const VkExtent2D fragmentSize = ShadingRateToVkExtent2D(shadingRate);

	vkCmdSetFragmentShadingRateKHR(m_vkCommandBuffer, &fragmentSize, combiner.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const RendererAPI::Color& color, const u32 width,
													 const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	attachment.clearValue.color =
	{
		{
			NumericCast<f32>(color.Red),
			NumericCast<f32>(color.Green),
			NumericCast<f32>(color.Blue),
			NumericCast<f32>(color.Alpha)
		}
	};

	vkCmdClearAttachments(m_vkCommandBuffer, 1, &attachment, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const f32 depth, const u32 stencil,
 												     const u32 width, const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const f32 depth, const u32 width, const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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

void TRAP::Graphics::API::VulkanCommandBuffer::Clear(const u32 stencil, const u32 width,
                                                     const u32 height) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

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
	attachment.clearValue.depthStencil = { 0.0f, stencil };

	vkCmdClearAttachments(m_vkCommandBuffer, 1, &attachment, 1, &rect);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResolveImage(const Ref<TRAP::Graphics::API::VulkanTexture>& srcImage,
															const RendererAPI::ResourceState srcState,
															const Ref<TRAP::Graphics::API::VulkanTexture>& dstImage,
															const RendererAPI::ResourceState dstState) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(srcImage, "VulkanCommandBuffer::ResolveImage(): Invalid Source Image!");
	TRAP_ASSERT(dstImage, "VulkanCommandBuffer::ResolveImage(): Invalid Destination Image!");

	VkImageResolve imageResolve{};
	imageResolve.srcSubresource = {srcImage->GetAspectMask(), 0, 0, 1};
	imageResolve.dstSubresource = {dstImage->GetAspectMask(), 0, 0, 1};
	imageResolve.extent = {srcImage->GetWidth(), srcImage->GetHeight(), srcImage->GetDepth()};

	vkCmdResolveImage(m_vkCommandBuffer, srcImage->GetVkImage(), ResourceStateToVkImageLayout(srcState),
	                  dstImage->GetVkImage(), ResourceStateToVkImageLayout(dstState), 1, &imageResolve);
}

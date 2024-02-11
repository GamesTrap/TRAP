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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanCommandBufferPrefix, "Destroying CommandBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(m_device && (m_vkCommandBuffer != nullptr) && (m_vkCommandPool != nullptr))
		vkFreeCommandBuffers(m_device->GetVkDevice(), m_vkCommandPool, 1, &m_vkCommandBuffer);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanCommandBuffer::VulkanCommandBuffer(TRAP::Ref<VulkanDevice> device, TRAP::Ref<Queue> queue,
                                                              VkCommandPool commandPool, const bool secondary)
	: CommandBuffer(std::move(queue)),
	  m_device(std::move(device)),
	  m_vkCommandPool(commandPool),
	  m_secondary(secondary)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstants(const RootSignature& rootSignature,
                                                                 const std::string_view name,
																 const std::span<const u8> constants) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(!name.empty(), "VulkanCommandBuffer::BindPushConstants(): Name is empty");
	TRAP_ASSERT(!constants.empty(), "VulkanCommandBuffer::BindPushConstants(): Constants are empty");

	const VulkanRootSignature* const rSig = dynamic_cast<const VulkanRootSignature*>(&rootSignature);
	TRAP_ASSERT(rSig, "VulkanCommandBuffer::BindPushConstants(): Failed to convert RootSignature to VulkanRootSignature");

	const RendererAPI::DescriptorInfo* const desc = rSig->GetDescriptor(name);

	if(desc == nullptr)
	{
		TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Unable to find PushConstants with name: ", name, "!");
		return;
	}

	TRAP_ASSERT(desc->Type == RendererAPI::DescriptorType::RootConstant, "VulkanCommandBuffer::BindPushConstants(): Descriptor is not a RootConstant!");
	TRAP_ASSERT(desc->Size == constants.size(), "VulkanCommandBuffer::BindPushConstants(): Size of constants don't match that of Descriptor!");

	vkCmdPushConstants(m_vkCommandBuffer, rSig->GetVkPipelineLayout(), desc->VkStages, 0, desc->Size, constants.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPushConstantsByIndex(const RootSignature& rootSignature,
	                                                                    const u32 paramIndex,
																		const std::span<const u8> constants) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanRootSignature* const rSig = dynamic_cast<const VulkanRootSignature*>(&rootSignature);

	TRAP_ASSERT(rSig, "VulkanCommandBuffer::BindPushConstantsByIndex(): Failed to convert RootSignature to VulkanRootSignature");
	TRAP_ASSERT(!constants.empty(), "VulkanCommandBuffer::BindPushConstantsByIndex(): Constants are empty");

	const auto& descriptors = rSig->GetDescriptors();
	TRAP_ASSERT(paramIndex < descriptors.size(), "VulkanCommandBuffer::BindPushConstantsByIndex(): Index out of bounds!");

	const RendererAPI::DescriptorInfo& desc = rSig->GetDescriptors()[paramIndex];

	TRAP_ASSERT(desc.Type == RendererAPI::DescriptorType::RootConstant, "VulkanCommandBuffer::BindPushConstantsByIndex(): Descriptor is not a RootConstant!");
	TRAP_ASSERT(desc.Size == constants.size(), "VulkanCommandBuffer::BindPushConstantsByIndex(): Size of constants don't match that of Descriptor!");

	vkCmdPushConstants(m_vkCommandBuffer, rSig->GetVkPipelineLayout(), desc.VkStages, 0, desc.Size, constants.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindDescriptorSet(const u32 index, DescriptorSet& descriptorSet)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
			if(rootSignature->GetVkEmptyDescriptorSets()[setIndex] == VK_NULL_HANDLE)
				continue;

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

void TRAP::Graphics::API::VulkanCommandBuffer::BindIndexBuffer(const Buffer& buffer,
                                                               const RendererAPI::IndexType indexType,
															   const u64 offset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanBuffer* const vkBuffer = dynamic_cast<const VulkanBuffer*>(&buffer);
	TRAP_ASSERT(vkBuffer, "VulkanCommandBuffer::BindIndexBuffer(): Failed to convert Buffer to VulkanBuffer!");

	const VkIndexType vkIndexType = ((RendererAPI::IndexType::UInt16 == indexType) ? VK_INDEX_TYPE_UINT16 :
	                                                                                 VK_INDEX_TYPE_UINT32);
	vkCmdBindIndexBuffer(m_vkCommandBuffer, vkBuffer->GetVkBuffer(), offset, vkIndexType);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindVertexBuffer(const std::vector<std::reference_wrapper<Buffer>>& buffers,
                                                                const std::vector<u32>& strides,
                                                                const std::vector<u64>& offsets) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(!buffers.empty(), "VulkanCommandBuffer::BindVertexBuffer(): Vertex buffers are empty!");
	TRAP_ASSERT(!strides.empty(), "VulkanCommandBuffer::BindVertexBuffer(): Strides are empty!");

	const u32 cappedBufferCount = TRAP::Math::Min(NumericCast<u32>(buffers.size()), RendererAPI::GPUSettings.MaxVertexInputBindings);

	//Capped the number of buffers to the maximum allowed by the device
	std::vector<VkBuffer> vkBuffers(cappedBufferCount);
	std::vector<VkDeviceSize> vkOffsets(cappedBufferCount);

	for(usize i = 0; i < vkBuffers.size(); ++i)
	{
		const VulkanBuffer* const vkBuffer = dynamic_cast<const VulkanBuffer*>(&buffers[i].get());
		TRAP_ASSERT(vkBuffer, "VulkanCommandBuffer::BindVertexBuffer(): Failed to convert Buffer to VulkanBuffer!");

		vkBuffers[i] = vkBuffer->GetVkBuffer();
		vkOffsets[i] = (offsets.size() > i) ? offsets[i] : 0;
	}

	vkCmdBindVertexBuffers(m_vkCommandBuffer, 0, cappedBufferCount, vkBuffers.data(), vkOffsets.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BindPipeline(const Pipeline& pipeline) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanPipeline* const vkPipeline = dynamic_cast<const VulkanPipeline*>(&pipeline);
	TRAP_ASSERT(vkPipeline, "VulkanCommandBuffer::BindPipeline(): Failed to convert Pipeline to VulkanPipeline!");

	const VkPipelineBindPoint pipelineBindPoint = VkPipelineBindPointTranslator[std::to_underlying(vkPipeline->GetPipelineType())];
	vkCmdBindPipeline(m_vkCommandBuffer, pipelineBindPoint, vkPipeline->GetVkPipeline());
}

//-------------------------------------------------------------------------------------------------------------------//

namespace
{
	template<typename T>
	[[nodiscard]] constexpr usize HashAlg(const std::span<const T> mem, const usize prev = 2166136261U)
	{
		TRAP_ASSERT(!mem.empty(), "VulkanCommandBuffer::HashAlg(): mem is empty!");

		//Intentionally u32 instead of usize, so the behavior is the same regardless of size.
		u32 result = static_cast<u32>(prev);

		for(const T& value : mem)
			result = static_cast<u32>((result * 16777619) ^ value);

		return NumericCast<usize>(result);
	}

	template<typename T>
	requires (!TRAP::Utils::IsStdSpan<T>::value && std::integral<T>)
	[[nodiscard]] constexpr usize HashAlg(const T data, const usize prev = 2166136261U)
	{
		return HashAlg<T>({&data, 1}, prev);
	}

	void EndActiveRenderPass(VkRenderPass& renderPass, VkCommandBuffer cmdBuffer)
	{
		if(renderPass == nullptr)
			return;

		vkCmdEndRenderPass(cmdBuffer);
		renderPass = nullptr;
	}

	[[nodiscard]] VkRenderPass BeginRenderPass(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	                                           const VkRect2D& renderArea, const std::vector<VkClearValue>& clearValues,
											   VkCommandBuffer cmdBuffer)
	{
		using namespace TRAP::Graphics::API::VulkanInits;

		const VkRenderPassBeginInfo beginInfo = RenderPassBeginInfo(renderPass, frameBuffer, renderArea, clearValues);
		vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

		return renderPass;
	}

	//Generate hash for RenderPass
	//NOTE:
	//RenderPass does not care about underlying VkImageView.
	//It only cares about the format and sample count of the attachments.
	//We hash those two values to generate the RenderPass Hash.
	[[nodiscard]] usize CalculateRenderPassHash(const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& renderTargets,
												const TRAP::Graphics::RendererAPI::LoadActionsDesc* const loadActions,
												const TRAP::Graphics::RenderTarget* const depthStencil,
												const TRAP::Graphics::RenderTarget* const shadingRate,
												std::vector<TRAP::Graphics::RendererAPI::StoreActionType>& outColorStoreActions,
												TRAP::Graphics::RendererAPI::StoreActionType& outDepthStoreAction,
												TRAP::Graphics::RendererAPI::StoreActionType& outStencilStoreAction)
	{
		using namespace TRAP::Graphics;

		const u32 clampedRenderTargets = TRAP::Math::Min(NumericCast<u32>(renderTargets.size()), RendererAPI::GPUSettings.MaxColorRenderTargets);

		outColorStoreActions = std::vector<RendererAPI::StoreActionType>(clampedRenderTargets);

		usize renderPassHash = 2166136261U;

		for(usize i = 0; i < clampedRenderTargets; ++i)
		{
			const TRAP::Ref<API::VulkanTexture> vkTex = std::dynamic_pointer_cast<API::VulkanTexture>(renderTargets[i]->GetTexture());
			if(vkTex->IsLazilyAllocated())
				outColorStoreActions[i] = RendererAPI::StoreActionType::DontCare;
			else if(loadActions != nullptr && loadActions->StoreActionsColor.size() > i)
				outColorStoreActions[i] = loadActions->StoreActionsColor[i];
			else
				outColorStoreActions[i] = RendererAPI::StoreActionType::DontCare;

			const std::array<u32, 4> hashValues =
			{
				std::to_underlying(renderTargets[i]->GetImageFormat()),
				NumericCast<u32>(std::to_underlying(renderTargets[i]->GetSampleCount())),
				loadActions != nullptr && loadActions->StoreActionsColor.size() > i ? std::to_underlying(loadActions->LoadActionsColor[i]) : 0u,
				std::to_underlying(outColorStoreActions[i])
			};

			renderPassHash = HashAlg<u32>(hashValues, renderPassHash);
		}

		if(depthStencil != nullptr)
		{
			const TRAP::Ref<API::VulkanTexture> vkTex = std::dynamic_pointer_cast<API::VulkanTexture>(depthStencil->GetTexture());

			if(vkTex->IsLazilyAllocated())
			{
				outDepthStoreAction = RendererAPI::StoreActionType::DontCare;
				outStencilStoreAction = RendererAPI::StoreActionType::DontCare;
			}
			else if(loadActions != nullptr)
			{
				outDepthStoreAction = loadActions->StoreActionDepth;
				outStencilStoreAction = loadActions->StoreActionStencil;
			}

			const std::array<u32, 6> hashValues =
			{
				std::to_underlying(depthStencil->GetImageFormat()),
				NumericCast<u32>(std::to_underlying(depthStencil->GetSampleCount())),
				loadActions != nullptr ? std::to_underlying(loadActions->LoadActionDepth) : 0u,
				loadActions != nullptr ? std::to_underlying(loadActions->LoadActionStencil) : 0u,
				std::to_underlying(outDepthStoreAction),
				std::to_underlying(outStencilStoreAction)
			};
			renderPassHash = HashAlg<u32>(hashValues, renderPassHash);
		}

		if(shadingRate != nullptr)
			renderPassHash = HashAlg(std::to_underlying(shadingRate->GetImageFormat()), renderPassHash);

		return renderPassHash;
	}

	//Generate hash for FrameBuffer
	//NOTE:
	//FrameBuffer is the actual array of all the VkImageViews
	//We hash the texture id associated with the RenderTarget to generate the FrameBuffer Hash.
	[[nodiscard]] usize CalculateFrameBufferHash(const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& renderTargets,
												 const TRAP::Graphics::RenderTarget* const depthStencil,
												 const std::vector<u32>* const colorArraySlices,
												 const std::vector<u32>* const colorMipSlices,
												 const u32 depthArraySlice,
												 const u32 depthMipSlice,
												 const TRAP::Graphics::RenderTarget* const shadingRate)
	{
		using namespace TRAP::Graphics;
		using namespace TRAP::Graphics::API;

		const u32 clampedRenderTargets = TRAP::Math::Min(NumericCast<u32>(renderTargets.size()), RendererAPI::GPUSettings.MaxColorRenderTargets);

		usize frameBufferHash = 2166136261U;

		for(u32 i = 0; i < clampedRenderTargets; ++i)
		{
			const u64 id = dynamic_cast<const VulkanRenderTarget*>(renderTargets[i].get())->GetID();
			frameBufferHash = HashAlg(id, frameBufferHash);
		}

		if(depthStencil != nullptr)
		{
			const u64 id = dynamic_cast<const VulkanRenderTarget*>(depthStencil)->GetID();
			frameBufferHash = HashAlg(id, frameBufferHash);
		}

		if (colorArraySlices != nullptr)
			frameBufferHash = HashAlg<u32>({colorArraySlices->data(), renderTargets.size()}, frameBufferHash);

		if (colorMipSlices != nullptr)
			frameBufferHash = HashAlg<u32>({colorMipSlices->data(), renderTargets.size()}, frameBufferHash);

		if (depthArraySlice != std::numeric_limits<u32>::max())
			frameBufferHash = HashAlg(depthArraySlice, frameBufferHash);

		if (depthMipSlice != std::numeric_limits<u32>::max())
			frameBufferHash = HashAlg(depthMipSlice, frameBufferHash);

		if(shadingRate != nullptr)
		{
			const u64 id = dynamic_cast<const VulkanRenderTarget*>(shadingRate)->GetID();
			frameBufferHash = HashAlg(id, frameBufferHash);
		}

		return frameBufferHash;
	}

	[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanFrameBuffer> GetCachedVulkanFrameBuffer(const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& renderTargets,
	                                                                                           const TRAP::Ref<TRAP::Graphics::RenderTarget>& depthStencil,
																							   const std::vector<u32>* const colorArraySlices,
																                               const std::vector<u32>* const colorMipSlices,
																							   const u32 depthArraySlice,
																                               const u32 depthMipSlice,
																							   const TRAP::Ref<TRAP::Graphics::RenderTarget>& shadingRate,
																							   const TRAP::Ref<TRAP::Graphics::API::VulkanRenderPass>& renderPass,
																							   const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device)
	{
		using namespace TRAP::Graphics::API;
		using namespace TRAP::Graphics;

		const u32 clampedRenderTargets = TRAP::Math::Min(NumericCast<u32>(renderTargets.size()), RendererAPI::GPUSettings.MaxColorRenderTargets);

		const usize frameBufferHash = CalculateFrameBufferHash(renderTargets, depthStencil.get(),
														       colorArraySlices, colorMipSlices,
														       depthArraySlice, depthMipSlice,
														       shadingRate.get());

		VulkanRenderer::FrameBufferMap& frameBufferMap = VulkanRenderer::GetFrameBufferMap();
		TRAP::Ref<VulkanFrameBuffer> frameBuffer = nullptr;

		//If a FrameBuffer of this combination already exists just use it or create a new one
		if (const auto frameBufferNode = frameBufferMap.find(frameBufferHash); frameBufferNode != frameBufferMap.end())
			frameBuffer = frameBufferNode->second;
		else
		{
			const VulkanRenderer::FrameBufferDesc desc
			{
				.RenderPass = renderPass,
				.RenderTargets = std::vector<TRAP::Ref<RenderTarget>>(renderTargets.begin(), renderTargets.begin() + clampedRenderTargets),
				.DepthStencil = depthStencil,
				.ShadingRate = shadingRate,
				.ColorArraySlices = colorArraySlices != nullptr ? *colorArraySlices : std::vector<u32>{},
				.ColorMipSlices = colorMipSlices != nullptr ? *colorMipSlices : std::vector<u32>{},
				.DepthArraySlice = depthArraySlice,
				.DepthMipSlice = depthMipSlice
			};
			frameBuffer = TRAP::MakeRef<VulkanFrameBuffer>(device, desc);

			//No need of a lock here since this map is per thread
			frameBufferMap.insert({ {frameBufferHash, frameBuffer} });
		}

		return frameBuffer;
	}

	[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanRenderPass> GetCachedVulkanRenderPass(const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& renderTargets,
	                                                                                         const TRAP::Graphics::RendererAPI::LoadActionsDesc* const loadActions,
																							 const TRAP::Graphics::RenderTarget* const depthStencil,
																							 const TRAP::Graphics::RenderTarget* const shadingRate,
																							 std::vector<TRAP::Graphics::RendererAPI::StoreActionType>& outColorStoreActions,
																							 TRAP::Graphics::RendererAPI::StoreActionType& outDepthStoreAction,
																							 TRAP::Graphics::RendererAPI::StoreActionType& outStencilStoreAction,
																							 const TRAP::Ref<TRAP::Graphics::API::VulkanDevice>& device)
	{
		using namespace TRAP::Graphics::API;
		using namespace TRAP::Graphics;

		const u32 clampedRenderTargets = TRAP::Math::Min(NumericCast<u32>(renderTargets.size()), RendererAPI::GPUSettings.MaxColorRenderTargets);

		const usize renderPassHash = CalculateRenderPassHash(renderTargets, loadActions, depthStencil,
														     shadingRate, outColorStoreActions,
														     outDepthStoreAction, outStencilStoreAction);

		VulkanRenderer::RenderPassMap& renderPassMap = VulkanRenderer::GetRenderPassMap();
		TRAP::Ref<VulkanRenderPass> renderPass = nullptr;

		//If a RenderPass of this combination already exists just use it or create a new one
		if (const auto renderPassNode = renderPassMap.find(renderPassHash); renderPassNode != renderPassMap.end())
			renderPass = renderPassNode->second;
		else
		{
			std::vector<ImageFormat> colorFormats(clampedRenderTargets);
			for (u32 i = 0; i < clampedRenderTargets; ++i)
				colorFormats[i] = renderTargets[i]->GetImageFormat();

			const ImageFormat depthStencilFormat = depthStencil != nullptr ? depthStencil->GetImageFormat() : ImageFormat::Undefined;

			RendererAPI::SampleCount sampleCount = RendererAPI::SampleCount::One;
			if (depthStencil != nullptr)
				sampleCount = depthStencil->GetSampleCount();
			else if (!renderTargets.empty())
				sampleCount = renderTargets.front()->GetSampleCount();

			const ImageFormat shadingRateFormat = shadingRate != nullptr ? shadingRate->GetImageFormat() : ImageFormat::Undefined;

			std::vector<RendererAPI::LoadActionType> loadActionsColor(clampedRenderTargets);
			if(loadActions != nullptr)
			{
				for(u32 i = 0; i < clampedRenderTargets; ++i)
				{
					if(loadActions->LoadActionsColor.size() > i)
						loadActionsColor[i] = loadActions->LoadActionsColor[i];
				}
			}

			const VulkanRenderer::RenderPassDesc desc
			{
				.ColorFormats = colorFormats,
				.LoadActionsColor = loadActionsColor,
				.StoreActionsColor = outColorStoreActions,
				.RenderTargetCount = clampedRenderTargets,
				.SampleCount = sampleCount,
				.DepthStencilFormat = depthStencilFormat,
				.ShadingRateFormat = shadingRateFormat,
				.LoadActionDepth = loadActions != nullptr ? loadActions->LoadActionDepth : RendererAPI::LoadActionType::DontCare,
				.LoadActionStencil = loadActions != nullptr ? loadActions->LoadActionStencil : RendererAPI::LoadActionType::DontCare,
				.StoreActionDepth = outDepthStoreAction,
				.StoreActionStencil = outStencilStoreAction
			};
			renderPass = TRAP::MakeRef<VulkanRenderPass>(device, desc);

			//No need of a lock here since this map is per thread
			renderPassMap.insert({ {renderPassHash, renderPass} });
		}

		return renderPass;
	}

	[[nodiscard]] std::vector<VkClearValue> GetClearValues(const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& renderTargets,
	                                                       const TRAP::Graphics::RenderTarget* const depthStencil,
														   const TRAP::Graphics::RendererAPI::LoadActionsDesc* const loadActions)
	{
		const u32 clampedColorRenderTargets = TRAP::Math::Min(NumericCast<u32>(renderTargets.size()), TRAP::Graphics::RendererAPI::GPUSettings.MaxColorRenderTargets);

		std::vector<VkClearValue> clearValues(clampedColorRenderTargets + 1); //Color render targets + depth/stencil render target

		if(loadActions == nullptr)
			return clearValues;

		for(usize i = 0; i < clampedColorRenderTargets; ++i)
		{
			const VkClearValue val
			{
				.color = TRAP::Graphics::API::VulkanInits::ClearColorValue(loadActions->ClearColorValues[i], renderTargets[i]->GetImageFormat())
			};

			clearValues[i] = val;
		}

		if(depthStencil != nullptr)
		{
			const VkClearValue val
			{
				.depthStencil =
				{
					loadActions->ClearDepthStencil.Depth,
					loadActions->ClearDepthStencil.Stencil
				}
			};

			clearValues.back() = val;
		}

		return clearValues;
	}
}

void TRAP::Graphics::API::VulkanCommandBuffer::BindRenderTargets(const std::vector<TRAP::Ref<RenderTarget>>& renderTargets,
																 const TRAP::Ref<RenderTarget>& depthStencil,
																 RendererAPI::LoadActionsDesc* const loadActions,
																 const std::vector<u32>* const colorArraySlices,
																 const std::vector<u32>* const colorMipSlices,
																 const u32 depthArraySlice,
																 const u32 depthMipSlice,
																 const TRAP::Ref<RenderTarget>& shadingRate)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(renderTargets.size() <= RendererAPI::GPUSettings.MaxColorRenderTargets, "VulkanCommandBuffer::BindRenderTargets(): Trying to bind more render targets than GPU supports!");
	if (renderTargets.size() > 4) //Guaranteed minimum from spec for limits.maxColorAttachments
		TP_WARN(Log::RendererVulkanCommandBufferPrefix, "Using more than the guaranteed maximum amount of color render targets for render pass!");
	if (renderTargets.size() > RendererAPI::GPUSettings.MaxColorRenderTargets)
		TP_ERROR(Log::RendererVulkanCommandBufferPrefix, "Trying to bind more render targets than GPU supports! Clamping to GPU maximum");

	EndActiveRenderPass(m_activeRenderPass, m_vkCommandBuffer);

	if (renderTargets.empty() && !depthStencil)
		return;

	const u32 clampedColorRenderTargets = TRAP::Math::Min(NumericCast<u32>(renderTargets.size()), RendererAPI::GPUSettings.MaxColorRenderTargets);

	if(loadActions != nullptr)
	{
		if(loadActions->LoadActionsColor.size() < clampedColorRenderTargets)
			loadActions->LoadActionsColor.resize(clampedColorRenderTargets);
		if(loadActions->StoreActionsColor.size() < clampedColorRenderTargets)
			loadActions->StoreActionsColor.resize(clampedColorRenderTargets);
		if(loadActions->ClearColorValues.size() < clampedColorRenderTargets)
			loadActions->ClearColorValues.resize(clampedColorRenderTargets);
	}

	std::vector<RendererAPI::StoreActionType> colorStoreActions{};
	RendererAPI::StoreActionType depthStoreAction{};
	RendererAPI::StoreActionType stencilStoreAction{};

	const TRAP::Ref<VulkanRenderPass> renderPass = GetCachedVulkanRenderPass(renderTargets, loadActions,
	                                                                         depthStencil.get(), shadingRate.get(),
																			 colorStoreActions, depthStoreAction,
																			 stencilStoreAction, m_device);

	const TRAP::Ref<VulkanFrameBuffer> frameBuffer = GetCachedVulkanFrameBuffer(renderTargets, depthStencil, colorArraySlices,
	                                                                            colorMipSlices, depthArraySlice, depthMipSlice,
																		        shadingRate, renderPass, m_device);

	const VkRect2D renderArea
	{
		.offset = {},
		.extent = {.width = frameBuffer->GetWidth(), .height = frameBuffer->GetHeight()}
	};

	const std::vector<VkClearValue> clearValues = GetClearValues(renderTargets, depthStencil.get(), loadActions);

	m_activeRenderPass = BeginRenderPass(renderPass->GetVkRenderPass(), frameBuffer->GetVkFrameBuffer(),
	                                     renderArea, clearValues, m_vkCommandBuffer);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::AddDebugMarker(const TRAP::Math::Vec3& color, const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VkCommandBufferBeginInfo beginInfo = VulkanInits::CommandBufferBeginInfo(oneTimeSubmit);

	VkCall(vkBeginCommandBuffer(m_vkCommandBuffer, &beginInfo));

	//Reset CPU side data
	m_boundPipelineLayout = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::End()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, 1u, firstVertex, 0u);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawInstanced(const u32 vertexCount, const u32 firstVertex,
                                                             const u32 instanceCount,
                                                             const u32 firstInstance) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	vkCmdDraw(m_vkCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexed(const u32 indexCount, const u32 firstIndex,
                                                           const i32 vertexOffset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, 1, firstIndex, vertexOffset, 0);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::DrawIndexedInstanced(const u32 indexCount,
																    const u32 firstIndex,
                                                                    const u32 instanceCount,
                                                                    const u32 firstInstance,
                                                                    const i32 vertexOffset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	vkCmdDrawIndexed(m_vkCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ExecuteIndirect(const CommandSignature& cmdSignature,
                                                               const u32 maxCommandCount,
                                                               const Buffer& indirectBuffer,
                                                               const u64 bufferOffset,
                                                               const Buffer* const counterBuffer,
                                                               const u64 counterBufferOffset) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanCommandSignature* const cSig = dynamic_cast<const VulkanCommandSignature*>(&cmdSignature);
	const VulkanBuffer* const iBuffer = dynamic_cast<const VulkanBuffer*>(&indirectBuffer);

	TRAP_ASSERT(cSig, "VulkanCommandBuffer::ExecuteIndirect(): Failed to convert CommandSignature to VulkanCommandSignature!");
	TRAP_ASSERT(iBuffer, "VulkanCommandBuffer::ExecuteIndirect(): Failed to convert Buffer to VulkanBuffer!");

	if (cSig->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDraw)
	{
		if (counterBuffer != nullptr)
		{
			const VulkanBuffer* const cBuffer = dynamic_cast<const VulkanBuffer*>(counterBuffer);
			TRAP_ASSERT(cBuffer, "VulkanCommandBuffer::ExecuteIndirect(): Failed to convert Buffer to VulkanBuffer!");

			vkCmdDrawIndirectCountKHR(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset,
				                      cBuffer->GetVkBuffer(), counterBufferOffset, maxCommandCount,
									  cSig->GetStride());
		}
		else
			vkCmdDrawIndirect(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset, maxCommandCount,
			                  cSig->GetStride());
	}
	else if (cSig->GetDrawType() == RendererAPI::IndirectArgumentType::IndirectDrawIndex)
	{
		if (counterBuffer != nullptr)
		{
			const VulkanBuffer* const cBuffer = dynamic_cast<const VulkanBuffer*>(counterBuffer);
			TRAP_ASSERT(cBuffer, "VulkanCommandBuffer::ExecuteIndirect(): Failed to convert Buffer to VulkanBuffer!");

			vkCmdDrawIndexedIndirectCountKHR(m_vkCommandBuffer, iBuffer->GetVkBuffer(), bufferOffset,
				                             cBuffer->GetVkBuffer(), counterBufferOffset, maxCommandCount,
											 cSig->GetStride());
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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	vkCmdDispatch(m_vkCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateBuffer(const Buffer& buffer, const u64 dstOffset,
                                                            const Buffer& srcBuffer, const u64 srcOffset,
															const u64 size) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanBuffer* const sBuffer = dynamic_cast<const VulkanBuffer*>(&srcBuffer);
	const VulkanBuffer* const dBuffer = dynamic_cast<const VulkanBuffer*>(&buffer);

	TRAP_ASSERT(sBuffer, "VulkanCommandBuffer::UpdateBuffer(): Failed to convert Buffer to VulkanBuffer!");
	TRAP_ASSERT(dBuffer, "VulkanCommandBuffer::UpdateBuffer(): Failed to convert Buffer to VulkanBuffer!");
	TRAP_ASSERT(sBuffer->GetVkBuffer(), "VulkanCommandBuffer::UpdateBuffer(): Source Vulkan Buffer is nullptr!");
	TRAP_ASSERT(dBuffer->GetVkBuffer(), "VulkanCommandBuffer::UpdateBuffer(): Destination Vulkan Buffer is nullptr!");
	TRAP_ASSERT(size != 0, "VulkanCommandBuffer::UpdateBuffer(): Size to copy is 0!");
	TRAP_ASSERT(srcOffset + size <= sBuffer->GetSize(), "VulkanCommandBuffer::UpdateBuffer(): Source Buffer out of bounds!");
	TRAP_ASSERT(dstOffset + size <= dBuffer->GetSize(), "VulkanCommandBuffer::UpdateBuffer(): Destination Buffer out of bounds!");

	VkBufferCopy region{};
	region.srcOffset = srcOffset;
	region.dstOffset = dstOffset;
	region.size = static_cast<VkDeviceSize>(size);

	vkCmdCopyBuffer(m_vkCommandBuffer, sBuffer->GetVkBuffer(), dBuffer->GetVkBuffer(), 1, &region);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::UpdateSubresource(const TRAP::Graphics::Texture& texture,
                                                                 const Buffer& srcBuffer,
                                                                 const RendererAPI::SubresourceDataDesc& subresourceDesc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanTexture* const vkDstTexture = dynamic_cast<const VulkanTexture*>(&texture);
	const VulkanBuffer* const vkSrcBuffer = dynamic_cast<const VulkanBuffer*>(&srcBuffer);

	TRAP_ASSERT(vkDstTexture, "VulkanCommandBuffer::UpdateSubresource(): Failed to convert Texture to VulkanTexture!");
	TRAP_ASSERT(vkSrcBuffer, "VulkanCommandBuffer::UpdateSubresource(): Failed to convert Buffer to VulkanBuffer!");

	const TRAP::Graphics::API::ImageFormat fmt = vkDstTexture->GetImageFormat();
	if(TRAP::Graphics::API::ImageFormatIsSinglePlane(fmt))
	{
		const u32 width = TRAP::Math::Max<u32>(1u, vkDstTexture->GetWidth() >> subresourceDesc.MipLevel);
		const u32 height = TRAP::Math::Max<u32>(1u, vkDstTexture->GetHeight() >> subresourceDesc.MipLevel);
		const u32 depth = TRAP::Math::Max<u32>(1u, vkDstTexture->GetDepth() >> subresourceDesc.MipLevel);
		const u32 numBlocksWide = subresourceDesc.RowPitch / (TRAP::Graphics::API::ImageFormatBitSizeOfBlock(fmt) >> 3u);
		const u32 numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);

		VkBufferImageCopy copy{};
		copy.bufferOffset = subresourceDesc.SrcOffset;
		copy.bufferRowLength = numBlocksWide * TRAP::Graphics::API::ImageFormatWidthOfBlock(fmt);
		copy.bufferImageHeight = numBlocksHigh * TRAP::Graphics::API::ImageFormatHeightOfBlock(fmt);
		copy.imageSubresource.aspectMask = vkDstTexture->GetAspectMask();
		copy.imageSubresource.mipLevel = subresourceDesc.MipLevel;
		copy.imageSubresource.baseArrayLayer = subresourceDesc.ArrayLayer;
		copy.imageSubresource.layerCount = 1;
		copy.imageOffset.x = 0;
		copy.imageOffset.y = 0;
		copy.imageOffset.z = 0;
		copy.imageExtent.width = width;
		copy.imageExtent.height = height;
		copy.imageExtent.depth = depth;

		vkCmdCopyBufferToImage(m_vkCommandBuffer, vkSrcBuffer->GetVkBuffer(), vkDstTexture->GetVkImage(),
		                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
		return;
	}

	const u32 width = vkDstTexture->GetWidth();
	const u32 height = vkDstTexture->GetHeight();
	const u32 depth = vkDstTexture->GetDepth();
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

	vkCmdCopyBufferToImage(m_vkCommandBuffer, vkSrcBuffer->GetVkBuffer(), vkDstTexture->GetVkImage(),
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, numOfPlanes, bufferImagesCopy.data());
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::CopySubresource(const Buffer& dstBuffer, const Texture& texture,
                                                               const RendererAPI::SubresourceDataDesc& subresourceDesc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanBuffer* const vkDstBuffer = dynamic_cast<const VulkanBuffer*>(&dstBuffer);
	const VulkanTexture* const vkSrcTexture = dynamic_cast<const VulkanTexture*>(&texture);

	TRAP_ASSERT(vkDstBuffer, "VulkanCommandBuffer::CopySubresource(): Failed to convert Buffer to VulkanBuffer!");
	TRAP_ASSERT(vkSrcTexture, "VulkanCommandBuffer::CopySubresource(): Failed to convert Texture to VulkanTexture!");

	const ImageFormat format = vkSrcTexture->GetImageFormat();

	//TODO Currently all textures can only be single plane (as we dont support formats that have multiple textures inside)
	if(ImageFormatIsSinglePlane(format))
	{
		const u32 width = TRAP::Math::Max(1u, vkSrcTexture->GetWidth() >> subresourceDesc.MipLevel);
		const u32 height = TRAP::Math::Max(1u, vkSrcTexture->GetHeight() >> subresourceDesc.MipLevel);
		const u32 depth = TRAP::Math::Max(1u, vkSrcTexture->GetDepth() >> subresourceDesc.MipLevel);
		const u32 numBlocksWide = subresourceDesc.RowPitch / (ImageFormatBitSizeOfBlock(format) >> 3u);
		const u32 numBlocksHigh = (subresourceDesc.SlicePitch / subresourceDesc.RowPitch);

		VkImageSubresourceLayers layers{};
		layers.aspectMask = vkSrcTexture->GetAspectMask();
		layers.mipLevel = subresourceDesc.MipLevel;
		layers.baseArrayLayer = subresourceDesc.ArrayLayer;
		layers.layerCount = 1;

		const VkBufferImageCopy copy = VulkanInits::ImageCopy(subresourceDesc.SrcOffset,
		                                                      numBlocksWide * ImageFormatWidthOfBlock(format),
															  numBlocksHigh * ImageFormatHeightOfBlock(format),
															  width, height, depth, layers);

		vkCmdCopyImageToBuffer(m_vkCommandBuffer, vkSrcTexture->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		                       vkDstBuffer->GetVkBuffer(), 1, &copy);
		return;
	}

	const u32 width = vkSrcTexture->GetWidth();
	const u32 height = vkSrcTexture->GetHeight();
	const u32 depth = vkSrcTexture->GetDepth();
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

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResetQueryPool(const QueryPool& queryPool,
                                                              const u32 startQuery,
															  const u32 queryCount) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanQueryPool* const vkQueryPool = dynamic_cast<const VulkanQueryPool*>(&queryPool);
	TRAP_ASSERT(vkQueryPool, "VulkanCommandBuffer::ResetQueryPool(): Failed to convert QueryPool to VulkanQueryPool!");

	vkCmdResetQueryPool(m_vkCommandBuffer, vkQueryPool->GetVkQueryPool(), startQuery, queryCount);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::BeginQuery(const QueryPool& queryPool,
                                                          const RendererAPI::QueryDesc& desc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanQueryPool* const qPool = dynamic_cast<const VulkanQueryPool*>(&queryPool);
	TRAP_ASSERT(qPool, "VulkanCommandBuffer::BeginQuery(): Failed to convert QueryPool to VulkanQueryPool!");

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

void TRAP::Graphics::API::VulkanCommandBuffer::EndQuery(const QueryPool& queryPool,
                                                        const RendererAPI::QueryDesc& desc) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	BeginQuery(queryPool, desc);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResolveQuery(const QueryPool& queryPool,
                                                            const Buffer& readBackBuffer,
                                                            const u32 startQuery,
															const u32 queryCount) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const VulkanQueryPool* const vkQueryPool = dynamic_cast<const VulkanQueryPool*>(&queryPool);
	const VulkanBuffer* const vkReadBackBuffer = dynamic_cast<const VulkanBuffer*>(&readBackBuffer);

	TRAP_ASSERT(vkQueryPool, "VulkanCommandBuffer::ResolveQuery(): Failed to convert QueryPool to VulkanQueryPool!");
	TRAP_ASSERT(vkReadBackBuffer, "VulkanCommandBuffer::ResolveQuery(): Failed to convert Buffer to VulkanBuffer!");

	static constexpr VkQueryResultFlags flags = VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT;

	vkCmdCopyQueryPoolResults(m_vkCommandBuffer, vkQueryPool->GetVkQueryPool(), startQuery, queryCount,
	                          vkReadBackBuffer->GetVkBuffer(), 0, sizeof(u64), flags);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::ResourceBarrier(const std::vector<RendererAPI::BufferBarrier>* const bufferBarriers,
	                                                           const std::vector<RendererAPI::TextureBarrier>* const textureBarriers,
	                                                           const std::vector<RendererAPI::RenderTargetBarrier>* const renderTargetBarriers) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(bufferBarriers || textureBarriers || renderTargetBarriers, "VulkanCommandBuffer::ResourceBarrier(): No barrier specified!");
	if(bufferBarriers == nullptr && textureBarriers == nullptr && renderTargetBarriers == nullptr)
		return;

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(bufferBarrier || textureBarrier || renderTargetBarrier, "VulkanCommandBuffer::ResourceBarrier(): No barrier specified!");
	if(bufferBarrier == nullptr && textureBarrier == nullptr && renderTargetBarrier == nullptr)
		return;

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	vkCmdSetStencilReference(m_vkCommandBuffer, VK_STENCIL_FRONT_AND_BACK, val);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanCommandBuffer::SetShadingRate(const RendererAPI::ShadingRate shadingRate,
															  const RendererAPI::ShadingRateCombiner postRasterizerState,
															  const RendererAPI::ShadingRateCombiner finalRate) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

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

void TRAP::Graphics::API::VulkanCommandBuffer::ResolveImage(const TRAP::Graphics::API::VulkanTexture& srcImage,
															const RendererAPI::ResourceState srcState,
															const TRAP::Graphics::API::VulkanTexture& dstImage,
															const RendererAPI::ResourceState dstState) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	VkImageResolve imageResolve{};
	imageResolve.srcSubresource = {srcImage.GetAspectMask(), 0, 0, 1};
	imageResolve.dstSubresource = {dstImage.GetAspectMask(), 0, 0, 1};
	imageResolve.extent = {srcImage.GetWidth(), srcImage.GetHeight(), srcImage.GetDepth()};

	vkCmdResolveImage(m_vkCommandBuffer, srcImage.GetVkImage(), ResourceStateToVkImageLayout(srcState),
	                  dstImage.GetVkImage(), ResourceStateToVkImageLayout(dstState), 1, &imageResolve);
}

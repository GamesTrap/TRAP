#include "TRAPPCH.h"
#include "VulkanRenderPass.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "Utils/Optional.h"

namespace
{
	struct VulkanRenderPassAttachments
	{
		std::vector<VkAttachmentDescription> Attachments{};
		std::vector<VkAttachmentReference> ColorAttachmentReferences{};
		TRAP::Optional<VkAttachmentReference> DepthStencilAttachmentReference{};
	};

	void FillRenderPassColorAttachments(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc,
	                                    VulkanRenderPassAttachments& outRenderPassAttachments)
	{
		const VkSampleCountFlagBits sampleCount = TRAP::Graphics::API::SampleCountToVkSampleCount(desc.SampleCount);

		for(u32 i = 0; i < outRenderPassAttachments.ColorAttachmentReferences.size(); ++i)
		{
			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			if(!desc.LoadActionsColor.empty())
				loadOp = TRAP::Graphics::API::VkAttachmentLoadOpTranslator(desc.LoadActionsColor[i]);
			VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			if(!desc.StoreActionsColor.empty())
				storeOp = TRAP::Graphics::API::VkAttachmentStoreOpTranslator(desc.StoreActionsColor[i]);

			//Descriptions
			outRenderPassAttachments.Attachments[i] = TRAP::Graphics::API::VulkanInits::AttachmentDescription(ImageFormatToVkFormat(desc.ColorFormats[i]),
																	                                          sampleCount,
																	                                          loadOp,
																	                                          storeOp,
																	                                          VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																	                                          VK_ATTACHMENT_STORE_OP_STORE,
																	                                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																	                                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			);

			//References
			outRenderPassAttachments.ColorAttachmentReferences[i] = VkAttachmentReference
			{
				.attachment = i,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};
		}
	}

	void FillRenderPassDepthStencilAttachment(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc,
	                                          VulkanRenderPassAttachments& outRenderPassAttachments)
	{
		if(desc.DepthStencilFormat == TRAP::Graphics::API::ImageFormat::Undefined)
			return;

		const VkSampleCountFlagBits sampleCount = TRAP::Graphics::API::SampleCountToVkSampleCount(desc.SampleCount);

		const u32 idx = NumericCast<u32>(outRenderPassAttachments.ColorAttachmentReferences.size());
		outRenderPassAttachments.Attachments[idx] = TRAP::Graphics::API::VulkanInits::AttachmentDescription(ImageFormatToVkFormat(desc.DepthStencilFormat),
																											sampleCount,
																											TRAP::Graphics::API::VkAttachmentLoadOpTranslator(desc.LoadActionDepth),
																											TRAP::Graphics::API::VkAttachmentStoreOpTranslator(desc.StoreActionDepth),
																											TRAP::Graphics::API::VkAttachmentLoadOpTranslator(desc.LoadActionStencil),
																											TRAP::Graphics::API::VkAttachmentStoreOpTranslator(desc.StoreActionStencil),
																											VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																											VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);

		outRenderPassAttachments.DepthStencilAttachmentReference = VkAttachmentReference
		{
			.attachment = idx,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
	}

	[[nodiscard]] VulkanRenderPassAttachments GetRenderPassAttachments(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc)
	{
		const u32 colorAttachmentCount = desc.RenderTargetCount;
		const u32 depthAttachmentCount = desc.DepthStencilFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;

		VulkanRenderPassAttachments renderPassAttachments{};

		//Fill out attachment descriptions and references
		renderPassAttachments.Attachments.resize(colorAttachmentCount + depthAttachmentCount);

		if(colorAttachmentCount > 0)
			renderPassAttachments.ColorAttachmentReferences.resize(colorAttachmentCount);

		FillRenderPassColorAttachments(desc, renderPassAttachments);
		FillRenderPassDepthStencilAttachment(desc, renderPassAttachments);

		return renderPassAttachments;
	}

	[[nodiscard]] VkRenderPass CreateRenderPass(const TRAP::Graphics::API::VulkanDevice& device,
	                                            const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc)
	{
		VkRenderPass renderPass = VK_NULL_HANDLE;

		const VulkanRenderPassAttachments renderPassAttachments = GetRenderPassAttachments(desc);

		VkSubpassDescription subpass{};
		if(renderPassAttachments.DepthStencilAttachmentReference)
		{
			subpass = TRAP::Graphics::API::VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {},
		                                                                   renderPassAttachments.ColorAttachmentReferences,
													                       *renderPassAttachments.DepthStencilAttachmentReference);
		}
		else
			subpass = TRAP::Graphics::API::VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, renderPassAttachments.ColorAttachmentReferences);

		const VkRenderPassCreateInfo info = TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(renderPassAttachments.Attachments, subpass);

		VkCall(vkCreateRenderPass(device.GetVkDevice(), &info, nullptr, &renderPass));

		return renderPass;
	}

	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//

	struct VulkanRenderPassAttachments2
	{
		std::vector<VkAttachmentDescription2KHR> Attachments{};
		std::vector<VkAttachmentReference2KHR> ColorAttachmentReferences{};
		TRAP::Optional<VkAttachmentReference2KHR> DepthStencilAttachmentReference{};
		TRAP::Optional<VkFragmentShadingRateAttachmentInfoKHR> ShadingRateAttachmentInfo{};
		TRAP::Optional<VkAttachmentReference2KHR> ShadingRateAttachmentReference{};
	};

	void FillRenderPassColorAttachments2(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc,
	                                     VulkanRenderPassAttachments2& outRenderPassAttachments)
	{
		const VkSampleCountFlagBits sampleCount = TRAP::Graphics::API::SampleCountToVkSampleCount(desc.SampleCount);

		for(u32 i = 0; i < outRenderPassAttachments.ColorAttachmentReferences.size(); ++i)
		{
			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			if(!desc.LoadActionsColor.empty())
				loadOp = TRAP::Graphics::API::VkAttachmentLoadOpTranslator(desc.LoadActionsColor[i]);
			VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			if(!desc.StoreActionsColor.empty())
				storeOp = TRAP::Graphics::API::VkAttachmentStoreOpTranslator(desc.StoreActionsColor[i]);

			//Descriptions
			outRenderPassAttachments.Attachments[i] = TRAP::Graphics::API::VulkanInits::AttachmentDescription2(ImageFormatToVkFormat(desc.ColorFormats[i]),
																	                                           sampleCount,
																	                                           loadOp,
																	                                           storeOp,
																	                                           VK_ATTACHMENT_LOAD_OP_DONT_CARE,
																	                                           VK_ATTACHMENT_STORE_OP_STORE,
																	                                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
																	                                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			);

			//References
			outRenderPassAttachments.ColorAttachmentReferences[i] = VkAttachmentReference2KHR
			{
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR,
				.pNext = nullptr,
				.attachment = i,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.aspectMask = VK_IMAGE_ASPECT_NONE
			};
		}
	}

	void FillRenderPassDepthStencilAttachment2(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc,
	                                           VulkanRenderPassAttachments2& outRenderPassAttachments)
	{
		if(desc.DepthStencilFormat == TRAP::Graphics::API::ImageFormat::Undefined)
			return;

		const VkSampleCountFlagBits sampleCount = TRAP::Graphics::API::SampleCountToVkSampleCount(desc.SampleCount);

		const u32 idx = NumericCast<u32>(outRenderPassAttachments.ColorAttachmentReferences.size());
		outRenderPassAttachments.Attachments[idx] = TRAP::Graphics::API::VulkanInits::AttachmentDescription2(ImageFormatToVkFormat(desc.DepthStencilFormat),
																											 sampleCount,
																											 TRAP::Graphics::API::VkAttachmentLoadOpTranslator(desc.LoadActionDepth),
																											 TRAP::Graphics::API::VkAttachmentStoreOpTranslator(desc.StoreActionDepth),
																											 TRAP::Graphics::API::VkAttachmentLoadOpTranslator(desc.LoadActionStencil),
																											 TRAP::Graphics::API::VkAttachmentStoreOpTranslator(desc.StoreActionStencil),
																											 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
																											 VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);

		outRenderPassAttachments.DepthStencilAttachmentReference = VkAttachmentReference2KHR
		{
			.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR,
			.pNext = nullptr,
			.attachment = idx,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.aspectMask = VK_IMAGE_ASPECT_NONE
		};
	}

	void FillRenderPassShadingRateAttachment2(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc,
	                                          VulkanRenderPassAttachments2& outRenderPassAttachments)
	{
		if((TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateCaps & TRAP::Graphics::RendererAPI::ShadingRateCaps::PerTile) == TRAP::Graphics::RendererAPI::ShadingRateCaps::NotSupported ||
		   (desc.ShadingRateFormat == TRAP::Graphics::API::ImageFormat::Undefined))
		{
			return;
		}

		const u32 idx = NumericCast<u32>(outRenderPassAttachments.ColorAttachmentReferences.size()) + (outRenderPassAttachments.DepthStencilAttachmentReference ? 1 : 0);
		outRenderPassAttachments.Attachments[idx] = TRAP::Graphics::API::VulkanInits::AttachmentDescription2(ImageFormatToVkFormat(desc.ShadingRateFormat),
																											 VK_SAMPLE_COUNT_1_BIT,
																											 VK_ATTACHMENT_LOAD_OP_LOAD,
																											 VK_ATTACHMENT_STORE_OP_DONT_CARE,
																											 VK_ATTACHMENT_LOAD_OP_CLEAR,
																											 VK_ATTACHMENT_STORE_OP_DONT_CARE,
																											 VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
																											 VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR
		);

		outRenderPassAttachments.ShadingRateAttachmentReference = VkAttachmentReference2KHR
		{
			.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR,
			.pNext = nullptr,
			.attachment = idx,
			.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
			.aspectMask = VK_IMAGE_ASPECT_NONE
		};

		outRenderPassAttachments.ShadingRateAttachmentInfo = TRAP::Graphics::API::VulkanInits::FragmentShadingRateAttachmentInfo(*outRenderPassAttachments.ShadingRateAttachmentReference,
																																 {TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelWidth,
																																  TRAP::Graphics::RendererAPI::GPUSettings.ShadingRateTexelHeight});
	}

	[[nodiscard]] VulkanRenderPassAttachments2 GetRenderPassAttachments2(const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc)
	{
		VulkanRenderPassAttachments2 renderPassAttachments{};

		const u32 colorAttachmentCount = desc.RenderTargetCount;
		const u32 depthAttachmentCount = desc.DepthStencilFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;
		const u32 shadingRateAttachmentCount = desc.ShadingRateFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;

		//Fill out attachment descriptions and references
		renderPassAttachments.Attachments.resize(colorAttachmentCount + depthAttachmentCount + shadingRateAttachmentCount);

		if(colorAttachmentCount > 0)
			renderPassAttachments.ColorAttachmentReferences.resize(colorAttachmentCount);

		FillRenderPassColorAttachments2(desc, renderPassAttachments);
		FillRenderPassDepthStencilAttachment2(desc, renderPassAttachments);
		FillRenderPassShadingRateAttachment2(desc, renderPassAttachments);

		return renderPassAttachments;
	}

	[[nodiscard]] VkRenderPass CreateRenderPass2(const TRAP::Graphics::API::VulkanDevice& device,
	                                             const TRAP::Graphics::API::VulkanRenderer::RenderPassDesc& desc)
	{
		VkRenderPass renderPass = VK_NULL_HANDLE;

		const VulkanRenderPassAttachments2 renderPassAttachments = GetRenderPassAttachments2(desc);

		const VkFragmentShadingRateAttachmentInfoKHR* shadingRateAttachmentInfo = nullptr;
		if(renderPassAttachments.ShadingRateAttachmentInfo)
			shadingRateAttachmentInfo = &renderPassAttachments.ShadingRateAttachmentInfo.Value();

		VkSubpassDescription2KHR subpass{};
		if(renderPassAttachments.DepthStencilAttachmentReference)
		{
			subpass = TRAP::Graphics::API::VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {},
		                                                                   renderPassAttachments.ColorAttachmentReferences,
													                       *renderPassAttachments.DepthStencilAttachmentReference,
																		   shadingRateAttachmentInfo);
		}
		else
		{
			subpass = TRAP::Graphics::API::VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {},
		                                                                   renderPassAttachments.ColorAttachmentReferences,
												                           shadingRateAttachmentInfo);
		}

		const VkRenderPassCreateInfo2KHR info = TRAP::Graphics::API::VulkanInits::RenderPassCreateInfo(renderPassAttachments.Attachments, subpass);

		VkCall(vkCreateRenderPass2KHR(device.GetVkDevice(), &info, nullptr, &renderPass));

		return renderPass;
	}
}

TRAP::Graphics::API::VulkanRenderPass::VulkanRenderPass(TRAP::Ref<VulkanDevice> device,
                                                        const VulkanRenderer::RenderPassDesc& desc)
	: m_colorFormats(desc.ColorFormats),
	  m_loadActionsColor(desc.LoadActionsColor),
	  m_renderTargetCount(desc.RenderTargetCount),
	  m_sampleCount(desc.SampleCount),
	  m_depthStencilFormat(desc.DepthStencilFormat),
	  m_loadActionDepth(desc.LoadActionDepth),
	  m_loadActionStencil(desc.LoadActionStencil),
	  m_device(std::move(device))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanRenderPass(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Creating RenderPass");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(VulkanRenderer::s_renderPass2)
		m_renderPass = CreateRenderPass2(*m_device, desc);
	else
		m_renderPass = CreateRenderPass(*m_device, desc);
	TRAP_ASSERT(m_renderPass, "VulkanRenderPass(): Vulkan RenderPass is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderPass::~VulkanRenderPass()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Destroying RenderPass");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyRenderPass(m_device->GetVkDevice(), m_renderPass, nullptr);
}

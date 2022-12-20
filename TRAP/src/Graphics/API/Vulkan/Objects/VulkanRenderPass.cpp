#include "TRAPPCH.h"
#include "VulkanRenderPass.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include <vulkan/vulkan_core.h>

TRAP::Graphics::API::VulkanRenderPass::VulkanRenderPass(TRAP::Ref<VulkanDevice> device,
                                                        const VulkanRenderer::RenderPassDesc& desc)
	: m_renderPass(VK_NULL_HANDLE),
	  m_colorFormats(desc.ColorFormats),
	  m_loadActionsColor(desc.LoadActionsColor),
	  m_renderTargetCount(desc.RenderTargetCount),
	  m_sampleCount(desc.SampleCount),
	  m_depthStencilFormat(desc.DepthStencilFormat),
	  m_loadActionDepth(desc.LoadActionDepth),
	  m_loadActionStencil(desc.LoadActionStencil),
	  m_device(std::move(device))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanRenderPass(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Creating RenderPass");
#endif

	if(VulkanRenderer::s_renderPass2)
		m_renderPass = CreateRenderPass2(m_device, desc);
	else
		m_renderPass = CreateRenderPass(m_device, desc);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderPass::~VulkanRenderPass()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_renderPass, "~VulkanRenderPass(): Vulkan RenderPass is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Destroying RenderPass");
#endif
	vkDestroyRenderPass(m_device->GetVkDevice(), m_renderPass, nullptr);
	m_renderPass = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass TRAP::Graphics::API::VulkanRenderPass::GetVkRenderPass() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_renderPass;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::API::ImageFormat>& TRAP::Graphics::API::VulkanRenderPass::GetColorFormats() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_colorFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::RendererAPI::LoadActionType>& TRAP::Graphics::API::VulkanRenderPass::GetLoadActionsColor() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_loadActionsColor;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderPass::GetRenderTargetCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_renderTargetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::API::VulkanRenderPass::GetSampleCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::VulkanRenderPass::GetDepthStencilFormat() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_depthStencilFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::LoadActionType TRAP::Graphics::API::VulkanRenderPass::GetLoadActionTypeDepth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_loadActionDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::LoadActionType TRAP::Graphics::API::VulkanRenderPass::GetLoadActionTypeStencil() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_loadActionStencil;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass TRAP::Graphics::API::VulkanRenderPass::CreateRenderPass(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::RenderPassDesc& desc)
{
	VkRenderPass renderPass = VK_NULL_HANDLE;

	const uint32_t colorAttachmentCount = desc.RenderTargetCount;
	const uint32_t depthAttachmentCount = desc.DepthStencilFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;

	std::vector<VkAttachmentDescription> attachments{};
	std::vector<VkAttachmentReference> colorAttachmentRefs{};
	std::vector<VkAttachmentReference> depthStencilAttachmentRefs{};

	const VkSampleCountFlagBits sampleCount = SampleCountToVkSampleCount(desc.SampleCount);

	//Fill out attachment descriptions and references
	{
		attachments.resize(colorAttachmentCount + depthAttachmentCount);
		TRAP_ASSERT(!attachments.empty(), "VulkanRenderPass(): No color or depth attachments");

		if(colorAttachmentCount > 0)
		{
			colorAttachmentRefs.resize(colorAttachmentCount);
			TRAP_ASSERT(!colorAttachmentRefs.empty(), "VulkanRenderPass(): No color attachments");
		}
		if(depthAttachmentCount > 0)
			depthStencilAttachmentRefs.resize(1);

		//Color
		for(uint32_t i = 0; i < colorAttachmentCount; ++i)
		{
			const uint32_t ssidx = i;

			//Descriptions
			attachments[ssidx] = VulkanInits::AttachmentDescription(ImageFormatToVkFormat(desc.ColorFormats[i]),
				                                                    sampleCount,
				                                                    !desc.LoadActionsColor.empty() ?
																	VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionsColor[i])] :
																	VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				                                                    !desc.StoreActionsColor.empty() ?
																	VkAttachmentStoreOpTranslator[static_cast<uint32_t>(desc.StoreActionsColor[i])] :
																	VK_ATTACHMENT_STORE_OP_STORE,
				                                                    VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				                                                    VK_ATTACHMENT_STORE_OP_STORE,
				                                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				                                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			);

			//References
			colorAttachmentRefs[i].attachment = ssidx;
			colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}

	//Depth stencil
	if(depthAttachmentCount > 0)
	{
		const uint32_t idx = colorAttachmentCount;
		attachments[idx] = VulkanInits::AttachmentDescription(ImageFormatToVkFormat(desc.DepthStencilFormat),
															  sampleCount,
															  VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionDepth)],
															  VkAttachmentStoreOpTranslator[static_cast<uint32_t>(desc.StoreActionDepth)],
															  VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionStencil)],
															  VkAttachmentStoreOpTranslator[static_cast<uint32_t>(desc.StoreActionStencil)],
															  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);

		depthStencilAttachmentRefs[0].attachment = idx;
		depthStencilAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkSubpassDescription subpass;
	if(depthAttachmentCount > 0)
		subpass = VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, colorAttachmentRefs,
												depthStencilAttachmentRefs[0]);
	else
		subpass = VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, colorAttachmentRefs);

	const VkRenderPassCreateInfo info = VulkanInits::RenderPassCreateInfo(attachments, subpass);

	VkCall(vkCreateRenderPass(device->GetVkDevice(), &info, nullptr, &renderPass));

	return renderPass;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass TRAP::Graphics::API::VulkanRenderPass::CreateRenderPass2(TRAP::Ref<VulkanDevice> device, const VulkanRenderer::RenderPassDesc& desc)
{
	VkRenderPass renderPass = VK_NULL_HANDLE;

	const uint32_t colorAttachmentCount = desc.RenderTargetCount;
	const uint32_t depthAttachmentCount = desc.DepthStencilFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;
	const uint32_t shadingRateAttachmentCount = desc.ShadingRateFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;

	std::vector<VkAttachmentDescription2KHR> attachments{};
	std::vector<VkAttachmentReference2KHR> colorAttachmentRefs{};
	std::vector<VkAttachmentReference2KHR> depthStencilAttachmentRefs{};

	const VkSampleCountFlagBits sampleCount = SampleCountToVkSampleCount(desc.SampleCount);

	//Fill out attachment descriptions and references
	{
		attachments.resize(colorAttachmentCount + depthAttachmentCount + shadingRateAttachmentCount);
		TRAP_ASSERT(!attachments.empty(), "VulkanRenderPass(): No color or depth attachments");

		if(colorAttachmentCount > 0)
		{
			colorAttachmentRefs.resize(colorAttachmentCount);
			TRAP_ASSERT(!colorAttachmentRefs.empty(), "VulkanRenderPass(): No color attachments");
		}
		if(depthAttachmentCount > 0)
			depthStencilAttachmentRefs.resize(1);

		//Color
		for(uint32_t i = 0; i < colorAttachmentCount; ++i)
		{
			const uint32_t ssidx = i;

			//Descriptions
			attachments[ssidx] = VulkanInits::AttachmentDescription2(ImageFormatToVkFormat(desc.ColorFormats[i]),
				                                                     sampleCount,
				                                                     !desc.LoadActionsColor.empty() ?
																	 VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionsColor[i])] :
																	 VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				                                                     !desc.StoreActionsColor.empty() ?
																	 VkAttachmentStoreOpTranslator[static_cast<uint32_t>(desc.StoreActionsColor[i])] :
																	 VK_ATTACHMENT_STORE_OP_STORE,
				                                                     VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				                                                     VK_ATTACHMENT_STORE_OP_STORE,
				                                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				                                                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			);

			//References
			colorAttachmentRefs[i].sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
			colorAttachmentRefs[i].pNext = nullptr;
			colorAttachmentRefs[i].attachment = ssidx;
			colorAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}

	//Depth stencil
	if(depthAttachmentCount > 0)
	{
		const uint32_t idx = colorAttachmentCount;
		attachments[idx] = VulkanInits::AttachmentDescription2(ImageFormatToVkFormat(desc.DepthStencilFormat),
															   sampleCount,
															   VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionDepth)],
															   VkAttachmentStoreOpTranslator[static_cast<uint32_t>(desc.StoreActionDepth)],
															   VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionStencil)],
															   VkAttachmentStoreOpTranslator[static_cast<uint32_t>(desc.StoreActionStencil)],
															   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
															   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);

		depthStencilAttachmentRefs[0].sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
		depthStencilAttachmentRefs[0].pNext = nullptr;
		depthStencilAttachmentRefs[0].attachment = idx;
		depthStencilAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkFragmentShadingRateAttachmentInfoKHR shadingRateAttachmentInfo{};
	VkAttachmentReference2KHR shadingRateAttachmentRef{};
	if(static_cast<bool>(RendererAPI::GPUSettings.ShadingRateCaps & RendererAPI::ShadingRateCaps::PerTile) && shadingRateAttachmentCount)
	{
		const uint32_t idx = colorAttachmentCount + depthAttachmentCount;
		attachments[idx] = VulkanInits::AttachmentDescription2(ImageFormatToVkFormat(desc.ShadingRateFormat),
															   VK_SAMPLE_COUNT_1_BIT,
															   VK_ATTACHMENT_LOAD_OP_LOAD,
															   VK_ATTACHMENT_STORE_OP_DONT_CARE,
															   VK_ATTACHMENT_LOAD_OP_CLEAR,
															   VK_ATTACHMENT_STORE_OP_DONT_CARE,
															   VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR,
															   VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR
		);

		shadingRateAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
		shadingRateAttachmentRef.pNext = nullptr;
		shadingRateAttachmentRef.attachment = colorAttachmentCount + depthAttachmentCount;
		shadingRateAttachmentRef.layout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
		shadingRateAttachmentInfo = VulkanInits::FragmentShadingRateAttachmentInfo(shadingRateAttachmentRef,
		                                                                           {RendererAPI::GPUSettings.ShadingRateTexelWidth, RendererAPI::GPUSettings.ShadingRateTexelHeight});
	}

	VkSubpassDescription2KHR subpass;
	if(depthAttachmentCount > 0)
		subpass = VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, colorAttachmentRefs,
												  depthStencilAttachmentRefs[0], shadingRateAttachmentCount ? &shadingRateAttachmentInfo : nullptr);
	else
		subpass = VulkanInits::SubPassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS, {}, colorAttachmentRefs,
		                                          shadingRateAttachmentCount ? &shadingRateAttachmentInfo : nullptr);

	const VkRenderPassCreateInfo2KHR info = VulkanInits::RenderPassCreateInfo(attachments, subpass);

	VkCall(vkCreateRenderPass2KHR(device->GetVkDevice(), &info, nullptr, &renderPass));

	return renderPass;
}
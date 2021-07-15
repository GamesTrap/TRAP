#include "TRAPPCH.h"
#include "VulkanRenderPass.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanRenderPass::VulkanRenderPass(TRAP::Ref<VulkanDevice> device,
                                                        const VulkanRenderer::RenderPassDesc& desc)
	: m_renderPass(VK_NULL_HANDLE),
	  m_colorFormats(desc.ColorFormats),
	  m_loadActionsColor(desc.LoadActionsColor),
	  m_SRGBValues(desc.SRGBValues),
	  m_renderTargetCount(desc.RenderTargetCount),
	  m_sampleCount(desc.SampleCount),
	  m_depthStencilFormat(desc.DepthStencilFormat),
	  m_loadActionDepth(desc.LoadActionDepth),
	  m_loadActionStencil(desc.LoadActionStencil),
	  m_device(std::move(device))
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Creating RenderPass");
#endif

	const uint32_t colorAttachmentCount = desc.RenderTargetCount;
	const uint32_t depthAttachmentCount = desc.DepthStencilFormat != TRAP::Graphics::API::ImageFormat::Undefined ? 1 : 0;

	std::vector<VkAttachmentDescription> attachments{};
	std::vector<VkAttachmentReference> colorAttachmentRefs{};
	std::vector<VkAttachmentReference> depthStencilAttachmentRefs{};

	const VkSampleCountFlagBits sampleCount = SampleCountToVkSampleCount(desc.SampleCount);

	//Fill out attachment descriptions and references
	{
		attachments.resize(colorAttachmentCount + depthAttachmentCount);
		TRAP_ASSERT(!attachments.empty());

		if(colorAttachmentCount > 0)
		{
			colorAttachmentRefs.resize(colorAttachmentCount);
			TRAP_ASSERT(!colorAttachmentRefs.empty());
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
			                                                                        VK_ATTACHMENT_STORE_OP_STORE,
			                                                                        VkAttachmentLoadOpTranslator[static_cast<uint32_t>(desc.LoadActionStencil)],
			                                                                        VK_ATTACHMENT_STORE_OP_STORE,
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

	VkRenderPassCreateInfo info = VulkanInits::RenderPassCreateInfo(attachments, subpass);

	VkCall(vkCreateRenderPass(m_device->GetVkDevice(), &info, nullptr, &m_renderPass));

	attachments.clear();
	colorAttachmentRefs.clear();
	depthStencilAttachmentRefs.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderPass::~VulkanRenderPass()
{
	TRAP_ASSERT(m_renderPass);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderPassPrefix, "Destroying RenderPass");
#endif
	vkDestroyRenderPass(m_device->GetVkDevice(), m_renderPass, nullptr);
	m_renderPass = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------//

VkRenderPass& TRAP::Graphics::API::VulkanRenderPass::GetVkRenderPass()
{
	return m_renderPass;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::API::ImageFormat>& TRAP::Graphics::API::VulkanRenderPass::GetColorFormats() const
{
	return m_colorFormats;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Graphics::RendererAPI::LoadActionType>& TRAP::Graphics::API::VulkanRenderPass::GetLoadActionsColor() const
{
	return m_loadActionsColor;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<bool>& TRAP::Graphics::API::VulkanRenderPass::GetSRGBValues() const
{
	return m_SRGBValues;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderPass::GetRenderTargetCount() const
{
	return m_renderTargetCount;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::API::VulkanRenderPass::GetSampleCount() const
{
	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::VulkanRenderPass::GetDepthStencilFormat() const
{
	return m_depthStencilFormat;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::LoadActionType TRAP::Graphics::API::VulkanRenderPass::GetLoadActionTypeDepth() const
{
	return m_loadActionDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::LoadActionType TRAP::Graphics::API::VulkanRenderPass::GetLoadActionTypeStencil() const
{
	return m_loadActionStencil;
}
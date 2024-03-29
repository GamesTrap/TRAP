#include "TRAPPCH.h"
#include "VulkanFrameBuffer.h"

#include "VulkanRenderTarget.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanRenderPass.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

TRAP::Graphics::API::VulkanFrameBuffer::VulkanFrameBuffer(TRAP::Ref<VulkanDevice> device,
                                                          const VulkanRenderer::FrameBufferDesc& desc)
	: m_device(std::move(device))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanFrameBuffer(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFrameBufferPrefix, "Creating FrameBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const u32 colorAttachmentCount = NumericCast<u32>(desc.RenderTargets.size());
	const u32 depthAttachmentCount = desc.DepthStencil ? 1 : 0;
	const u32 shadingRateAttachmentCount = desc.ShadingRate ? 1 : 0;

	if(colorAttachmentCount != 0u)
	{
		const Ref<VulkanRenderTarget> renderTarget = std::dynamic_pointer_cast<VulkanRenderTarget>(desc.RenderTargets[0]);
		m_width = renderTarget->GetWidth();
		m_height = renderTarget->GetHeight();
		if (!desc.ColorArraySlices.empty())
			m_arraySize = 1;
		else
			m_arraySize = renderTarget->GetArraySize();
	}
	else if(depthAttachmentCount != 0u)
	{
		m_width = desc.DepthStencil->GetWidth();
		m_height = desc.DepthStencil->GetHeight();
		if (desc.DepthArraySlice != std::numeric_limits<u32>::max())
			m_arraySize = 1;
		else
			m_arraySize = desc.DepthStencil->GetArraySize();
	}
	else
	{
		TRAP_ASSERT(false, "VulkanFrameBuffer(): No color or depth attachments");
	}

	if ((colorAttachmentCount != 0u) && desc.RenderTargets[0]->GetDepth() > 1)
		m_arraySize = desc.RenderTargets[0]->GetDepth();

	const u32 attachmentCount = colorAttachmentCount + depthAttachmentCount + shadingRateAttachmentCount;

	std::vector<VkImageView> imageViews(attachmentCount);
	auto iterAttachments = imageViews.begin();

	//Color
	for(usize i = 0; i < desc.RenderTargets.size(); i++)
	{
		const Ref<VulkanRenderTarget> rTarget = std::dynamic_pointer_cast<VulkanRenderTarget>(desc.RenderTargets[i]);
		if(desc.ColorMipSlices.empty() && desc.ColorArraySlices.empty())
		{
			*iterAttachments = rTarget->GetVkImageView();
			++iterAttachments;
		}
		else
		{
			u32 handle = 0;
			if (!desc.ColorMipSlices.empty())
			{
				if (!desc.ColorArraySlices.empty())
					handle = desc.ColorMipSlices[i] * desc.RenderTargets[i]->GetArraySize() +
					         desc.ColorArraySlices[i];
				else
					handle = desc.ColorMipSlices[i];
			}
			else if (!desc.ColorArraySlices.empty())
				handle = desc.ColorArraySlices[i];

			*iterAttachments = rTarget->GetVkImageViewSlices()[handle];
			++iterAttachments;
		}
	}

	//Depth/Stencil
	if(desc.DepthStencil)
	{
		const Ref<VulkanRenderTarget> rTarget = std::dynamic_pointer_cast<VulkanRenderTarget>(desc.DepthStencil);
		if(desc.DepthMipSlice == std::numeric_limits<u32>::max() &&
		   desc.DepthArraySlice == std::numeric_limits<u32>::max())
		{
			*iterAttachments = rTarget->GetVkImageView();
			++iterAttachments;
		}
		else
		{
			u32 handle = 0;
			if (desc.DepthMipSlice != std::numeric_limits<u32>::max())
			{
				if (desc.DepthArraySlice != std::numeric_limits<u32>::max())
					handle = desc.DepthMipSlice * desc.DepthStencil->GetArraySize() + desc.DepthArraySlice;
				else
					handle = desc.DepthMipSlice;
			}
			else if (desc.DepthArraySlice != std::numeric_limits<u32>::max())
				handle = desc.DepthArraySlice;

			*iterAttachments = rTarget->GetVkImageViewSlices()[handle];
			++iterAttachments;
		}
	}

	//Shading rate
	if(desc.ShadingRate)
	{
		const Ref<VulkanRenderTarget> rTarget = std::dynamic_pointer_cast<VulkanRenderTarget>(desc.ShadingRate);

		*iterAttachments = rTarget->GetVkImageView();
		++iterAttachments;
	}

	const VkFramebufferCreateInfo info = VulkanInits::FramebufferCreateInfo(desc.RenderPass->GetVkRenderPass(),
	                                                                        imageViews, m_width,
																			m_height, m_arraySize);
	VkCall(vkCreateFramebuffer(m_device->GetVkDevice(), &info, nullptr, &m_framebuffer));
	TRAP_ASSERT(m_framebuffer, "VulkanFrameBuffer(): Vulkan FrameBuffer is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer::~VulkanFrameBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFrameBufferPrefix, "Destroying FrameBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyFramebuffer(m_device->GetVkDevice(), m_framebuffer, nullptr);
}

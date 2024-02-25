#include "TRAPPCH.h"
#include "VulkanFrameBuffer.h"

#include "VulkanRenderTarget.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanRenderPass.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

namespace
{
	[[nodiscard]] std::vector<VkImageView> GetImageViews(const TRAP::Graphics::API::VulkanRenderer::FrameBufferDesc& desc)
	{
		const u32 colorAttachmentCount = NumericCast<u32>(desc.RenderTargets.size());
		const u32 depthAttachmentCount = desc.DepthStencil ? 1 : 0;
		const u32 shadingRateAttachmentCount = desc.ShadingRate ? 1 : 0;

		std::vector<VkImageView> imageViews{};
		imageViews.reserve(colorAttachmentCount + depthAttachmentCount + shadingRateAttachmentCount);

		//Color
		for(usize i = 0; i < desc.RenderTargets.size(); i++)
		{
			const auto rTarget = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanRenderTarget>(desc.RenderTargets[i]);

			if(desc.ColorMipSlices.empty() && desc.ColorArraySlices.empty())
				imageViews.emplace_back(rTarget->GetVkImageView());
			else
			{
				u32 handle = 0;
				if (!desc.ColorMipSlices.empty())
				{
					if (!desc.ColorArraySlices.empty())
						handle = desc.ColorMipSlices[i] * desc.RenderTargets[i]->GetArraySize() + desc.ColorArraySlices[i];
					else
						handle = desc.ColorMipSlices[i];
				}
				else if (!desc.ColorArraySlices.empty())
					handle = desc.ColorArraySlices[i];

				imageViews.emplace_back(rTarget->GetVkImageViewSlices()[handle]);
			}
		}

		//Depth/Stencil
		if(desc.DepthStencil)
		{
			const auto rTarget = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanRenderTarget>(desc.DepthStencil);

			if(desc.DepthMipSlice == std::numeric_limits<u32>::max() &&
			   desc.DepthArraySlice == std::numeric_limits<u32>::max())
				imageViews.emplace_back(rTarget->GetVkImageView());
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

				imageViews.emplace_back(rTarget->GetVkImageViewSlices()[handle]);
			}
		}

		//Shading rate
		if(desc.ShadingRate)
		{
			const auto rTarget = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanRenderTarget>(desc.ShadingRate);

			imageViews.emplace_back(rTarget->GetVkImageView());
		}

		return imageViews;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer::VulkanFrameBuffer(TRAP::Ref<VulkanDevice> device,
                                                          const VulkanRenderer::FrameBufferDesc& desc)
	: m_device(std::move(device))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanFrameBuffer(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFrameBufferPrefix, "Creating FrameBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if(!desc.RenderTargets.empty())
	{
		const Ref<RenderTarget> renderTarget = desc.RenderTargets[0];
		m_width = renderTarget->GetWidth();
		m_height = renderTarget->GetHeight();
		if (!desc.ColorArraySlices.empty())
			m_arraySize = 1;
		else
			m_arraySize = renderTarget->GetArraySize();
	}
	else if(desc.DepthStencil != nullptr)
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
		TRAP_ASSERT(false, "VulkanFrameBuffer(): No color or depth/stencil attachments");
	}

	if (!desc.RenderTargets.empty() && desc.RenderTargets[0]->GetDepth() > 1)
		m_arraySize = desc.RenderTargets[0]->GetDepth();

	const std::vector<VkImageView> imageViews = GetImageViews(desc);

	const VkFramebufferCreateInfo info = VulkanInits::FramebufferCreateInfo(desc.RenderPass->GetVkRenderPass(),
	                                                                        imageViews, m_width,
																			m_height, m_arraySize);
	VkCall(vkCreateFramebuffer(m_device->GetVkDevice(), &info, nullptr, &m_framebuffer));
	TRAP_ASSERT(m_framebuffer, "VulkanFrameBuffer(): Vulkan FrameBuffer is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanFrameBuffer::~VulkanFrameBuffer()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanFrameBufferPrefix, "Destroying FrameBuffer");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyFramebuffer(m_device->GetVkDevice(), m_framebuffer, nullptr);
}

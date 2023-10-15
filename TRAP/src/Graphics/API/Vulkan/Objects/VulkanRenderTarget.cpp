#include "TRAPPCH.h"
#include "VulkanRenderTarget.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

TRAP::Graphics::API::VulkanRenderTarget::VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc)
	: RenderTarget(desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(m_device, "VulkanRenderTarget(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Creating RenderTarget");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const bool isDepth = TRAP::Graphics::API::ImageFormatIsDepthOnly(m_format) ||
	                     TRAP::Graphics::API::ImageFormatIsDepthAndStencil(m_format);

	TRAP_ASSERT(!((isDepth) && (m_descriptors & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined),
	            "VulkanRenderTarget(): Cannot use depth stencil as UAV");

	const uint32_t depthOrArraySize = m_arraySize * m_depth;
	uint32_t numRTVs = m_mipLevels;
	if(((m_descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) != RendererAPI::DescriptorType::Undefined) ||
	   ((m_descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices) != RendererAPI::DescriptorType::Undefined))
	{
		numRTVs *= depthOrArraySize;
	}

	m_vkSliceDescriptors.resize(numRTVs);

	RendererAPI::TextureDesc textureDesc{};
	textureDesc.Flags = desc.Flags;
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.Depth = m_depth;
	textureDesc.ArraySize = m_arraySize;
	textureDesc.MipLevels = m_mipLevels;
	textureDesc.SampleCount = m_sampleCount;
	textureDesc.SampleQuality = m_sampleQuality;
	textureDesc.Format = m_format;
	textureDesc.ClearValue = m_clearValue;
	textureDesc.NativeHandle = desc.NativeHandle;

	if (!isDepth)
		textureDesc.StartState |= RendererAPI::ResourceState::RenderTarget;
	else
		textureDesc.StartState |= RendererAPI::ResourceState::DepthWrite;
	if((desc.StartState & RendererAPI::ResourceState::ShadingRateSource) != RendererAPI::ResourceState::Undefined)
		textureDesc.StartState |= RendererAPI::ResourceState::ShadingRateSource;

	//Set this by default to be able to sample the renderTarget in shader
	textureDesc.Descriptors = m_descriptors;
	//Create SRV by default for a render target unless this is on tile texture
	//where SRV is not supported
	if((desc.Flags & RendererAPI::TextureCreationFlags::OnTile) == RendererAPI::TextureCreationFlags::None)
		textureDesc.Descriptors |= RendererAPI::DescriptorType::Texture;
	else
	{
		if((textureDesc.Descriptors & RendererAPI::DescriptorType::Texture) != RendererAPI::DescriptorType::Undefined ||
		   (textureDesc.Descriptors & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
		{
			TP_WARN(Log::RendererVulkanRenderTargetPrefix, "On tile textures do not support DescriptorType::Texture or DescriptorType::RWTexture");
		}

		//On tile textures do not support SRV/UAV as there is no backing memory
		//You can only read these textures as input attachments inside same render pass
		textureDesc.Descriptors &= RendererAPI::DescriptorType::Texture | RendererAPI::DescriptorType::RWTexture;
	}

	if(isDepth)
	{
		//Make sure depth/stencil format is supported - fall back to VK_FORMAT_D16_UNORM if not
		const VkFormat vkDepthStencilFormat = ImageFormatToVkFormat(m_format);
		if(VK_FORMAT_UNDEFINED != vkDepthStencilFormat)
		{
			VkImageFormatProperties props{};
			const VkResult res = vkGetPhysicalDeviceImageFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(),
				                                                          vkDepthStencilFormat,
																	      VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
				                                                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
																	      0, &props);
			//Fall back to something that's guaranteed to work
			if(res != VK_SUCCESS)
			{
				textureDesc.Format = TRAP::Graphics::API::ImageFormat::D16_UNORM;
				TP_WARN(Log::RendererVulkanRenderTargetPrefix, "Depth stencil format (",
				        std::to_underlying(m_format), ") is not supported. Falling back to D16 format");
			}
		}
	}

	textureDesc.Name = desc.Name;

	m_texture = TRAP::MakeRef<VulkanTexture>();
	TRAP_ASSERT(m_texture, "VulkanRenderTarget(): Texture is nullptr!");
	m_texture->Init(textureDesc);

	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	if (m_height > 1)
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
	else
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;

	auto vkTexture = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(m_texture);
	VkImageViewCreateInfo rtvDesc = VulkanInits::ImageViewCreateInfo(vkTexture->GetVkImage(), viewType,
	                                                                 ImageFormatToVkFormat(m_format), 1,
																	 depthOrArraySize);

	VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr, &m_vkDescriptor));
	TRAP_ASSERT(m_vkDescriptor, "VulkanRenderTarget(): Vulkan Descriptor is nullptr!");

	for(uint32_t i = 0; i < m_mipLevels; ++i)
	{
		rtvDesc.subresourceRange.baseMipLevel = i;
		if (((m_descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) != RendererAPI::DescriptorType::Undefined) ||
			((m_descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices) != RendererAPI::DescriptorType::Undefined))
		{
			for (uint32_t j = 0; j < depthOrArraySize; ++j)
			{
				rtvDesc.subresourceRange.layerCount = 1;
				rtvDesc.subresourceRange.baseArrayLayer = j;
				VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr,
				                         &m_vkSliceDescriptors[i * depthOrArraySize + j]));
			}
		}
		else
			VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr, &m_vkSliceDescriptors[i]));
	}

	//Unlike DirectX 12, Vulkan textures start in undefined layout.
	//To keep in line with DirectX 12, we transition them to the specified layout
	//manually so app code doesn't have to worry about this
	//Render targets wont be created during runtime so this overhead will be minimal
	VulkanRenderer::UtilInitialTransition(m_texture, desc.StartState);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderTarget::~VulkanRenderTarget()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Destroying RenderTarget");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyImageView(m_device->GetVkDevice(), m_vkDescriptor, nullptr);

	const uint32_t depthOrArraySize = m_arraySize * m_depth;
	if (((m_descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) != RendererAPI::DescriptorType::Undefined) ||
		((m_descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices) != RendererAPI::DescriptorType::Undefined))
	{
		for(uint32_t i = 0; i < m_mipLevels; ++i)
		{
			for (uint32_t j = 0; j < depthOrArraySize; ++j)
			{
				if(m_vkSliceDescriptors[i * depthOrArraySize + j] != nullptr)
					vkDestroyImageView(m_device->GetVkDevice(), m_vkSliceDescriptors[i * depthOrArraySize + j],
					                   nullptr);
			}
		}
	}
	else
	{
		for (uint32_t i = 0; i < m_mipLevels; ++i)
		{
			if(m_vkSliceDescriptors[i] != nullptr)
				vkDestroyImageView(m_device->GetVkDevice(), m_vkSliceDescriptors[i], nullptr);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderTarget::SetRenderTargetName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	m_texture->SetTextureName(name);
}
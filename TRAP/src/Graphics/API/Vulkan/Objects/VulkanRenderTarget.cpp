#include "TRAPPCH.h"
#include "VulkanRenderTarget.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"
#include <memory>

TRAP::Graphics::API::VulkanRenderTarget::VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice()),
	  m_vkDescriptor(VK_NULL_HANDLE),
	  m_used(),
	  m_ID(s_RenderTargetIDs++)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	m_width = desc.Width;
	m_height = desc.Height;
	m_depth = desc.Depth;
	m_arraySize = desc.ArraySize;
	m_mipLevels = TRAP::Math::Max(1U, desc.MipLevels);
	m_sampleCount = desc.SampleCount;
	m_sampleQuality = desc.SampleQuality;
	m_format = desc.Format;
	m_clearValue = desc.ClearValue;
	m_descriptors = desc.Descriptors;

	TRAP_ASSERT(m_device, "VulkanRenderTarget(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Creating RenderTarget");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const bool isDepth = TRAP::Graphics::API::ImageFormatIsDepthOnly(desc.Format) ||
	                     TRAP::Graphics::API::ImageFormatIsDepthAndStencil(desc.Format);

	TRAP_ASSERT(!((isDepth) && (desc.Descriptors & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined),
	            "VulkanRenderTarget(): Cannot use depth stencil as UAV");

	const uint32_t depthOrArraySize = desc.ArraySize * desc.Depth;
	uint32_t numRTVs = m_mipLevels;
	if(((desc.Descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) != RendererAPI::DescriptorType::Undefined) ||
	   ((desc.Descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices) != RendererAPI::DescriptorType::Undefined))
	{
		numRTVs *= depthOrArraySize;
	}

	m_vkSliceDescriptors.resize(numRTVs);

	RendererAPI::TextureDesc textureDesc{};
	textureDesc.Flags = desc.Flags;
	textureDesc.Width = desc.Width;
	textureDesc.Height = desc.Height;
	textureDesc.Depth = desc.Depth;
	textureDesc.ArraySize = desc.ArraySize;
	textureDesc.MipLevels = m_mipLevels;
	textureDesc.SampleCount = desc.SampleCount;
	textureDesc.SampleQuality = desc.SampleQuality;
	textureDesc.Format = desc.Format;
	textureDesc.ClearValue = desc.ClearValue;
	textureDesc.NativeHandle = desc.NativeHandle;

	if (!isDepth)
		textureDesc.StartState |= RendererAPI::ResourceState::RenderTarget;
	else
		textureDesc.StartState |= RendererAPI::ResourceState::DepthWrite;
	if((desc.StartState & RendererAPI::ResourceState::ShadingRateSource) != RendererAPI::ResourceState::Undefined)
		textureDesc.StartState |= RendererAPI::ResourceState::ShadingRateSource;

	//Set this by default to be able to sample the renderTarget in shader
	textureDesc.Descriptors = desc.Descriptors;
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
		const VkFormat vkDepthStencilFormat = ImageFormatToVkFormat(desc.Format);
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
				        std::to_underlying(desc.Format), ") is not supported. Falling back to D16 format");
			}
		}
	}

	textureDesc.Name = desc.Name;

	m_texture = TRAP::MakeRef<VulkanTexture>();
	TRAP_ASSERT(m_texture, "VulkanRenderTarget(): Texture is nullptr!");
	m_texture->Init(textureDesc);

	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	if (desc.Height > 1)
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
	else
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;

	auto vkTexture = std::dynamic_pointer_cast<TRAP::Graphics::API::VulkanTexture>(m_texture);
	VkImageViewCreateInfo rtvDesc = VulkanInits::ImageViewCreateInfo(vkTexture->GetVkImage(), viewType,
	                                                                 ImageFormatToVkFormat(desc.Format), 1,
																	 depthOrArraySize);

	VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr, &m_vkDescriptor));
	TRAP_ASSERT(m_vkDescriptor, "VulkanRenderTarget(): Vulkan Descriptor is nullptr!");

	for(uint32_t i = 0; i < m_mipLevels; ++i)
	{
		rtvDesc.subresourceRange.baseMipLevel = i;
		if (((desc.Descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) != RendererAPI::DescriptorType::Undefined) ||
			((desc.Descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices) != RendererAPI::DescriptorType::Undefined))
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

	m_texture.reset();

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
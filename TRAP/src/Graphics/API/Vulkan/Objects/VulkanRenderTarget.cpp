#include "TRAPPCH.h"
#include "VulkanRenderTarget.h"

#include "VulkanTexture.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

std::atomic_int32_t TRAP::Graphics::API::VulkanRenderTarget::s_RenderTargetIDs = 1;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderTarget::VulkanRenderTarget(TRAP::Ref<VulkanDevice> device,
	const RendererAPI::RenderTargetDesc& desc,
	const TRAP::Ref<VulkanMemoryAllocator>& vma)
	: m_device(std::move(device)),
	  m_vkDescriptor(VK_NULL_HANDLE),
	  m_used(),
	  m_width(desc.Width),
	  m_height(desc.Height),
	  m_depth(desc.Depth),
	  m_arraySize(desc.ArraySize),
	  m_mipLevels(TRAP::Math::Max(1U, desc.MipLevels)),
	  m_sampleCount(desc.SampleCount),
	  m_sampleQuality(desc.SampleQuality),
	  m_format(desc.Format),
      m_clearValue(desc.ClearValue), 
      m_descriptors(desc.Descriptors),
	  m_ID(++s_RenderTargetIDs)
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Creating RenderTarget");
#endif

	const bool isDepth = RendererAPI::ImageFormatIsDepthOnly(desc.Format) || RendererAPI::ImageFormatIsDepthAndStencil(desc.Format);

	TRAP_ASSERT(!((isDepth) && static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWTexture)), "Cannot use depth stencil as UAV");

	m_mipLevels = TRAP::Math::Max(1U, desc.MipLevels);

	uint32_t depthOrArraySize = desc.ArraySize * desc.Depth;
	uint32_t numRTVs = m_mipLevels;
	if(static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) ||
	   static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices))
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
	textureDesc.MipLevels = desc.MipLevels;
	textureDesc.SampleCount = desc.SampleCount;
	textureDesc.SampleQuality = desc.SampleQuality;
	textureDesc.Format = desc.Format;
	textureDesc.ClearValue = desc.ClearValue;
	textureDesc.HostVisible = false;

	if (!isDepth)
		textureDesc.StartState |= RendererAPI::ResourceState::RenderTarget;
	else
		textureDesc.StartState |= RendererAPI::ResourceState::DepthWrite;

	//Set this by default to be able to sample the renderTarget in shader
	textureDesc.Descriptors = desc.Descriptors;
	//Create SRV by default for a render target
	textureDesc.Descriptors |= RendererAPI::DescriptorType::Texture;

	if(isDepth)
	{
		//Make sure depth/stencil format is supported - fall back to VK_FORMAT_D16_UNORM if not
		VkFormat vkDepthStencilFormat = ImageFormatToVkFormat(desc.Format);
		if(VK_FORMAT_UNDEFINED != vkDepthStencilFormat)
		{
			VkImageFormatProperties props{};
			VkResult res = vkGetPhysicalDeviceImageFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(),
				vkDepthStencilFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, 
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, &props);
			//Fall back to something that's guaranteed to work
			if(res != VK_SUCCESS)
			{
				textureDesc.Format = RendererAPI::ImageFormat::D16_UNORM;
				TP_WARN(Log::RendererVulkanRenderTargetPrefix, "Depth stencil format (", static_cast<uint32_t>(desc.Format), ") is not supported. Falling back to D16 format");
			}
		}
	}

	textureDesc.Name = desc.Name;

	m_texture = TRAP::MakeRef<VulkanTexture>(m_device, textureDesc, vma);

	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	if (desc.Height > 1)
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
	else
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;

	VkImageViewCreateInfo rtvDesc = VulkanInits::ImageViewCreateInfo(m_texture->GetVkImage(), viewType, ImageFormatToVkFormat(desc.Format), 1, depthOrArraySize);

	VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr, &m_vkDescriptor));

	for(uint32_t i = 0; i < m_mipLevels; ++i)
	{
		rtvDesc.subresourceRange.baseMipLevel = i;
		if (static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) ||
			static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices))
		{
			for (uint32_t j = 0; j < depthOrArraySize; ++j)
			{
				rtvDesc.subresourceRange.layerCount = 1;
				rtvDesc.subresourceRange.baseArrayLayer = j;
				VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr, &m_vkSliceDescriptors[i * depthOrArraySize + j]));
			}
		}
		else
			VkCall(vkCreateImageView(m_device->GetVkDevice(), &rtvDesc, nullptr, &m_vkSliceDescriptors[i]));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderTarget::~VulkanRenderTarget()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Destroying RenderTarget");
#endif

	if (m_texture)
		m_texture.reset();
	
	if(m_vkDescriptor)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkDescriptor, nullptr);

	const uint32_t depthOrArraySize = m_arraySize * m_depth;
	if(static_cast<uint32_t>(m_descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) ||
		static_cast<uint32_t>(m_descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices))
	{
		for(uint32_t i = 0; i < m_mipLevels; ++i)
		{
			for (uint32_t j = 0; j < depthOrArraySize; ++j)
			{
				if(m_vkSliceDescriptors[i * depthOrArraySize + j])
					vkDestroyImageView(m_device->GetVkDevice(), m_vkSliceDescriptors[i * depthOrArraySize + j], nullptr);
			}
		}
	}
	else
	{
		for (uint32_t i = 0; i < m_mipLevels; ++i)
		{
			if(m_vkSliceDescriptors[i])
				vkDestroyImageView(m_device->GetVkDevice(), m_vkSliceDescriptors[i], nullptr);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetDepth() const
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetArraySize() const
{
	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetMipLevels() const
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::API::VulkanRenderTarget::GetSampleCount() const
{
	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetSampleQuality() const
{
	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ImageFormat TRAP::Graphics::API::VulkanRenderTarget::GetImageFormat() const
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ClearValue TRAP::Graphics::API::VulkanRenderTarget::GetClearValue() const
{
	return m_clearValue;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::API::VulkanRenderTarget::GetDescriptorType() const
{
	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageView& TRAP::Graphics::API::VulkanRenderTarget::GetVkImageView()
{
	return m_vkDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkImageView>& TRAP::Graphics::API::VulkanRenderTarget::GetVkImageViewSlices() const
{
	return m_vkSliceDescriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderTarget::SetRenderTargetName(const char* name) const
{
	m_texture->SetTextureName(name);
}
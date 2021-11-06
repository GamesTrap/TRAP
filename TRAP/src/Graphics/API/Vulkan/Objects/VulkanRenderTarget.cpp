#include "TRAPPCH.h"
#include "VulkanRenderTarget.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/Textures/TextureBase.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

std::atomic_int32_t TRAP::Graphics::API::VulkanRenderTarget::s_RenderTargetIDs = 1;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderTarget::VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer().get())->GetDevice()),
	  m_vkDescriptor(VK_NULL_HANDLE),
	  m_used(),
	  m_ID(++s_RenderTargetIDs)
{
	m_width = desc.Width;
	m_height = desc.Height;
	m_depth = desc.Depth;
	m_arraySize = desc.ArraySize;
	m_mipLevels = TRAP::Math::Max(1U, desc.MipLevels);
	m_sampleCount = desc.SampleCount;
	m_sampleQuality = desc.SampleQuality;
	m_format = desc.Format;
	m_clearColor = desc.ClearColor;
	m_clearDepth = desc.ClearDepth;
	m_clearStencil = desc.ClearStencil;
	m_descriptors = desc.Descriptors;

	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Creating RenderTarget");
#endif

	const bool isDepth = TRAP::Graphics::API::ImageFormatIsDepthOnly(desc.Format) ||
	                     TRAP::Graphics::API::ImageFormatIsDepthAndStencil(desc.Format);

	TRAP_ASSERT(!((isDepth) && static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWTexture)),
	            "Cannot use depth stencil as UAV");

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
	textureDesc.MipLevels = m_mipLevels;
	textureDesc.SampleCount = desc.SampleCount;
	textureDesc.SampleQuality = desc.SampleQuality;
	textureDesc.Format = desc.Format;
	textureDesc.ClearColor = desc.ClearColor;
	textureDesc.ClearDepth = desc.ClearDepth;
	textureDesc.ClearStencil = desc.ClearStencil;
	textureDesc.NativeHandle = desc.NativeHandle;

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
				                                                    vkDepthStencilFormat,
																	VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
				                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
																	0, &props);
			//Fall back to something that's guaranteed to work
			if(res != VK_SUCCESS)
			{
				textureDesc.Format = TRAP::Graphics::API::ImageFormat::D16_UNORM;
				TP_WARN(Log::RendererVulkanRenderTargetPrefix, "Depth stencil format (",
				        static_cast<uint32_t>(desc.Format), ") is not supported. Falling back to D16 format");
			}
		}
	}

	textureDesc.Name = desc.Name;

	m_texture = TRAP::Graphics::TextureBase::Create(textureDesc);

	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	if (desc.Height > 1)
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
	else
		viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;

	auto* vkTexture = dynamic_cast<TRAP::Graphics::API::VulkanTexture*>(m_texture.get());
	VkImageViewCreateInfo rtvDesc = VulkanInits::ImageViewCreateInfo(vkTexture->GetVkImage(), viewType,
	                                                                 ImageFormatToVkFormat(desc.Format), 1,
																	 depthOrArraySize);

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
	TRAP_ASSERT(m_texture);
	TRAP_ASSERT(m_vkDescriptor);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Destroying RenderTarget");
#endif

	m_texture.reset();

	vkDestroyImageView(m_device->GetVkDevice(), m_vkDescriptor, nullptr);

	const uint32_t depthOrArraySize = m_arraySize * m_depth;
	if (static_cast<uint32_t>(m_descriptors & RendererAPI::DescriptorType::RenderTargetArraySlices) ||
		static_cast<uint32_t>(m_descriptors & RendererAPI::DescriptorType::RenderTargetDepthSlices))
	{
		for(uint32_t i = 0; i < m_mipLevels; ++i)
		{
			for (uint32_t j = 0; j < depthOrArraySize; ++j)
			{
				if(m_vkSliceDescriptors[i * depthOrArraySize + j])
					vkDestroyImageView(m_device->GetVkDevice(), m_vkSliceDescriptors[i * depthOrArraySize + j],
					                   nullptr);
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

TRAP::Graphics::API::ImageFormat TRAP::Graphics::API::VulkanRenderTarget::GetImageFormat() const
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Math::Vec4 TRAP::Graphics::API::VulkanRenderTarget::GetClearColor() const
{
	return m_clearColor;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::API::VulkanRenderTarget::GetClearDepth() const
{
	return m_clearDepth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetClearStencil() const
{
	return m_clearStencil;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::API::VulkanRenderTarget::GetDescriptorType() const
{
	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageView TRAP::Graphics::API::VulkanRenderTarget::GetVkImageView() const
{
	return m_vkDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkImageView>& TRAP::Graphics::API::VulkanRenderTarget::GetVkImageViewSlices() const
{
	return m_vkSliceDescriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanRenderTarget::GetID() const
{
	return m_ID;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanRenderTarget::SetRenderTargetName(const char* name) const
{
	m_texture->SetTextureName(name);
}
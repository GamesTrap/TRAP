#include "TRAPPCH.h"
#include "VulkanTexture.h"

#include "VulkanMemoryAllocator.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanTexture::VulkanTexture(TRAP::Ref<VulkanDevice> device,
                                                  const RendererAPI::TextureDesc& desc,
                                                  TRAP::Ref<VulkanMemoryAllocator> vma)
	: m_device(std::move(device)),
	  m_vma(std::move(vma)),
	  m_vkSRVDescriptor(VK_NULL_HANDLE),
	  m_vkSRVStencilDescriptor(VK_NULL_HANDLE),
	  m_vkImage(VK_NULL_HANDLE),
	  m_vkAllocation(),
	  m_width(desc.Width),
	  m_height(desc.Height),
	  m_depth(desc.Depth),
	  m_mipLevels(desc.MipLevels),
	  m_arraySizeMinusOne(desc.ArraySize - 1),
	  m_format(desc.Format),
	  m_aspectMask(),
	  m_UAV(desc.Descriptors & RendererAPI::DescriptorType::RWTexture),
	  m_ownsImage(true)
{
	TRAP_ASSERT(m_device, "device is nullptr");
	TRAP_ASSERT(m_vma, "vma is nullptr");
	TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize));

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Creating Texture");
#endif
	
	if(desc.SampleCount > RendererAPI::SampleCount::SampleCount1 && desc.MipLevels > 1)
	{
		TP_ERROR(Log::RendererVulkanTexturePrefix, "Multi-Sampled textures cannot have mip maps");
		TRAP_ASSERT(false);
		return;
	}

	if (static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWTexture))
		m_vkUAVDescriptors.resize((static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWTexture) ? desc.MipLevels : 0));

	VkImageUsageFlags additionalFlags = 0;
	if (static_cast<uint32_t>(desc.StartState & RendererAPI::ResourceState::RenderTarget))
		additionalFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	else if (static_cast<uint32_t>(desc.StartState & RendererAPI::ResourceState::DepthWrite))
		additionalFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VkImageType imageType = VK_IMAGE_TYPE_MAX_ENUM;
	if(static_cast<uint32_t>(desc.Flags & RendererAPI::TextureCreationFlags::Force2D))
	{
		TRAP_ASSERT(desc.Depth == 1);
		imageType = VK_IMAGE_TYPE_2D;
	}
	else if(static_cast<uint32_t>(desc.Flags & RendererAPI::TextureCreationFlags::Force3D))
		imageType = VK_IMAGE_TYPE_3D;
	else
	{
		if (desc.Depth > 1)
			imageType = VK_IMAGE_TYPE_3D;
		else if (desc.Height > 1)
			imageType = VK_IMAGE_TYPE_2D;
		else
			imageType = VK_IMAGE_TYPE_1D;
	}

	RendererAPI::DescriptorType descriptors = desc.Descriptors;
	bool cubeMapRequired = (descriptors & RendererAPI::DescriptorType::TextureCube) == RendererAPI::DescriptorType::TextureCube;
	bool arrayRequired = false;

	if (imageType == VK_IMAGE_TYPE_3D)
		arrayRequired = true;

	if(m_vkImage == VK_NULL_HANDLE)
	{
		VkImageCreateInfo info = VulkanInits::ImageCreateInfo(imageType,
		                                                      ImageFormatToVkFormat(desc.Format),
		                                                      desc.Width,
		                                                      desc.Height,
		                                                      desc.Depth,
		                                                      desc.MipLevels,
		                                                      desc.ArraySize,
		                                                      SampleCountToVkSampleCount(desc.SampleCount),
		                                                      (desc.HostVisible != 0)
			                                                      ? VK_IMAGE_TILING_LINEAR
			                                                      : VK_IMAGE_TILING_OPTIMAL,
															  DescriptorTypeToVkImageUsage(descriptors));
		info.usage |= additionalFlags;

		if (cubeMapRequired)
			info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		if (arrayRequired)
			info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;

		if((info.usage & VK_IMAGE_USAGE_SAMPLED_BIT) || (info.usage & VK_IMAGE_USAGE_STORAGE_BIT))
		{
			//Make it easy to copy to and from textures
			info.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		}

		TRAP_ASSERT(VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[static_cast<uint32_t>(desc.Format)], "GPU shader can't read from this format");

		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device->GetPhysicalDevice()->GetVkPhysicalDevice(), info.format, &props);
		const VkFormatFeatureFlags formatFeatures = VkImageUsageToFormatFeatures(info.usage);

		if(desc.HostVisible)
		{
			const VkFormatFeatureFlags flags = props.linearTilingFeatures & formatFeatures;
			TRAP_ASSERT(0 != flags, "Format is not supported for host visibile images");
		}
		else
		{
			const VkFormatFeatureFlags flags = props.optimalTilingFeatures & formatFeatures;
			TRAP_ASSERT(0 != flags, "Format is not suported for GPU local images (i.e. not host visible images)");
		}

		VmaAllocationCreateInfo memReqs{};
		if (static_cast<uint32_t>(desc.Flags & RendererAPI::TextureCreationFlags::OwnMemory))
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		memReqs.usage = static_cast<VmaMemoryUsage>(VMA_MEMORY_USAGE_GPU_ONLY);

		VmaAllocationInfo allocInfo{};
		VkCall(vmaCreateImage(m_vma->GetVMAAllocator(), &info, &memReqs, &m_vkImage, &m_vkAllocation, &allocInfo));
	}

	//Create image view
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	switch(imageType)
	{
	case VK_IMAGE_TYPE_1D:
		viewType = desc.ArraySize > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
		break;

	case VK_IMAGE_TYPE_2D:
		if (cubeMapRequired)
			viewType = (desc.ArraySize > 6) ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
		else
			viewType = desc.ArraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
		break;

	case VK_IMAGE_TYPE_3D:
		if(desc.ArraySize > 1)
		{
			TP_ERROR(Log::RendererVulkanTexturePrefix, "Cannot support 3D Texture Array in Vulkan");
			TRAP_ASSERT(false);
		}
		viewType = VK_IMAGE_VIEW_TYPE_3D;
		break;

	default:
		TRAP_ASSERT(false, "Image Format not supported!");
		break;
	}

	TRAP_ASSERT(viewType != VK_IMAGE_VIEW_TYPE_MAX_ENUM, "Invalid Image View");

	//SRV
	VkImageViewCreateInfo srvDesc = VulkanInits::ImageViewCreateInfo(m_vkImage, viewType, ImageFormatToVkFormat(desc.Format), desc.MipLevels, desc.ArraySize);
	if (static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::Texture))
		VkCall(vkCreateImageView(device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVDescriptor));

	//SRV stencil
	if((RendererAPI::ImageFormatHasStencil(desc.Format)) && (static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::Texture)))
	{
		srvDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		VkCall(vkCreateImageView(device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVStencilDescriptor));
	}

	//UAV
	if(static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::RWTexture))
	{
		VkImageViewCreateInfo uavDesc = srvDesc;
		//Note: We dont support imageCube, imageCubeArray for consistency with other APIs
		//All cubemaps will be used as image2DArray for Image Load / Store ops
		if (uavDesc.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY || uavDesc.viewType == VK_IMAGE_VIEW_TYPE_CUBE)
			uavDesc.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		uavDesc.subresourceRange.levelCount = 1;
		for(uint32_t i = 0; i < desc.MipLevels; ++i)
		{
			uavDesc.subresourceRange.baseMipLevel = i;
			VkCall(vkCreateImageView(device->GetVkDevice(), &uavDesc, nullptr, &m_vkUAVDescriptors[i]));
		}
	}

#if defined(ENABLE_GRAPHICS_DEBUG)
	if (desc.Name)
		SetTextureName(desc.Name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::~VulkanTexture()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Destroying Texture");
#endif

	if(m_ownsImage)
	{
		if(m_vkImage)
			vmaDestroyImage(m_vma->GetVMAAllocator(), m_vkImage, m_vkAllocation);
	}

	if (m_vkSRVDescriptor)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkSRVDescriptor, nullptr);

	if (m_vkSRVStencilDescriptor)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkSRVStencilDescriptor, nullptr);

	if(!m_vkUAVDescriptors.empty())
	{
		for(uint32_t i = 0; i < m_mipLevels; ++i)
			vkDestroyImageView(m_device->GetVkDevice(), m_vkUAVDescriptors[i], nullptr);
	}

	/*if (m_SVT)
		RemoveVirtualTexture(, m_SVT);*/ //TODO
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::SetTextureName(const char* name) const
{
	TRAP_ASSERT(name);

	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(m_vkImage), VK_OBJECT_TYPE_IMAGE, name);
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageView& TRAP::Graphics::API::VulkanTexture::GetSRVVkImageView()
{
	return m_vkSRVDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageView& TRAP::Graphics::API::VulkanTexture::GetSRVStencilVkImageView()
{
	return m_vkSRVStencilDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkImageView>& TRAP::Graphics::API::VulkanTexture::GetUAVVkImageViews() const
{
	return m_vkUAVDescriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImage& TRAP::Graphics::API::VulkanTexture::GetVkImage()
{
	return m_vkImage;
}

//-------------------------------------------------------------------------------------------------------------------//

VmaAllocation& TRAP::Graphics::API::VulkanTexture::GetVMAAllocation()
{
	return m_vkAllocation;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetWidth() const
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetHeight() const
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetDepth() const
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetMipLevels() const
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetArraySizeMinusOne() const
{
	return m_arraySizeMinusOne;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::ImageFormat TRAP::Graphics::API::VulkanTexture::GetImageFormat() const
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetAspectMask() const
{
	return m_aspectMask;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::API::VulkanTexture::GetUAV() const
{
	return m_UAV;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanTexture::OwnsImage() const
{
	return m_ownsImage;
}
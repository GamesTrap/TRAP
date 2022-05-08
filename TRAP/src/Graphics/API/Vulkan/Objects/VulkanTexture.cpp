#include "TRAPPCH.h"
#include "VulkanTexture.h"

#include "VulkanQueue.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
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
	  m_vkDeviceMemory()
{
	TRAP_ASSERT(m_device, "Device is nullptr");
	TRAP_ASSERT(m_vma, "VMA is nullptr");
	TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize), "Invalid resolution");
	TRAP_ASSERT(!(desc.SampleCount > RendererAPI::SampleCount::SampleCount1 && desc.MipLevels > 1), "Multi-Sampled texture cannot have mip maps");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Creating Texture");
#endif

	if (static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWTexture))
		m_vkUAVDescriptors.resize((static_cast<uint32_t>(desc.Descriptors & RendererAPI::DescriptorType::RWTexture) ?
		                           desc.MipLevels : 0));

	if (desc.NativeHandle && !static_cast<bool>((desc.Flags & RendererAPI::TextureCreationFlags::Import)))
	{
		m_ownsImage = false;
		m_vkImage = static_cast<VkImage>(desc.NativeHandle);
	}
	else
		m_ownsImage = true;

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
	bool cubeMapRequired = (descriptors & RendererAPI::DescriptorType::TextureCube) ==
	                       RendererAPI::DescriptorType::TextureCube;
	bool arrayRequired = false;

	const bool isPlanarFormat = TRAP::Graphics::API::ImageFormatIsPlanar(desc.Format);
	const uint32_t numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(desc.Format);
	const bool isSinglePlane = TRAP::Graphics::API::ImageFormatIsSinglePlane(desc.Format);
	TRAP_ASSERT(((isSinglePlane && numOfPlanes == 1) || (!isSinglePlane && numOfPlanes > 1 && numOfPlanes <= 3)),
	            "Number of planes for multi-planar formats must be 2 or 3 and for single-planar "
				"formats it must be 1");

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
		                                                      VK_IMAGE_TILING_OPTIMAL,
															  DescriptorTypeToVkImageUsage(descriptors));
		info.usage |= additionalFlags;

		if (cubeMapRequired)
			info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		if (arrayRequired)
			info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;

		VkFormatProperties formatProps{};
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), info.format,
		                                    &formatProps);

		//Multi-Planar formats must have each plane separately bound to memory, rather than having a single memory binding for the whole image
		if(isPlanarFormat)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DISJOINT_BIT);
			info.flags |= VK_IMAGE_CREATE_DISJOINT_BIT;
		}

		if((info.usage & VK_IMAGE_USAGE_SAMPLED_BIT) || (info.usage & VK_IMAGE_USAGE_STORAGE_BIT))
		{
			//Make it easy to copy to and from textures
			info.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		}

		TRAP_ASSERT(VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[static_cast<uint32_t>(desc.Format)],
		            "GPU shader can't read from this format");

		const VkFormatFeatureFlags formatFeatures = VkImageUsageToFormatFeatures(info.usage);

		const VkFormatFeatureFlags flags = formatProps.optimalTilingFeatures & formatFeatures;
		TRAP_ASSERT(0 != flags, "Format is not suported for GPU local images (i.e. not host visible images)");

		VmaAllocationCreateInfo memReqs{};
		if (static_cast<uint32_t>(desc.Flags & RendererAPI::TextureCreationFlags::OwnMemory))
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		memReqs.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkExternalMemoryImageCreateInfo externalInfo{};
		externalInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
		externalInfo.pNext = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
		VkImportMemoryWin32HandleInfoKHR importInfo{};
		importInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
		importInfo.pNext = nullptr;
#endif
		VkExportMemoryAllocateInfo exportMemoryInfo{};
		exportMemoryInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
		exportMemoryInfo.pNext = nullptr;

		if(VulkanRenderer::s_externalMemory && static_cast<bool>(desc.Flags & RendererAPI::TextureCreationFlags::Import))
		{
			info.pNext = &externalInfo;

#ifdef TRAP_PLATFORM_WINDOWS
			struct ImportHandleInfo
			{
				void* Handle;
				VkExternalMemoryHandleTypeFlagBits HandleType;
			};

			ImportHandleInfo* handleInfo = reinterpret_cast<ImportHandleInfo*>(desc.NativeHandle);
			importInfo.Handle = handleInfo->Handle;
			importInfo.HandleType = handleInfo->HandleType;

			externalInfo.handleTypes = handleInfo->HandleType;

			memReqs.pUserData = &importInfo;
			//Allocate external (importable / exportable) memory as dedicated memory to avoid
			//unnecessary complexity to the Vulkan Memory Allocator
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
#endif
		}
		else if(VulkanRenderer::s_externalMemory && static_cast<bool>(desc.Flags & RendererAPI::TextureCreationFlags::Export))
		{
#ifdef TRAP_PLATFORM_WINDOWS
			exportMemoryInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#endif
			memReqs.pUserData = &exportMemoryInfo;
			//Allocate external (importable / exportable) memory as dedicated memory to avoid
			//unnecessary complexity to the Vulkan Memory Allocator
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}

		VmaAllocationInfo allocInfo{};
		if(isSinglePlane)
		{
			VkCall(vmaCreateImage(m_vma->GetVMAAllocator(), &info, &memReqs, &m_vkImage, &m_vkAllocation,
			                      &allocInfo));
		}
		else //Multi-Planar formats
		{
			//Create info requires the mutable format flag set for multi planar images
			//Also pass the format list for mutable formats as per recommendation from the spec
			//Might help to keep DCC enabled if we ever use this as a output format
			//DCC gets disabled when we pass mutable format bit to the create info.
			//Passing the format list helps the driver to enable it
			VkFormat planarFormat = ImageFormatToVkFormat(desc.Format);
			VkImageFormatListCreateInfo formatList{};
			formatList.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
			formatList.pNext = nullptr;
			formatList.pViewFormats = &planarFormat;
			formatList.viewFormatCount = 1;

			info.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
			info.pNext = &formatList;

			//Create Image
			VkCall(vkCreateImage(m_device->GetVkDevice(), &info, nullptr, &m_vkImage));

			VkMemoryRequirements memReq{};
			std::vector<uint64_t> planeOffsets(3);
			UtilGetPlanarVkImageMemoryRequirement(m_device->GetVkDevice(), m_vkImage, numOfPlanes, memReq,
			                                      planeOffsets);

			//Allocate image memory
			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocInfo.allocationSize = memReq.size;
			const VkPhysicalDeviceMemoryProperties& memProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceMemoryProperties();
			memAllocInfo.memoryTypeIndex = GetMemoryType(memReq.memoryTypeBits, memProps,
			                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VkCall(vkAllocateMemory(m_device->GetVkDevice(), &memAllocInfo, nullptr, &m_vkDeviceMemory));

			//Bind planes to their memories
			std::vector<VkBindImageMemoryInfo> bindImagesMemoryInfo(3);
			std::vector<VkBindImagePlaneMemoryInfo> bindImagePlanesMemoryInfo(3);
			for(uint32_t i = 0; i < numOfPlanes; ++i)
			{
				VkBindImagePlaneMemoryInfo& bindImagePlaneMemoryInfo = bindImagePlanesMemoryInfo[i];
				bindImagePlaneMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
				bindImagePlaneMemoryInfo.pNext = nullptr;
				bindImagePlaneMemoryInfo.planeAspect = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_PLANE_0_BIT << i);

				VkBindImageMemoryInfo& bindImageMemoryInfo = bindImagesMemoryInfo[i];
				bindImageMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
				bindImageMemoryInfo.pNext = &bindImagePlaneMemoryInfo;
				bindImageMemoryInfo.image = m_vkImage;
				bindImageMemoryInfo.memory = m_vkDeviceMemory;
				bindImageMemoryInfo.memoryOffset = planeOffsets[i];
			}

			VkCall(vkBindImageMemory2(m_device->GetVkDevice(), numOfPlanes, bindImagesMemoryInfo.data()));
		}
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
	VkImageViewCreateInfo srvDesc = VulkanInits::ImageViewCreateInfo(m_vkImage, viewType,
	                                                                 ImageFormatToVkFormat(desc.Format),
																	 desc.MipLevels, desc.ArraySize);
	m_aspectMask = DetermineAspectMask(srvDesc.format, true);

	if (desc.VkSamplerYcbcrConversionInfo)
		srvDesc.pNext = desc.VkSamplerYcbcrConversionInfo;

	if (static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::Texture))
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVDescriptor));

	//SRV stencil
	if((TRAP::Graphics::API::ImageFormatHasStencil(desc.Format)) &&
	   (static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::Texture)))
	{
		srvDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVStencilDescriptor));
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
			VkCall(vkCreateImageView(m_device->GetVkDevice(), &uavDesc, nullptr, &m_vkUAVDescriptors[i]));
		}
	}

	m_width = desc.Width;
	m_height = desc.Height;
	m_depth = desc.Depth;
	m_mipLevels = desc.MipLevels;
	m_arraySize = desc.ArraySize;
	m_format = desc.Format;
	m_UAV = desc.Descriptors & RendererAPI::DescriptorType::RWTexture;

#if defined(ENABLE_GRAPHICS_DEBUG)
	if (!desc.Name.empty())
		VulkanTexture::SetTextureName(desc.Name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::~VulkanTexture()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Destroying Texture");
#endif

	if(m_ownsImage && m_vkImage)
	{
		if(TRAP::Graphics::API::ImageFormatIsSinglePlane(m_format))
		{
			vmaDestroyImage(m_vma->GetVMAAllocator(), m_vkImage, m_vkAllocation);
		}
		else
		{
			vkDestroyImage(m_device->GetVkDevice(), m_vkImage, nullptr);
			vkFreeMemory(m_device->GetVkDevice(), m_vkDeviceMemory, nullptr);
		}
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
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::SetTextureName(const std::string_view name) const
{
	TRAP_ASSERT(!name.empty());

	if(!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(m_vkImage), VK_OBJECT_TYPE_IMAGE, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), reinterpret_cast<uint64_t>(m_vkImage), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageView TRAP::Graphics::API::VulkanTexture::GetSRVVkImageView() const
{
	return m_vkSRVDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImageView TRAP::Graphics::API::VulkanTexture::GetSRVStencilVkImageView() const
{
	return m_vkSRVStencilDescriptor;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<VkImageView>& TRAP::Graphics::API::VulkanTexture::GetUAVVkImageViews() const
{
	return m_vkUAVDescriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

VkImage TRAP::Graphics::API::VulkanTexture::GetVkImage()
{
	return m_vkImage;
}

//-------------------------------------------------------------------------------------------------------------------//

VmaAllocation TRAP::Graphics::API::VulkanTexture::GetVMAAllocation() const
{
	return m_vkAllocation;
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetMemoryType(uint32_t typeBits,
                                                           const VkPhysicalDeviceMemoryProperties& memProps,
                                                           const VkMemoryPropertyFlags props, VkBool32* memTypeFound)
{
	for(uint32_t i = 0; i < memProps.memoryTypeCount; i++)
	{
		if((typeBits & 1) == 1)
		{
			if((memProps.memoryTypes[i].propertyFlags & props) == props)
			{
				if (memTypeFound)
					*memTypeFound = true;

				return i;
			}
		}
		typeBits >>= 1;
	}

	if(memTypeFound)
	{
		*memTypeFound = false;
		return 0;
	}

	TP_ERROR(Log::RendererVulkanTexturePrefix, "Could not find a matching memory type");
	TRAP_ASSERT(false);
	return 0;
}
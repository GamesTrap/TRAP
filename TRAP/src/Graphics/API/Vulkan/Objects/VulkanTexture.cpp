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

	if (desc.NativeHandle)
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
	bool cubeMapRequired = (descriptors & RendererAPI::DescriptorType::TextureCube) == RendererAPI::DescriptorType::TextureCube;
	bool arrayRequired = false;

	const bool isPlanarFormat = RendererAPI::ImageFormatIsPlanar(desc.Format);
	const uint32_t numOfPlanes = RendererAPI::ImageFormatNumOfPlanes(desc.Format);
	const bool isSinglePlane = RendererAPI::ImageFormatIsSinglePlane(desc.Format);
	TRAP_ASSERT(((isSinglePlane && numOfPlanes == 1) || (!isSinglePlane && numOfPlanes > 1 && numOfPlanes <= 3)), "Number of planes for multi-planar formats must be 2 or 3 and for single-planar formats it must be 1");
	
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
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), info.format, &formatProps);
		if(isPlanarFormat) //Multi-Planar formats must have each plane separately bound to memory, rather than having a single memory binding for the whole image
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DISJOINT_BIT);
			info.flags |= VK_IMAGE_CREATE_DISJOINT_BIT;
		}
		
		if((info.usage & VK_IMAGE_USAGE_SAMPLED_BIT) || (info.usage & VK_IMAGE_USAGE_STORAGE_BIT))
		{
			//Make it easy to copy to and from textures
			info.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		}

		TRAP_ASSERT(VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[static_cast<uint32_t>(desc.Format)], "GPU shader can't read from this format");
		
		const VkFormatFeatureFlags formatFeatures = VkImageUsageToFormatFeatures(info.usage);

		const VkFormatFeatureFlags flags = formatProps.optimalTilingFeatures & formatFeatures;
		TRAP_ASSERT(0 != flags, "Format is not suported for GPU local images (i.e. not host visible images)");

		VmaAllocationCreateInfo memReqs{};
		if (static_cast<uint32_t>(desc.Flags & RendererAPI::TextureCreationFlags::OwnMemory))
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		memReqs.usage = static_cast<VmaMemoryUsage>(VMA_MEMORY_USAGE_GPU_ONLY);

		VmaAllocationInfo allocInfo{};
		if(isSinglePlane)
		{
			VkCall(vmaCreateImage(m_vma->GetVMAAllocator(), &info, &memReqs, &m_vkImage, &m_vkAllocation, &allocInfo));
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
			UtilGetPlanarVkImageMemoryRequirement(m_device->GetVkDevice(), m_vkImage, numOfPlanes, memReq, planeOffsets);

			//Allocate image memory
			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocInfo.allocationSize = memReq.size;
			const VkPhysicalDeviceMemoryProperties& memProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceMemoryProperties();
			memAllocInfo.memoryTypeIndex = GetMemoryType(memReq.memoryTypeBits, memProps, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
	VkImageViewCreateInfo srvDesc = VulkanInits::ImageViewCreateInfo(m_vkImage, viewType, ImageFormatToVkFormat(desc.Format), desc.MipLevels, desc.ArraySize);
	m_aspectMask = DetermineAspectMask(srvDesc.format, true);

	if (desc.VkSamplerYcbcrConversionInfo)
		srvDesc.pNext = desc.VkSamplerYcbcrConversionInfo;
	
	if (static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::Texture))
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVDescriptor));

	//SRV stencil
	if((RendererAPI::ImageFormatHasStencil(desc.Format)) && (static_cast<uint32_t>(descriptors & RendererAPI::DescriptorType::Texture)))
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

#if defined(ENABLE_GRAPHICS_DEBUG)
	if (desc.Name)
		SetTextureName(desc.Name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(TRAP::Ref<VulkanDevice> device,
	const RendererAPI::TextureDesc& desc,
	const std::vector<uint8_t>& imageData,
	const TRAP::Ref<VulkanCommandBuffer>& cmd,
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
	      m_UAV(desc.Descriptors& RendererAPI::DescriptorType::RWTexture),
	      m_ownsImage(true)
{
	TRAP_ASSERT(m_device);
	TRAP_ASSERT(cmd);
	
	m_SVT = TRAP::MakeRef<RendererAPI::VirtualTexture>();

	uint32_t imageSize = 0;
	uint32_t mipSize = desc.Width * desc.Height * desc.Depth;
	while(mipSize > 0)
	{
		imageSize += mipSize;
		mipSize /= 4;
	}

	m_SVT->VirtualImageData = imageData;

	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	m_ownsImage = true;

	VkImageCreateInfo addInfo = VulkanInits::ImageCreateInfo(VK_IMAGE_TYPE_2D,
	                                                         format,
	                                                         desc.Width,
	                                                         desc.Height,
	                                                         desc.Depth,
	                                                         desc.MipLevels,
	                                                         1,
	                                                         VK_SAMPLE_COUNT_1_BIT,
	                                                         VK_IMAGE_TILING_OPTIMAL,
	                                                         VK_IMAGE_USAGE_SAMPLED_BIT |
	                                                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
	                                                         VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	VkCall(vkCreateImage(m_device->GetVkDevice(), &addInfo, nullptr, &m_vkImage));

	//Get memory requirements
	VkMemoryRequirements sparseImageMemoryReqs;
	//Sparse image memory requirement counts
	vkGetImageMemoryRequirements(m_device->GetVkDevice(), m_vkImage, &sparseImageMemoryReqs);

	//Check requested image size against hardware sparse limit
	if(sparseImageMemoryReqs.size > m_device->GetPhysicalDevice()->GetVkPhysicalDeviceProperties().limits.sparseAddressSpaceSize)
	{
		TP_ERROR(Log::RendererVulkanVirtualTexturePrefix, "Requested sparse image size exceeds supported sparse address space size!");
		return;
	}

	//Get sparse memory requirements
	//Count
	uint32_t sparseMemoryReqsCount = 32;
	std::vector<VkSparseImageMemoryRequirements> sparseMemoryReqs(sparseMemoryReqsCount);
	vkGetImageSparseMemoryRequirements(m_device->GetVkDevice(), m_vkImage, &sparseMemoryReqsCount, sparseMemoryReqs.data());

	if(sparseMemoryReqsCount == 0)
	{
		TP_ERROR(Log::RendererVulkanVirtualTexturePrefix, "No memory requirements for the sparse image!");
		return;
	}
	sparseMemoryReqs.resize(sparseMemoryReqsCount);

	//Get actual requirements
	vkGetImageSparseMemoryRequirements(m_device->GetVkDevice(), m_vkImage, &sparseMemoryReqsCount, sparseMemoryReqs.data());

	m_SVT->SparseVirtualTexturePageWidth = sparseMemoryReqs[0].formatProperties.imageGranularity.width;
	m_SVT->SparseVirtualTexturePageHeight = sparseMemoryReqs[0].formatProperties.imageGranularity.height;
	m_SVT->VirtualPageTotalCount = imageSize / (static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageWidth) * static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageHeight));

	uint32_t tiledMipLevel = desc.MipLevels - static_cast<uint32_t>(TRAP::Math::Log2(TRAP::Math::Min(static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageWidth),
		static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageHeight))));

	TP_INFO(Log::RendererVulkanVirtualTexturePrefix, "Sparse image memory requirements: ", sparseMemoryReqsCount);

	for(uint32_t i = 0; i < sparseMemoryReqs.size(); ++i)
	{
		const VkSparseImageMemoryRequirements& reqs = sparseMemoryReqs[i];
		m_SVT->MipTailStart = reqs.imageMipTailFirstLod;
	}

	m_SVT->LastFilledMip = m_SVT->MipTailStart - 1;

	//Get sparse image requirements for the color aspect
	VkSparseImageMemoryRequirements sparseMemoryReq{};
	bool colorAspectFound = false;
	for (auto& reqs : sparseMemoryReqs)
	{
		if (reqs.formatProperties.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
		{
			sparseMemoryReq = reqs;
			colorAspectFound = true;
			break;
		}
	}
	if(!colorAspectFound)
	{
		TP_ERROR(Log::RendererVulkanVirtualTexturePrefix, "Could not find sparse image memory requirements for color aspect bit!");
		return;
	}

	VkPhysicalDeviceMemoryProperties memProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceMemoryProperties();

	TRAP_ASSERT((sparseImageMemoryReqs.size % sparseImageMemoryReqs.alignment) == 0);
	m_SVT->SparseMemoryTypeIndex = GetMemoryType(sparseImageMemoryReqs.memoryTypeBits, memProps, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//Check if the format has a single mip tail for all layers or one mip tail for each layer
	//The mip tail contains all mip levels > sparseMemoryReq.imageMipTailFirstLod
	bool singleMipTail = sparseMemoryReq.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT;

	m_SVT->Pages = TRAP::MakeRef<std::vector<RendererAPI::VirtualTexturePage>>(1);
	m_SVT->SparseImageMemoryBinds = TRAP::MakeRef<std::vector<VkSparseImageMemoryBind>>(1);
	m_SVT->OpaqueMemoryBinds = TRAP::MakeRef<std::vector<VkSparseMemoryBind>>(1);

	std::vector<VkSparseMemoryBind>& opaqueMemoryBinds = *static_cast<std::vector<VkSparseMemoryBind>*>(m_SVT->OpaqueMemoryBinds.get());

	//Sparse bindings for each mip level of all layers outside of the mip tail
	for(uint32_t layer = 0; layer < 1; layer++)
	{
		//sparseMemoryReq.imageMipTailFirstLod is the first mip level that's stored inside the mip tail
		for(uint32_t mipLevel = 0; mipLevel < tiledMipLevel; mipLevel++)
		{
			VkExtent3D extent;
			extent.width = TRAP::Math::Max(addInfo.extent.width >> mipLevel, 1u);
			extent.height = TRAP::Math::Max(addInfo.extent.height >> mipLevel, 1u);
			extent.depth = TRAP::Math::Max(addInfo.extent.depth >> mipLevel, 1u);

			VkImageSubresource subResource{};
			subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subResource.mipLevel = mipLevel;
			subResource.arrayLayer = layer;

			//Aligned sizes by image granularity
			VkExtent3D imageGranularity = sparseMemoryReq.formatProperties.imageGranularity;
			VkExtent3D sparseBindCounts{};
			VkExtent3D lastBlockExtent{};
			sparseBindCounts = AlignedDivision(extent, imageGranularity);
			lastBlockExtent.width = ((extent.width % imageGranularity.width) ? extent.width % imageGranularity.width : imageGranularity.width);
			lastBlockExtent.height = ((extent.height % imageGranularity.height) ? extent.height % imageGranularity.height : imageGranularity.height);
			lastBlockExtent.depth = ((extent.depth % imageGranularity.depth) ? extent.depth % imageGranularity.depth : imageGranularity.depth);

			//Allocate memory for some blocks
			uint32_t index = 0;
			for (uint32_t z = 0; z < sparseBindCounts.depth; z++)
			{
				for(uint32_t y = 0; y < sparseBindCounts.height; y++)
				{
					for(uint32_t x = 0; x < sparseBindCounts.width; x++)
					{
						//Offset
						VkOffset3D offset;
						offset.x = x * imageGranularity.width;
						offset.y = y * imageGranularity.height;
						offset.z = z * imageGranularity.depth;
						//Size of the page
						VkExtent3D extent;
						extent.width = (x == sparseBindCounts.width - 1) ? lastBlockExtent.width : imageGranularity.width;
						extent.height = (y == sparseBindCounts.height - 1) ? lastBlockExtent.height : imageGranularity.height;
						extent.depth = (z == sparseBindCounts.depth - 1) ? lastBlockExtent.depth : imageGranularity.depth;

						//Add new virtual page
						RendererAPI::VirtualTexturePage* newPage = AddPage(offset, extent, m_SVT->SparseVirtualTexturePageWidth * m_SVT->SparseVirtualTexturePageHeight * sizeof(uint32_t), mipLevel, layer);
						newPage->ImageMemoryBind.subresource = subResource;
						
						index++;
					}
				}
			}
		}

		//Check if format has one mip tail per layer
		if((!singleMipTail) && (sparseMemoryReq.imageMipTailFirstLod < desc.MipLevels))
		{
			//Allocate memory for the mip tail
			VkMemoryAllocateInfo allocInfo = VulkanInits::MemoryAllocateInfo(sparseMemoryReq.imageMipTailSize, m_SVT->SparseMemoryTypeIndex);

			VkDeviceMemory deviceMemory;
			VkCall(vkAllocateMemory(m_device->GetVkDevice(), &allocInfo, nullptr, &deviceMemory));

			//(Opaque) sparse memory binding
			VkSparseMemoryBind sparseMemoryBind{};
			sparseMemoryBind.resourceOffset = sparseMemoryReq.imageMipTailOffset + layer * sparseMemoryReq.imageMipTailStride;
			sparseMemoryBind.size = sparseMemoryReq.imageMipTailSize;
			sparseMemoryBind.memory = deviceMemory;

			opaqueMemoryBinds.push_back(sparseMemoryBind);
		}
	} //End Layers and Mips

	TP_INFO(Log::RendererVulkanVirtualTexturePrefix, "Virtual Texture info: Dim ", desc.Width, " x ", desc.Height, " Pages ",
		static_cast<uint32_t>((*static_cast<std::vector<VkSparseMemoryBind>*>(m_SVT->Pages.get())).size()));

	//Check if format has one mip tail for all layers
	if((sparseMemoryReq.formatProperties.flags & VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT) && (sparseMemoryReq.imageMipTailFirstLod < desc.MipLevels))
	{
		//Allocate memory for the mip tail
		VkMemoryAllocateInfo allocInfo = VulkanInits::MemoryAllocateInfo(sparseMemoryReq.imageMipTailSize, m_SVT->SparseMemoryTypeIndex);

		VkDeviceMemory deviceMemory;
		VkCall(vkAllocateMemory(m_device->GetVkDevice(), &allocInfo, nullptr, &deviceMemory));

		//(Opaque) sparse memory binding
		VkSparseMemoryBind sparseMemoryBind{};
		sparseMemoryBind.resourceOffset = sparseMemoryReq.imageMipTailOffset;
		sparseMemoryBind.size = sparseMemoryReq.imageMipTailSize;
		sparseMemoryBind.memory = deviceMemory;

		opaqueMemoryBinds.push_back(sparseMemoryBind);
	}

	m_SVT->LastFilledMip = m_SVT->MipTailStart - 1;

	//Create image view
	VkImageViewCreateInfo view{};
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.pNext = nullptr;
	view.flags = 0;
	view.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view.format = format;
	view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view.subresourceRange.baseMipLevel = 0;
	view.subresourceRange.baseArrayLayer = 0;
	view.subresourceRange.layerCount = 1;
	view.subresourceRange.levelCount = desc.MipLevels;
	view.image = m_vkImage;
	m_aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkCall(vkCreateImageView(m_device->GetVkDevice(), &view, nullptr, &m_vkSRVDescriptor));

	RendererAPI::TextureBarrier textureBarrier{ TRAP::Ref<VulkanTexture>(this), RendererAPI::ResourceState::Undefined, RendererAPI::ResourceState::CopyDestination };
	cmd->ResourceBarrier({}, { textureBarrier }, {});

	//Fill smallest (non-tail) mip map level
	FillVirtualTextureLevel(cmd, tiledMipLevel - 1);

	m_ownsImage = true;
	m_mipLevels = desc.MipLevels;
	m_width = desc.Width;
	m_height = desc.Height;
	m_depth = desc.Depth;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::~VulkanTexture()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Destroying Texture");
#endif

	if(m_ownsImage)
	{
		if (m_vkImage && !m_SVT)
		{
			if(RendererAPI::ImageFormatIsSinglePlane(m_format))
			{
				vmaDestroyImage(m_vma->GetVMAAllocator(), m_vkImage, m_vkAllocation);
			}
			else
			{
				vkDestroyImage(m_device->GetVkDevice(), m_vkImage, nullptr);
				vkFreeMemory(m_device->GetVkDevice(), m_vkDeviceMemory, nullptr);
			}			
		}
		else if (m_vkImage && m_SVT)
			vkDestroyImage(m_device->GetVkDevice(), m_vkImage, nullptr);
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

	if (m_SVT)
		RemoveVirtualTexture();
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

TRAP::Ref<TRAP::Graphics::RendererAPI::VirtualTexture> TRAP::Graphics::API::VulkanTexture::GetSVT() const
{
	return m_SVT;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanTexture::OwnsImage() const
{
	return m_ownsImage;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::FillVirtualTexture(VulkanCommandBuffer& cmd)
{
	std::vector<RendererAPI::VirtualTexturePage>& pageTable = *static_cast<std::vector<RendererAPI::VirtualTexturePage>*>(m_SVT->Pages.get());
	std::vector<VkSparseImageMemoryBind>& imageMemory = *static_cast<std::vector<VkSparseImageMemoryBind>*>(m_SVT->SparseImageMemoryBinds.get());
	std::vector<VkSparseMemoryBind>& opaqueMemoryBinds = *static_cast<std::vector<VkSparseMemoryBind>*>(m_SVT->OpaqueMemoryBinds.get());

	imageMemory.resize(0);

	const uint32_t alivePageCount = static_cast<const PageCounts*>(m_SVT->PageCounts->GetCPUMappedAddress())->AlivePageCount;

	std::vector<uint32_t> visibilityData;
	visibilityData.resize(alivePageCount);
	std::memcpy(visibilityData.data(), m_SVT->AlivePage->GetCPUMappedAddress(), visibilityData.size() * sizeof(uint32_t));

	for(uint32_t i = 0; i < visibilityData.size(); ++i)
	{
		const uint32_t pageIndex = visibilityData[i];
		RendererAPI::VirtualTexturePage& page = pageTable[pageIndex];

		if(AllocateVirtualPage(page, m_SVT->SparseMemoryTypeIndex))
		{
			void* data = static_cast<void*>(m_SVT->VirtualImageData.data() + (pageIndex + page.Size));

			std::memcpy(page.IntermediateBuffer->GetCPUMappedAddress(), data, page.Size);

			//Copy image to VkImage
			VkBufferImageCopy region{};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.mipLevel = page.MipLevel;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = { page.Offset.x, page.Offset.y, 0 };
			region.imageExtent = { static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageWidth), static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageHeight), 1 };

			vkCmdCopyBufferToImage(cmd.GetVkCommandBuffer(), page.IntermediateBuffer->GetVkBuffer(), m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			//Update list of memory-backed sparse image memory binds
			imageMemory.push_back(page.ImageMemoryBind);
		}
	}

	//Update sparse bind info
	if(!imageMemory.empty())
	{
		m_SVT->BindSparseInfo = {};
		m_SVT->BindSparseInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;

		//Image memory binds
		m_SVT->ImageMemoryBindInfo = {};
		m_SVT->ImageMemoryBindInfo.image = m_vkImage;
		m_SVT->ImageMemoryBindInfo.bindCount = static_cast<uint32_t>(imageMemory.size());
		m_SVT->ImageMemoryBindInfo.pBinds = imageMemory.data();
		m_SVT->BindSparseInfo.imageBindCount = (m_SVT->ImageMemoryBindInfo.bindCount > 0) ? 1 : 0;
		m_SVT->BindSparseInfo.pImageBinds = &m_SVT->ImageMemoryBindInfo;

		//Opaque image memory binds (mip tail)
		m_SVT->OpaqueMemoryBindInfo.image = m_vkImage;
		m_SVT->OpaqueMemoryBindInfo.bindCount = static_cast<uint32_t>(opaqueMemoryBinds.size());
		m_SVT->OpaqueMemoryBindInfo.pBinds = opaqueMemoryBinds.data();
		m_SVT->BindSparseInfo.imageOpaqueBindCount = (m_SVT->OpaqueMemoryBindInfo.bindCount > 0) ? 1 : 0;
		m_SVT->BindSparseInfo.pImageOpaqueBinds = &m_SVT->OpaqueMemoryBindInfo;

		VkCall(vkQueueBindSparse(cmd.GetQueue()->GetVkQueue(), static_cast<uint32_t>(1), &m_SVT->BindSparseInfo, VK_NULL_HANDLE));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::ReleasePage()
{
	std::vector<RendererAPI::VirtualTexturePage>& pageTable = *static_cast<std::vector<RendererAPI::VirtualTexturePage>*>(m_SVT->Pages.get());

	const uint32_t removePageCount = static_cast<const PageCounts*>(m_SVT->PageCounts->GetCPUMappedAddress())->RemovePageCount;

	if (removePageCount == 0)
		return;

	std::vector<uint32_t> removePageTable;
	removePageTable.resize(removePageCount);

	std::memcpy(removePageTable.data(), m_SVT->RemovePage->GetCPUMappedAddress(), sizeof(uint32_t));

	for(uint32_t i = 0; i < removePageCount; ++i)
	{
		const uint32_t removeIndex = removePageTable[i];
		ReleaseVirtualPage(pageTable[removeIndex], false);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::RemoveVirtualTexture()
{
	auto* pageTable =  static_cast<std::vector<RendererAPI::VirtualTexturePage>*>(m_SVT->Pages.get());
	if(pageTable)
	{
		for (uint32_t i = 0; i < pageTable->size(); i++)
			ReleaseVirtualPage((*pageTable)[i], true);

		pageTable->resize(0);
		m_SVT->Pages.reset();
		m_SVT->Pages = nullptr;
	}

	auto* imageMemory = static_cast<std::vector<VkSparseImageMemoryBind>*>(m_SVT->SparseImageMemoryBinds.get());
	if(imageMemory)
	{
		imageMemory->resize(0);
		m_SVT->SparseImageMemoryBinds.reset();
		m_SVT->SparseImageMemoryBinds = nullptr;
	}

	auto* opaqueMemory = static_cast<std::vector<VkSparseMemoryBind>*>(m_SVT->OpaqueMemoryBinds.get());
	if(opaqueMemory)
	{
		for (auto& i : *opaqueMemory)
			vkFreeMemory(m_device->GetVkDevice(), i.memory, nullptr);

		opaqueMemory->resize(0);
		m_SVT->OpaqueMemoryBinds = nullptr;
	}

	if (m_SVT->Visibility)
	{
		m_SVT->Visibility.reset();
		m_SVT->Visibility = nullptr;
	}

	if(m_SVT->PrevVisibility)
	{
		m_SVT->PrevVisibility.reset();
		m_SVT->PrevVisibility = nullptr;
	}

	if(m_SVT->AlivePage)
	{
		m_SVT->AlivePage.reset();
		m_SVT->AlivePage = nullptr;
	}

	if(m_SVT->RemovePage)
	{
		m_SVT->RemovePage.reset();
		m_SVT->RemovePage = nullptr;
	}

	if(m_SVT->PageCounts)
	{
		m_SVT->PageCounts.reset();
		m_SVT->PageCounts = nullptr;
	}

	if(!m_SVT->VirtualImageData.empty())
	{
		m_SVT->VirtualImageData.clear();
		m_SVT->VirtualImageData = {};
	}
}

//-------------------------------------------------------------------------------------------------------------------//

uint32_t TRAP::Graphics::API::VulkanTexture::GetMemoryType(uint32_t typeBits,
                                                           const VkPhysicalDeviceMemoryProperties& memProps,
                                                           const VkMemoryPropertyFlags props,
                                                           VkBool32* memTypeFound)
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

	TP_ERROR(Log::RendererVulkanVirtualTexturePrefix, "Could not find a matching memory type");
	TRAP_ASSERT(false);
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------//

VkExtent3D TRAP::Graphics::API::VulkanTexture::AlignedDivision(const VkExtent3D& extent, const VkExtent3D& granularity)
{
	VkExtent3D out{};
	out.width = (extent.width / granularity.width + ((extent.width % granularity.width) ? 1u : 0u));
	out.height = (extent.height / granularity.height + ((extent.height % granularity.height) ? 1u : 0u));
	out.depth = (extent.depth / granularity.depth + ((extent.depth % granularity.depth) ? 1u : 0u));

	return out;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::FillVirtualTextureLevel(const TRAP::Ref<VulkanCommandBuffer>& cmd, const uint32_t mipLevel)
{
	std::vector<RendererAPI::VirtualTexturePage>& pageTable = *static_cast<std::vector<RendererAPI::VirtualTexturePage>*>(m_SVT->Pages.get());

	//Bind Data
	std::vector<VkSparseImageMemoryBind>& imageMemory = *static_cast<std::vector<VkSparseImageMemoryBind>*>(m_SVT->SparseImageMemoryBinds.get());
	std::vector<VkSparseMemoryBind>& opaqueMemoryBinds = *static_cast<std::vector<VkSparseMemoryBind>*>(m_SVT->OpaqueMemoryBinds.get());

	for(uint32_t i = 0; i < static_cast<uint32_t>(m_SVT->VirtualPageTotalCount); i++)
	{
		RendererAPI::VirtualTexturePage& page = pageTable[i];
		const uint32_t pageIndex = page.Index;

		if((page.MipLevel == mipLevel) && (page.ImageMemoryBind.memory == VK_NULL_HANDLE))
		{
			if(AllocateVirtualPage(page, m_SVT->SparseMemoryTypeIndex))
			{
				void* data = static_cast<void*>(m_SVT->VirtualImageData.data() + (pageIndex * static_cast<uint32_t>(page.Size)));

				//CPU to GPU
				std::memcpy(page.IntermediateBuffer->GetCPUMappedAddress(), data, page.Size);

				//Copy image to VkImage
				VkBufferImageCopy region{};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;

				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel = mipLevel;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;

				region.imageOffset = { page.Offset.x, page.Offset.y, 0 };
				region.imageExtent = { static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageWidth), static_cast<uint32_t>(m_SVT->SparseVirtualTexturePageHeight), 1 };

				vkCmdCopyBufferToImage(cmd->GetVkCommandBuffer(), page.IntermediateBuffer->GetVkBuffer(), m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
			}

			//Update list of memory-backed sparse image memory binds
			imageMemory.push_back(page.ImageMemoryBind);
		}
	}

	//Update sparse bind info
	{
		m_SVT->BindSparseInfo = {};
		m_SVT->BindSparseInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;

		//Image memory binds
		m_SVT->ImageMemoryBindInfo = {};
		m_SVT->ImageMemoryBindInfo.image = m_vkImage;
		m_SVT->ImageMemoryBindInfo.bindCount = static_cast<uint32_t>(imageMemory.size());
		m_SVT->ImageMemoryBindInfo.pBinds = imageMemory.data();
		m_SVT->BindSparseInfo.imageBindCount = (m_SVT->ImageMemoryBindInfo.bindCount > 0) ? 1 : 0;
		m_SVT->BindSparseInfo.pImageBinds = &m_SVT->ImageMemoryBindInfo;

		//Opaque image memory binds (mip tail)
		m_SVT->OpaqueMemoryBindInfo.image = m_vkImage;
		m_SVT->OpaqueMemoryBindInfo.bindCount = static_cast<uint32_t>(opaqueMemoryBinds.size());
		m_SVT->OpaqueMemoryBindInfo.pBinds = opaqueMemoryBinds.data();
		m_SVT->BindSparseInfo.imageOpaqueBindCount = (m_SVT->OpaqueMemoryBindInfo.bindCount > 0) ? 1 : 0;
		m_SVT->BindSparseInfo.pImageOpaqueBinds = &m_SVT->OpaqueMemoryBindInfo;

		VkCall(vkQueueBindSparse(cmd->GetQueue()->GetVkQueue(), static_cast<uint32_t>(1), &m_SVT->BindSparseInfo, VK_NULL_HANDLE));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::VirtualTexturePage* TRAP::Graphics::API::VulkanTexture::AddPage(const VkOffset3D& offset,
                                                                                             const VkExtent3D& extent,
                                                                                             const VkDeviceSize size,
                                                                                             const uint32_t mipLevel,
                                                                                             const uint32_t layer) const
{
	std::vector<RendererAPI::VirtualTexturePage>& pageTable = *static_cast<std::vector<RendererAPI::VirtualTexturePage>*>(m_SVT->Pages.get());

	RendererAPI::VirtualTexturePage newPage{};
	newPage.Offset = offset;
	newPage.Extent = extent;
	newPage.Size = size;
	newPage.MipLevel = mipLevel;
	newPage.Layer = layer;
	newPage.Index = static_cast<uint32_t>(pageTable.size());

	pageTable.push_back(newPage);

	return &pageTable.back();
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::API::VulkanTexture::AllocateVirtualPage(RendererAPI::VirtualTexturePage& virtualPage, const uint32_t memoryTypeIndex)
{
	if (virtualPage.ImageMemoryBind.memory != VK_NULL_HANDLE)
		//Already filled
		return false;

	RendererAPI::BufferDesc desc{};
	desc.Descriptors = RendererAPI::DescriptorType::RWBuffer;
	desc.MemoryUsage = RendererAPI::ResourceMemoryUsage::CPUToGPU;
	desc.Flags = RendererAPI::BufferCreationFlags::PersistentMap;

	desc.FirstElement = 0;
	desc.ElementCount = m_SVT->SparseVirtualTexturePageWidth * m_SVT->SparseVirtualTexturePageHeight;
	desc.StructStride = sizeof(uint32_t);
	desc.Size = desc.ElementCount * desc.StructStride;
	virtualPage.IntermediateBuffer = TRAP::MakeRef<VulkanBuffer>(m_device, m_vma, desc);

	virtualPage.ImageMemoryBind = {};

	VkMemoryAllocateInfo allocInfo = VulkanInits::MemoryAllocateInfo(virtualPage.Size, memoryTypeIndex);

	VkCall(vkAllocateMemory(m_device->GetVkDevice(), &allocInfo, nullptr, &virtualPage.ImageMemoryBind.memory));

	VkImageSubresource subResource{};
	subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subResource.mipLevel = virtualPage.MipLevel;
	subResource.arrayLayer = virtualPage.Layer;

	//Sparse image memory binding
	virtualPage.ImageMemoryBind.subresource = subResource;
	virtualPage.ImageMemoryBind.extent = virtualPage.Extent;
	virtualPage.ImageMemoryBind.offset = virtualPage.Offset;

	return true;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::ReleaseVirtualPage(RendererAPI::VirtualTexturePage& virtualPage, const bool removeMemoryBind) const
{
	if(removeMemoryBind && virtualPage.ImageMemoryBind.memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(m_device->GetVkDevice(), virtualPage.ImageMemoryBind.memory, nullptr);
		virtualPage.ImageMemoryBind.memory = VK_NULL_HANDLE;
	}

	if(virtualPage.IntermediateBuffer)
		virtualPage.IntermediateBuffer.reset();
}

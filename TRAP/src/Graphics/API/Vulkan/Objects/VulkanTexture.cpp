#include "TRAPPCH.h"
#include "VulkanTexture.h"

#include "VulkanCommandBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

namespace
{
#ifdef ENABLE_GRAPHICS_DEBUG
	void SetTextureName(const VkDevice& device, const std::string_view name, VkImage image, VkImageView imageSRV,
	                    VkImageView imageSRVStencil, const std::span<const VkImageView> imageUAV)
	{
		ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

		if(name.empty())
			return;

		TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(image), VK_OBJECT_TYPE_IMAGE, name);

		if(imageSRV != VK_NULL_HANDLE)
			TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(imageSRV), VK_OBJECT_TYPE_IMAGE_VIEW, name);
		if(imageSRVStencil != VK_NULL_HANDLE)
			TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(imageSRVStencil), VK_OBJECT_TYPE_IMAGE_VIEW, name);
		for(VkImageView uav : imageUAV)
		{
			if(uav == VK_NULL_HANDLE)
				continue;

			TRAP::Graphics::API::VkSetObjectName(device, std::bit_cast<u64>(uav), VK_OBJECT_TYPE_IMAGE_VIEW, name);
		}
	}
#endif /*ENABLE_GRAPHICS_DEBUG*/

	//-------------------------------------------------------------------------------------------------------------------//

	/// @brief Retrieve the memory type index via the given parameters.
	/// @param typeBits Memory type bits from Vulkan memory requirements.
	/// @param memProps Vulkan physical device memory properties.
	/// @param props Vulkan memory property flags.
	/// @param memTypeFound Output: True if memory type found, false otherwise.
	/// @return Index of memory type.
	[[nodiscard]] constexpr TRAP::Optional<u32> GetMemoryType(u32 typeBits,
	                                                          const VkPhysicalDeviceMemoryProperties& memProps,
	                                                          const VkMemoryPropertyFlags props)
	{
		for (u32 i = 0; i < memProps.memoryTypeCount; i++)
		{
			if ((typeBits & 1u) == 1 && (memProps.memoryTypes[i].propertyFlags & props) == props)
				return i;

			typeBits >>= 1u;
		}

		return TRAP::NullOpt;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr VkImageUsageFlags StartStateToVkImageUsageFlags(const TRAP::Graphics::RendererAPI::ResourceState startState)
	{
		using enum TRAP::Graphics::RendererAPI::ResourceState;

		VkImageUsageFlags usageFlags = 0;

		if ((startState & RenderTarget) != Undefined)
			usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		else if ((startState & DepthWrite) != Undefined)
			usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		if ((startState & ShadingRateSource) != Undefined)
			usageFlags |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

		return usageFlags;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr VkImageType GetVkImageType(const TRAP::Graphics::RendererAPI::TextureDesc& desc)
	{
		if ((desc.Flags & TRAP::Graphics::RendererAPI::TextureCreationFlags::Force2D) != TRAP::Graphics::RendererAPI::TextureCreationFlags::None)
		{
			TRAP_ASSERT(desc.Depth == 1, "VulkanTexture::Init(): 2D Texture cannot have depth");
			return VK_IMAGE_TYPE_2D;
		}

		if ((desc.Flags & TRAP::Graphics::RendererAPI::TextureCreationFlags::Force3D) != TRAP::Graphics::RendererAPI::TextureCreationFlags::None)
			return VK_IMAGE_TYPE_3D;

		if (desc.Depth > 1)
			return VK_IMAGE_TYPE_3D;

		if (desc.Height > 1)
			return VK_IMAGE_TYPE_2D;

		return VK_IMAGE_TYPE_1D;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorType GetFinalDescriptorTypes(const TRAP::Graphics::RendererAPI::TextureDesc& desc)
	{
		TRAP::Graphics::RendererAPI::DescriptorType descriptors = desc.Descriptors;

		if((desc.Flags & TRAP::Graphics::RendererAPI::TextureCreationFlags::Storage) != TRAP::Graphics::RendererAPI::TextureCreationFlags::None)
			descriptors |= TRAP::Graphics::RendererAPI::DescriptorType::RWTexture;

		return descriptors;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] constexpr VkImageViewType GetVkImageViewType(const VkImageType imageType,
	                                                           const TRAP::Graphics::RendererAPI::TextureDesc& desc,
															   const bool cubeMapRequired)
	{
		switch (imageType)
		{
		case VK_IMAGE_TYPE_1D:
			return (desc.ArraySize > 1) ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;

		case VK_IMAGE_TYPE_2D:
		{
			if (cubeMapRequired)
				return (desc.ArraySize > 6) ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
			return (desc.ArraySize > 1) ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
		}

		case VK_IMAGE_TYPE_3D:
		{
			if (desc.ArraySize > 1)
			{
				TP_ERROR(TRAP::Log::RendererVulkanTexturePrefix, "Cannot support 3D Texture Array in Vulkan");
				TRAP_ASSERT(false, "VulkanTexture::Init(): Cannot support 3D Texture Array in Vulkan");
			}
			return VK_IMAGE_VIEW_TYPE_3D;
		}

		default:
			TRAP_ASSERT(false, "VulkanTexture::Init(): Image Format not supported!");
			return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] bool SetupLazyAllocation(VkImageCreateInfo& imageInfo, VmaAllocationCreateInfo& memReqs,
	                                       const TRAP::Graphics::API::VulkanMemoryAllocator& vma)
	{
		u32 memoryTypeIndex = 0;
		VmaAllocationCreateInfo lazyMemReqs = memReqs;
		lazyMemReqs.usage = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
		const VkResult result = vmaFindMemoryTypeIndex(vma.GetVMAAllocator(),
													   std::numeric_limits<u32>::max(),
													   &lazyMemReqs, &memoryTypeIndex);
		if (result == VK_SUCCESS)
		{
			memReqs = lazyMemReqs;
			imageInfo.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
			// The Vulkan spec states: If usage includes VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
			// then bits other than VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			// VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, and VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
			// must not be set.
			imageInfo.usage &= (VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
						        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
			return true;
		}

		return false;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkImageCreateInfo CreateVkImageCreateInfo(const TRAP::Graphics::RendererAPI::TextureDesc& desc,
                                                            const VkImageType imageType,
													        const bool cubeMapRequired,
															const TRAP::Graphics::RendererAPI::DescriptorType descriptors,
															const TRAP::Graphics::API::VulkanDevice& device)
	{
		VkImageCreateInfo info = TRAP::Graphics::API::VulkanInits::ImageCreateInfo(imageType, ImageFormatToVkFormat(desc.Format),
															                       desc.Width, desc.Height, desc.Depth, desc.MipLevels,
															                       desc.ArraySize,
															                       TRAP::Graphics::API::SampleCountToVkSampleCount(desc.SampleCount),
															                       VK_IMAGE_TILING_OPTIMAL,
															                       TRAP::Graphics::API::DescriptorTypeToVkImageUsage(descriptors));
		info.usage |= StartStateToVkImageUsageFlags(desc.StartState);

		if (cubeMapRequired)
			info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		const bool arrayRequired = imageType == VK_IMAGE_TYPE_3D;
		if (arrayRequired)
			info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;


		// Multi-Planar formats must have each plane separately bound to memory, rather than having a single memory binding for the whole image
		if (TRAP::Graphics::API::ImageFormatIsPlanar(desc.Format))
		{
			TRAP_ASSERT(device.GetPhysicalDevice().GetVkPhysicalDeviceFormatProperties(desc.Format).optimalTilingFeatures & VK_FORMAT_FEATURE_DISJOINT_BIT,
			            "VulkanTexture::Init(): Format does not support disjoint planes");
			info.flags |= VK_IMAGE_CREATE_DISJOINT_BIT;
		}

		if (((info.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0u) || ((info.usage & VK_IMAGE_USAGE_STORAGE_BIT) != 0u))
		{
			// Make it easy to copy to and from textures
			info.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		}

		return info;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	[[nodiscard]] VkDeviceMemory AllocateDeviceMemory(const TRAP::Graphics::API::VulkanDevice& device, VkImage image,
	                                                  const u32 numOfPlanes, std::vector<u64>& outPlaneOffsets)
	{
		//Allocate Memory
		VkMemoryRequirements memReq{};
		TRAP::Graphics::API::UtilGetPlanarVkImageMemoryRequirement(device.GetVkDevice(), image, numOfPlanes, memReq,
		                                                           outPlaneOffsets);

		// Allocate image memory
		const VkPhysicalDeviceMemoryProperties& memProps = device.GetPhysicalDevice().GetVkPhysicalDeviceMemoryProperties();
		const auto memTypeIdx = GetMemoryType(memReq.memoryTypeBits, memProps, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if(!memTypeIdx)
			TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoMatchingMemoryTypeFound);

		const VkMemoryAllocateInfo memAllocInfo
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = memReq.size,
			.memoryTypeIndex = *memTypeIdx
		};

		VkDeviceMemory deviceMem = VK_NULL_HANDLE;
		VkCall(vkAllocateMemory(device.GetVkDevice(), &memAllocInfo, nullptr, &deviceMem));

		return deviceMem;
	}

	//-------------------------------------------------------------------------------------------------------------------//

	void BindDeviceMemory(const TRAP::Graphics::API::VulkanDevice& device, VkImage image, VkDeviceMemory imageMemory,
	                      const u32 numOfPlanes, const std::vector<u64>& planeOffsets)
	{
		// Bind planes to their memories
		std::vector<VkBindImageMemoryInfo> bindImagesMemoryInfo(numOfPlanes);
		std::vector<VkBindImagePlaneMemoryInfo> bindImagePlanesMemoryInfo(numOfPlanes);
		for (u32 i = 0; i < numOfPlanes; ++i)
		{
			bindImagePlanesMemoryInfo[i] = VkBindImagePlaneMemoryInfo
			{
				.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO,
				.pNext = nullptr,
				.planeAspect = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_PLANE_0_BIT << i)
			};

			bindImagesMemoryInfo[i] = VkBindImageMemoryInfo
			{
				.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
				.pNext = &bindImagePlanesMemoryInfo[i],
				.image = image,
				.memory = imageMemory,
				.memoryOffset = planeOffsets[i]
			};
		}

		VkCall(vkBindImageMemory2(device.GetVkDevice(), numOfPlanes, bindImagesMemoryInfo.data()));
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(std::string name)
	: Texture(std::move(name))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(std::string name, std::vector<std::filesystem::path> filePaths)
	: Texture(std::move(name), std::move(filePaths))
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(std::string name, std::vector<std::filesystem::path> filePaths,
                                                  const TRAP::Optional<TextureCubeFormat>& cubeFormat)
	: Texture(std::move(name), std::move(filePaths), cubeFormat)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::~VulkanTexture()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::Init(const RendererAPI::TextureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize), "VulkanTexture::Init(): Invalid resolution");
	TRAP_ASSERT(!(desc.SampleCount > RendererAPI::SampleCount::One && desc.MipLevels > 1), "VulkanTexture::Init(): Multi-Sampled texture cannot have mip maps");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Creating Texture");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if (desc.NativeHandle != nullptr)
	{
		m_ownsImage = false;
		m_vkImage = static_cast<VkImage>(desc.NativeHandle);
	}
	else
		m_ownsImage = true;

	const RendererAPI::DescriptorType descriptors = GetFinalDescriptorTypes(desc);
	const VkImageType imageType = GetVkImageType(desc);

	const bool cubeMapRequired = (descriptors & RendererAPI::DescriptorType::TextureCube) ==
						         RendererAPI::DescriptorType::TextureCube;

	if (m_vkImage == VK_NULL_HANDLE)
		CreateVkImage(desc, imageType, cubeMapRequired, descriptors);

	CreateVkImageViews(desc, imageType, cubeMapRequired, descriptors);

	m_aspectMask = DetermineAspectMask(ImageFormatToVkFormat(desc.Format), true);
	m_width = desc.Width;
	m_height = desc.Height;
	m_depth = desc.Depth;
	m_mipLevels = desc.MipLevels;
	m_arraySize = desc.ArraySize;
	m_imageFormat = desc.Format;
	m_descriptorTypes |= descriptors;

	if (m_name.empty() && !desc.Name.empty())
		m_name = desc.Name;

#if defined(ENABLE_GRAPHICS_DEBUG)
	SetTextureName(m_device->GetVkDevice(), m_name, m_vkImage, m_vkSRVDescriptor, m_vkSRVStencilDescriptor,
	               m_vkUAVDescriptors);
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Destroying Texture");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if (m_vkSRVDescriptor != VK_NULL_HANDLE)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkSRVDescriptor, nullptr);

	if (m_vkSRVStencilDescriptor != VK_NULL_HANDLE)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkSRVStencilDescriptor, nullptr);

	for (const auto& imageView : m_vkUAVDescriptors)
	{
		if(imageView != VK_NULL_HANDLE)
			vkDestroyImageView(m_device->GetVkDevice(), imageView, nullptr);
	}
	m_vkUAVDescriptors.clear();

	if (m_ownsImage && (m_vkImage != VK_NULL_HANDLE))
	{
		if (TRAP::Graphics::API::ImageFormatIsSinglePlane(m_imageFormat))
			vmaDestroyImage(m_vma->GetVMAAllocator(), m_vkImage, m_vkAllocation);
		else
		{
			vkDestroyImage(m_device->GetVkDevice(), m_vkImage, nullptr);
			vkFreeMemory(m_device->GetVkDevice(), m_vkDeviceMemory, nullptr);
		}
	}

	m_syncToken = 0;
	m_lazilyAllocated = false;
	m_vkSRVDescriptor = VK_NULL_HANDLE;
	m_vkSRVStencilDescriptor = VK_NULL_HANDLE;
	m_vkImage = VK_NULL_HANDLE;
	m_vkAllocation = {};
	m_vkDeviceMemory = {};
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::PreInit()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const auto *vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer *>(TRAP::Graphics::RendererAPI::GetRenderer());

	m_device = vkRenderer->GetDevice();
	m_vma = vkRenderer->GetVMA();

	TRAP_ASSERT(m_device, "VulkanTexture::PreInit(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_vma, "VulkanTexture::PreInit(): VMA is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::CreateVkImage(const TRAP::Graphics::RendererAPI::TextureDesc& desc,
                                                       const VkImageType imageType,
													   const bool cubeMapRequired,
													   const TRAP::Graphics::RendererAPI::DescriptorType descriptors)
{
	const u32 numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(desc.Format);
	const bool isSinglePlane = TRAP::Graphics::API::ImageFormatIsSinglePlane(desc.Format);
	TRAP_ASSERT(((isSinglePlane && numOfPlanes == 1) || (!isSinglePlane && numOfPlanes > 1 && numOfPlanes <= 3)),
				"VulkanTexture::Init(): Number of planes for multi-planar formats must be 2 or 3 and for single-planar "
				"formats it must be 1");

	VkImageCreateInfo info = CreateVkImageCreateInfo(desc, imageType, cubeMapRequired, descriptors, *m_device);

	TRAP_ASSERT(VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[std::to_underlying(desc.Format)],
				"VulkanTexture::Init(): GPU shader can't read from this format");
	TRAP_ASSERT(0 != (m_device->GetPhysicalDevice().GetVkPhysicalDeviceFormatProperties(desc.Format).optimalTilingFeatures & VkImageUsageToFormatFeatures(info.usage)),
	            "VulkanTexture::Init(): Format is not suported for GPU local images (i.e. not host visible images)");

	if (isSinglePlane)
	{
		VmaAllocationCreateInfo memReqs{};
		if ((desc.Flags & RendererAPI::TextureCreationFlags::OwnMemory) != RendererAPI::TextureCreationFlags::None)
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		memReqs.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		// If lazy allocation is requested check that the hardware supports it
		const bool lazyAllocation = (desc.Flags & RendererAPI::TextureCreationFlags::OnTile) != RendererAPI::TextureCreationFlags::None;
		if (lazyAllocation)
			m_lazilyAllocated = SetupLazyAllocation(info, memReqs, *m_vma);

		VkCall(vmaCreateImage(m_vma->GetVMAAllocator(), &info, &memReqs, &m_vkImage, &m_vkAllocation,
							  nullptr));
	}
	else // Multi-Planar formats
	{
		// Create info requires the mutable format flag set for multi planar images
		// Also pass the format list for mutable formats as per recommendation from the spec
		// Might help to keep DCC enabled if we ever use this as a output format
		// DCC gets disabled when we pass mutable format bit to the create info.
		// Passing the format list helps the driver to enable it
		const VkFormat planarFormat = ImageFormatToVkFormat(desc.Format);
		const VkImageFormatListCreateInfo formatList
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO,
			.pNext = nullptr,
			.viewFormatCount = 1,
			.pViewFormats = &planarFormat
		};

		info.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
		info.pNext = &formatList;

		// Create Image
		VkCall(vkCreateImage(m_device->GetVkDevice(), &info, nullptr, &m_vkImage));

		std::vector<u64> planeOffsets{};
		m_vkDeviceMemory = AllocateDeviceMemory(*m_device, m_vkImage, numOfPlanes, planeOffsets);

		BindDeviceMemory(*m_device, m_vkImage, m_vkDeviceMemory, numOfPlanes, planeOffsets);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::CreateVkImageViews(const TRAP::Graphics::RendererAPI::TextureDesc& desc,
                                                            const VkImageType imageType, const bool cubeMapRequired,
                                                            const TRAP::Graphics::RendererAPI::DescriptorType descriptors)
{
	const VkImageViewType imageViewType = GetVkImageViewType(imageType, desc, cubeMapRequired);
	const VkFormat imageFormat = ImageFormatToVkFormat(desc.Format);

	VkImageViewCreateInfo imageViewDesc = VulkanInits::ImageViewCreateInfo(m_vkImage, imageViewType, imageFormat,
																	       desc.MipLevels, desc.ArraySize);

	if (desc.VkSamplerYcbcrConversionInfo != nullptr)
		imageViewDesc.pNext = desc.VkSamplerYcbcrConversionInfo;

	//SRV
	if ((descriptors & RendererAPI::DescriptorType::Texture) != RendererAPI::DescriptorType::Undefined)
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &imageViewDesc, nullptr, &m_vkSRVDescriptor));

	//SRV Stencil
	if (TRAP::Graphics::API::ImageFormatHasStencil(desc.Format) &&
		((descriptors & RendererAPI::DescriptorType::Texture) != RendererAPI::DescriptorType::Undefined))
	{
		imageViewDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &imageViewDesc, nullptr, &m_vkSRVStencilDescriptor));
	}

	//UAV
	if ((descriptors & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
	{
		m_vkUAVDescriptors.resize(desc.MipLevels);

		// Note: We dont support imageCube, imageCubeArray for consistency with other APIs
		// All cubemaps will be used as image2DArray for Image Load / Store ops
		if (imageViewDesc.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY ||
		    imageViewDesc.viewType == VK_IMAGE_VIEW_TYPE_CUBE)
		{
			imageViewDesc.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		}
		imageViewDesc.subresourceRange.levelCount = 1;
		for (u32 i = 0; i < desc.MipLevels; ++i)
		{
			imageViewDesc.subresourceRange.baseMipLevel = i;
			VkCall(vkCreateImageView(m_device->GetVkDevice(), &imageViewDesc, nullptr, &m_vkUAVDescriptors[i]));
		}
	}
}

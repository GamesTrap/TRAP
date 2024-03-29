#include "TRAPPCH.h"
#include "VulkanTexture.h"

#include "VulkanCommandBuffer.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Utils/ErrorCodes/ErrorCodes.h"

TRAP::Graphics::API::VulkanTexture::VulkanTexture()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(std::string name, std::array<std::filesystem::path, 6> filepaths)
	: Texture(std::move(name), std::move(filepaths))
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(std::string name, std::filesystem::path filepath,
												  const TextureType type, const TextureCubeFormat cubeFormat)
	: Texture(std::move(name), std::move(filepath), type, cubeFormat)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::VulkanTexture(const TextureType type)
	: Texture(type)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	PreInit();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanTexture::~VulkanTexture()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::Init(const RendererAPI::TextureDesc &desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(desc.Width && desc.Height && (desc.Depth || desc.ArraySize), "VulkanTexture::Init(): Invalid resolution");
	TRAP_ASSERT(!(desc.SampleCount > RendererAPI::SampleCount::One && desc.MipLevels > 1), "VulkanTexture::Init(): Multi-Sampled texture cannot have mip maps");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Creating Texture");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if ((desc.Descriptors & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined ||
	    (desc.Flags & RendererAPI::TextureCreationFlags::Storage) != RendererAPI::TextureCreationFlags::None)
		m_vkUAVDescriptors.resize(desc.MipLevels);

	if ((desc.NativeHandle != nullptr) && ((desc.Flags & RendererAPI::TextureCreationFlags::Import) == RendererAPI::TextureCreationFlags::None))
	{
		m_ownsImage = false;
		m_vkImage = static_cast<VkImage>(desc.NativeHandle);
	}
	else
		m_ownsImage = true;

	VkImageUsageFlags additionalFlags = 0;
	if ((desc.StartState & RendererAPI::ResourceState::RenderTarget) != RendererAPI::ResourceState::Undefined)
		additionalFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	else if ((desc.StartState & RendererAPI::ResourceState::DepthWrite) != RendererAPI::ResourceState::Undefined)
		additionalFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if ((desc.StartState & RendererAPI::ResourceState::ShadingRateSource) != RendererAPI::ResourceState::Undefined)
		additionalFlags |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

	VkImageType imageType = VK_IMAGE_TYPE_MAX_ENUM;
	if ((desc.Flags & RendererAPI::TextureCreationFlags::Force2D) != RendererAPI::TextureCreationFlags::None)
	{
		TRAP_ASSERT(desc.Depth == 1, "VulkanTexture::Init(): 2D Texture cannot have depth");
		imageType = VK_IMAGE_TYPE_2D;
	}
	else if ((desc.Flags & RendererAPI::TextureCreationFlags::Force3D) != RendererAPI::TextureCreationFlags::None)
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
	if((desc.Flags & RendererAPI::TextureCreationFlags::Storage) != RendererAPI::TextureCreationFlags::None)
		descriptors |= RendererAPI::DescriptorType::RWTexture;
	const bool cubeMapRequired = (descriptors & RendererAPI::DescriptorType::TextureCube) ==
						         RendererAPI::DescriptorType::TextureCube;
	bool arrayRequired = false;

	const bool isPlanarFormat = TRAP::Graphics::API::ImageFormatIsPlanar(desc.Format);
	const u32 numOfPlanes = TRAP::Graphics::API::ImageFormatNumOfPlanes(desc.Format);
	const bool isSinglePlane = TRAP::Graphics::API::ImageFormatIsSinglePlane(desc.Format);
	TRAP_ASSERT(((isSinglePlane && numOfPlanes == 1) || (!isSinglePlane && numOfPlanes > 1 && numOfPlanes <= 3)),
				"VulkanTexture::Init(): Number of planes for multi-planar formats must be 2 or 3 and for single-planar "
				"formats it must be 1");

	if (imageType == VK_IMAGE_TYPE_3D)
		arrayRequired = true;

	if (m_vkImage == VK_NULL_HANDLE)
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


		const VkFormatProperties formatProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(info.format);

		// Multi-Planar formats must have each plane separately bound to memory, rather than having a single memory binding for the whole image
		if (isPlanarFormat)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DISJOINT_BIT, "VulkanTexture::Init(): Format does not support disjoint planes");
			info.flags |= VK_IMAGE_CREATE_DISJOINT_BIT;
		}

		if (((info.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0u) || ((info.usage & VK_IMAGE_USAGE_STORAGE_BIT) != 0u))
		{
			// Make it easy to copy to and from textures
			info.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
		}

		TRAP_ASSERT(VulkanRenderer::s_GPUCapBits.CanShaderReadFrom[std::to_underlying(desc.Format)],
					"VulkanTexture::Init(): GPU shader can't read from this format");

		const VkFormatFeatureFlags formatFeatures = VkImageUsageToFormatFeatures(info.usage);

		const VkFormatFeatureFlags flags = formatProps.optimalTilingFeatures & formatFeatures;
		TRAP_ASSERT(0 != flags, "VulkanTexture::Init(): Format is not suported for GPU local images (i.e. not host visible images)");

		VmaAllocationCreateInfo memReqs{};
		if ((desc.Flags & RendererAPI::TextureCreationFlags::OwnMemory) != RendererAPI::TextureCreationFlags::None)
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		memReqs.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkExternalMemoryImageCreateInfo externalInfo{};
		externalInfo.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
		externalInfo.pNext = nullptr;

#ifdef TRAP_PLATFORM_WINDOWS
		VkImportMemoryWin32HandleInfoKHR importInfo{};
		importInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
		importInfo.pNext = nullptr;
#endif /*TRAP_PLATFORM_WINDOWS*/
		VkExportMemoryAllocateInfo exportMemoryInfo{};
		exportMemoryInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
		exportMemoryInfo.pNext = nullptr;

		if (VulkanRenderer::s_externalMemory && (desc.Flags & RendererAPI::TextureCreationFlags::Import) != RendererAPI::TextureCreationFlags::None)
		{
			info.pNext = &externalInfo;

#ifdef TRAP_PLATFORM_WINDOWS
			struct ImportHandleInfo
			{
				void *Handle;
				VkExternalMemoryHandleTypeFlagBits HandleType;
			};

			ImportHandleInfo *handleInfo = static_cast<ImportHandleInfo*>(desc.NativeHandle);
			importInfo.handle = handleInfo->Handle;
			importInfo.handleType = handleInfo->HandleType;

			externalInfo.handleTypes = handleInfo->HandleType;

			memReqs.pUserData = &importInfo;
			// Allocate external (importable / exportable) memory as dedicated memory to avoid
			// unnecessary complexity to the Vulkan Memory Allocator
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
#endif /*TRAP_PLATFORM_WINDOWS*/
		}
		else if (VulkanRenderer::s_externalMemory && (desc.Flags & RendererAPI::TextureCreationFlags::Export) != RendererAPI::TextureCreationFlags::None)
		{
#ifdef TRAP_PLATFORM_WINDOWS
			exportMemoryInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#endif /*TRAP_PLATFORM_WINDOWS*/
			memReqs.pUserData = &exportMemoryInfo;
			// Allocate external (importable / exportable) memory as dedicated memory to avoid
			// unnecessary complexity to the Vulkan Memory Allocator
			memReqs.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}

		// If lazy allocation is requested check that the hardware supports it
		const bool lazyAllocation = (desc.Flags & RendererAPI::TextureCreationFlags::OnTile) != RendererAPI::TextureCreationFlags::None;
		if (lazyAllocation)
		{
			u32 memoryTypeIndex = 0;
			VmaAllocationCreateInfo lazyMemReqs = memReqs;
			lazyMemReqs.usage = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
			const VkResult result = vmaFindMemoryTypeIndex(m_vma->GetVMAAllocator(),
													       std::numeric_limits<u32>::max(),
													       &lazyMemReqs, &memoryTypeIndex);
			if (result == VK_SUCCESS)
			{
				memReqs = lazyMemReqs;
				info.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
				// The Vulkan spec states: If usage includes VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
				// then bits other than VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				// VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, and VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
				// must not be set.
				info.usage &= (VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
							   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT);
				m_lazilyAllocated = true;
			}
		}

		VmaAllocationInfo allocInfo{};
		if (isSinglePlane)
		{
			VkCall(vmaCreateImage(m_vma->GetVMAAllocator(), &info, &memReqs, &m_vkImage, &m_vkAllocation,
								  &allocInfo));
		}
		else // Multi-Planar formats
		{
			// Create info requires the mutable format flag set for multi planar images
			// Also pass the format list for mutable formats as per recommendation from the spec
			// Might help to keep DCC enabled if we ever use this as a output format
			// DCC gets disabled when we pass mutable format bit to the create info.
			// Passing the format list helps the driver to enable it
			const VkFormat planarFormat = ImageFormatToVkFormat(desc.Format);
			VkImageFormatListCreateInfo formatList{};
			formatList.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_LIST_CREATE_INFO;
			formatList.pNext = nullptr;
			formatList.pViewFormats = &planarFormat;
			formatList.viewFormatCount = 1;

			info.flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
			info.pNext = &formatList;

			// Create Image
			VkCall(vkCreateImage(m_device->GetVkDevice(), &info, nullptr, &m_vkImage));

			VkMemoryRequirements memReq{};
			std::vector<u64> planeOffsets(3);
			UtilGetPlanarVkImageMemoryRequirement(m_device->GetVkDevice(), m_vkImage, numOfPlanes, memReq,
												  planeOffsets);

			// Allocate image memory
			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocInfo.allocationSize = memReq.size;
			const VkPhysicalDeviceMemoryProperties &memProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceMemoryProperties();
			memAllocInfo.memoryTypeIndex = GetMemoryType(memReq.memoryTypeBits, memProps,
														 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VkCall(vkAllocateMemory(m_device->GetVkDevice(), &memAllocInfo, nullptr, &m_vkDeviceMemory));

			// Bind planes to their memories
			std::vector<VkBindImageMemoryInfo> bindImagesMemoryInfo(3);
			std::vector<VkBindImagePlaneMemoryInfo> bindImagePlanesMemoryInfo(3);
			for (u32 i = 0; i < numOfPlanes; ++i)
			{
				VkBindImagePlaneMemoryInfo &bindImagePlaneMemoryInfo = bindImagePlanesMemoryInfo[i];
				bindImagePlaneMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
				bindImagePlaneMemoryInfo.pNext = nullptr;
				bindImagePlaneMemoryInfo.planeAspect = static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_PLANE_0_BIT << i);

				VkBindImageMemoryInfo &bindImageMemoryInfo = bindImagesMemoryInfo[i];
				bindImageMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
				bindImageMemoryInfo.pNext = &bindImagePlaneMemoryInfo;
				bindImageMemoryInfo.image = m_vkImage;
				bindImageMemoryInfo.memory = m_vkDeviceMemory;
				bindImageMemoryInfo.memoryOffset = planeOffsets[i];
			}

			VkCall(vkBindImageMemory2(m_device->GetVkDevice(), numOfPlanes, bindImagesMemoryInfo.data()));
		}
	}

	// Create image view
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	switch (imageType)
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
		if (desc.ArraySize > 1)
		{
			TP_ERROR(Log::RendererVulkanTexturePrefix, "Cannot support 3D Texture Array in Vulkan");
			TRAP_ASSERT(false, "VulkanTexture::Init(): Cannot support 3D Texture Array in Vulkan");
		}
		viewType = VK_IMAGE_VIEW_TYPE_3D;
		break;

	default:
		TRAP_ASSERT(false, "VulkanTexture::Init(): Image Format not supported!");
		break;
	}

	TRAP_ASSERT(viewType != VK_IMAGE_VIEW_TYPE_MAX_ENUM, "VulkanTexture::Init(): Invalid Image View");

	// SRV
	VkImageViewCreateInfo srvDesc = VulkanInits::ImageViewCreateInfo(m_vkImage, viewType,
																	 ImageFormatToVkFormat(desc.Format),
																	 desc.MipLevels, desc.ArraySize);
	m_aspectMask = DetermineAspectMask(srvDesc.format, true);

	if (desc.VkSamplerYcbcrConversionInfo != nullptr)
		srvDesc.pNext = desc.VkSamplerYcbcrConversionInfo;

	if ((descriptors & RendererAPI::DescriptorType::Texture) != RendererAPI::DescriptorType::Undefined)
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVDescriptor));

	// SRV stencil
	if ((TRAP::Graphics::API::ImageFormatHasStencil(desc.Format)) &&
		(((descriptors & RendererAPI::DescriptorType::Texture)) != RendererAPI::DescriptorType::Undefined))
	{
		srvDesc.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
		VkCall(vkCreateImageView(m_device->GetVkDevice(), &srvDesc, nullptr, &m_vkSRVStencilDescriptor));
	}

	// UAV
	if ((descriptors & RendererAPI::DescriptorType::RWTexture) != RendererAPI::DescriptorType::Undefined)
	{
		VkImageViewCreateInfo uavDesc = srvDesc;
		// Note: We dont support imageCube, imageCubeArray for consistency with other APIs
		// All cubemaps will be used as image2DArray for Image Load / Store ops
		if (uavDesc.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY || uavDesc.viewType == VK_IMAGE_VIEW_TYPE_CUBE)
			uavDesc.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
		uavDesc.subresourceRange.levelCount = 1;
		for (u32 i = 0; i < desc.MipLevels; ++i)
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
	m_imageFormat = desc.Format;
	m_colorFormat = ImageFormatToColorFormat(m_imageFormat);
	m_descriptorTypes = desc.Descriptors;

	if (m_name.empty() && !desc.Name.empty())
		m_name = desc.Name;

#if defined(ENABLE_GRAPHICS_DEBUG)
	if (!m_name.empty())
		SetTextureName(m_name);
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::SetTextureName(const std::string_view name) const
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	TRAP_ASSERT(!name.empty(), "VulkanTexture::SetTextureName(): Name is empty!");

	if (!VulkanRenderer::s_debugMarkerSupport)
		return;

#ifdef ENABLE_DEBUG_UTILS_EXTENSION
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkImage), VK_OBJECT_TYPE_IMAGE, name);
#else
	VkSetObjectName(m_device->GetVkDevice(), std::bit_cast<u64>(m_vkImage), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name);
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanTexture::Shutdown()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanTexturePrefix, "Destroying Texture");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	if (m_ownsImage && (m_vkImage != nullptr))
	{
		if (TRAP::Graphics::API::ImageFormatIsSinglePlane(m_imageFormat))
			vmaDestroyImage(m_vma->GetVMAAllocator(), m_vkImage, m_vkAllocation);
		else
		{
			vkDestroyImage(m_device->GetVkDevice(), m_vkImage, nullptr);
			vkFreeMemory(m_device->GetVkDevice(), m_vkDeviceMemory, nullptr);
		}
	}

	if (m_vkSRVDescriptor != nullptr)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkSRVDescriptor, nullptr);

	if (m_vkSRVStencilDescriptor != nullptr)
		vkDestroyImageView(m_device->GetVkDevice(), m_vkSRVStencilDescriptor, nullptr);

	if (!m_vkUAVDescriptors.empty())
	{
		for (u32 i = 0; i < m_mipLevels; ++i)
			vkDestroyImageView(m_device->GetVkDevice(), m_vkUAVDescriptors[i], nullptr);

		m_vkUAVDescriptors.clear();
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
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const auto *vkRenderer = dynamic_cast<TRAP::Graphics::API::VulkanRenderer *>(TRAP::Graphics::RendererAPI::GetRenderer());

	m_device = vkRenderer->GetDevice();
	m_vma = vkRenderer->GetVMA();

	TRAP_ASSERT(m_device, "VulkanTexture::PreInit(): Vulkan Device is nullptr");
	TRAP_ASSERT(m_vma, "VulkanTexture::PreInit(): VMA is nullptr");
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] u32 TRAP::Graphics::API::VulkanTexture::GetMemoryType(u32 typeBits,
														                 const VkPhysicalDeviceMemoryProperties &memProps,
														                 const VkMemoryPropertyFlags props, VkBool32* const memTypeFound)
{
	for (u32 i = 0; i < memProps.memoryTypeCount; i++)
	{
		if ((typeBits & 1u) == 1)
		{
			if ((memProps.memoryTypes[i].propertyFlags & props) == props)
			{
				if (memTypeFound != nullptr)
					*memTypeFound = VK_TRUE;

				return i;
			}
		}
		typeBits >>= 1u;
	}

	if (memTypeFound != nullptr)
	{
		*memTypeFound = VK_FALSE;
		return 0;
	}

	TRAP::Utils::DisplayError(TRAP::Utils::ErrorCode::VulkanNoMatchingMemoryTypeFound);
	std::unreachable();
}

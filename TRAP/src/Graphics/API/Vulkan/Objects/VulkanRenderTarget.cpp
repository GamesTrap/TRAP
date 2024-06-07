#include "TRAPPCH.h"
#include "VulkanRenderTarget.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Objects/VulkanTexture.h"

namespace
{
	constinit std::atomic<u64> CurrentRenderTargetID = 1;

	[[nodiscard]] TRAP::Ref<TRAP::Graphics::API::VulkanTexture> CreateRenderTargetTexture(const TRAP::Graphics::RendererAPI::RenderTargetDesc& desc,
	                                                                                      const TRAP::Graphics::API::VulkanPhysicalDevice& physicalDevice)
	{
		TRAP::Graphics::RendererAPI::TextureDesc textureDesc{};
		textureDesc.Flags = desc.Flags;
		textureDesc.Width = desc.Width;
		textureDesc.Height = desc.Height;
		textureDesc.Depth = desc.Depth;
		textureDesc.ArraySize = desc.ArraySize;
		textureDesc.MipLevels = TRAP::Math::Max(1U, desc.MipLevels);
		textureDesc.SampleCount = desc.SampleCount;
		textureDesc.SampleQuality = desc.SampleQuality;
		textureDesc.Format = desc.Format;
		textureDesc.ClearValue = desc.ClearValue;
		textureDesc.NativeHandle = desc.NativeHandle;
		textureDesc.Name = desc.Name;

		const bool isDepth = TRAP::Graphics::API::ImageFormatIsDepthOnly(desc.Format) ||
							 TRAP::Graphics::API::ImageFormatIsDepthAndStencil(desc.Format);

		TRAP_ASSERT(!((isDepth) && (desc.Descriptors & TRAP::Graphics::RendererAPI::DescriptorType::RWTexture) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined),
					"VulkanRenderTarget::CreateRenderTargetTexture(): Cannot use depth stencil as UAV");

		if (!isDepth)
			textureDesc.StartState |= TRAP::Graphics::RendererAPI::ResourceState::RenderTarget;
		else
			textureDesc.StartState |= TRAP::Graphics::RendererAPI::ResourceState::DepthWrite;

		if((desc.StartState & TRAP::Graphics::RendererAPI::ResourceState::ShadingRateSource) != TRAP::Graphics::RendererAPI::ResourceState::Undefined)
			textureDesc.StartState |= TRAP::Graphics::RendererAPI::ResourceState::ShadingRateSource;

		//Set this by default to be able to sample the renderTarget in shader
		textureDesc.Descriptors = desc.Descriptors;
		//Create SRV by default for a render target unless this is on tile texture
		//where SRV is not supported
		if((desc.Flags & TRAP::Graphics::RendererAPI::TextureCreationFlags::OnTile) == TRAP::Graphics::RendererAPI::TextureCreationFlags::None)
			textureDesc.Descriptors |= TRAP::Graphics::RendererAPI::DescriptorType::Texture;
		else
		{
			if((textureDesc.Descriptors & TRAP::Graphics::RendererAPI::DescriptorType::Texture) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined ||
			   (textureDesc.Descriptors & TRAP::Graphics::RendererAPI::DescriptorType::RWTexture) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined)
			{
				TP_WARN(TRAP::Log::RendererVulkanRenderTargetPrefix, "On tile textures do not support DescriptorType::Texture or DescriptorType::RWTexture");
			}

			//On tile textures do not support SRV/UAV as there is no backing memory
			//You can only read these textures as input attachments inside same render pass
			textureDesc.Descriptors &= TRAP::Graphics::RendererAPI::DescriptorType::Texture | TRAP::Graphics::RendererAPI::DescriptorType::RWTexture;
		}

		if(isDepth)
		{
			//Make sure depth/stencil format is supported - fall back to VK_FORMAT_D16_UNORM if not
			const VkFormat vkDepthStencilFormat = ImageFormatToVkFormat(desc.Format);
			if(VK_FORMAT_UNDEFINED != vkDepthStencilFormat &&
			   !physicalDevice.GetVkPhysicalDeviceImageFormatProperties(desc.Format, VK_IMAGE_TYPE_2D, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))
			{
				//Fall back to something that's guaranteed to work
				textureDesc.Format = TRAP::Graphics::API::ImageFormat::D16_UNORM;
				TP_WARN(TRAP::Log::RendererVulkanRenderTargetPrefix, "Depth stencil format (",
						std::to_underlying(desc.Format), ") is not supported. Falling back to D16 format");
			}
		}

		TRAP::Ref<TRAP::Graphics::API::VulkanTexture> rTTexture = TRAP::MakeRef<TRAP::Graphics::API::VulkanTexture>(fmt::format("RenderTarget {}", desc.Name));
		TRAP_ASSERT(rTTexture, "VulkanRenderTarget::CreateRenderTargetTexture(): Texture is nullptr!");
		rTTexture->Init(textureDesc);

		return rTTexture;
	}

	[[nodiscard]] VkImageViewCreateInfo GetImageViewCreateInfo(const TRAP::Graphics::API::VulkanTexture& texture)
	{
		const u32 depthOrArraySize = texture.GetArraySize() * texture.GetDepth();

		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
		if (texture.GetHeight() > 1)
			viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
		else
			viewType = depthOrArraySize > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;

		return TRAP::Graphics::API::VulkanInits::ImageViewCreateInfo(texture.GetVkImage(), viewType,
																	 ImageFormatToVkFormat(texture.GetImageFormat()), 1,
																	 depthOrArraySize);
	}

	[[nodiscard]] VkImageView CreateMainImageView(const TRAP::Graphics::API::VulkanTexture& texture, const TRAP::Graphics::API::VulkanDevice& device)
	{
		const VkImageViewCreateInfo rtvDesc = GetImageViewCreateInfo(texture);

		VkImageView imageView = VK_NULL_HANDLE;
		VkCall(vkCreateImageView(device.GetVkDevice(), &rtvDesc, nullptr, &imageView));
		TRAP_ASSERT(imageView, "VulkanRenderTarget::CreateMainImageView(): Vulkan Descriptor is nullptr!");

		return imageView;
	}

	[[nodiscard]] std::vector<VkImageView> CreateSliceDescriptors(const TRAP::Graphics::API::VulkanTexture& texture,
	                                                              const TRAP::Graphics::API::VulkanDevice& device)
	{
		const u32 depthOrArraySize = texture.GetArraySize() * texture.GetDepth();

		u32 numRTVs = texture.GetMipLevels();
		if(((texture.GetDescriptorTypes() & TRAP::Graphics::RendererAPI::DescriptorType::RenderTargetArraySlices) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined) ||
		   ((texture.GetDescriptorTypes() & TRAP::Graphics::RendererAPI::DescriptorType::RenderTargetDepthSlices) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined))
		{
			numRTVs *= depthOrArraySize;
		}

		std::vector<VkImageView> sliceDescriptors(numRTVs);

		VkImageViewCreateInfo rtvDesc = GetImageViewCreateInfo(texture);

		for(u32 i = 0; i < texture.GetMipLevels(); ++i)
		{
			rtvDesc.subresourceRange.baseMipLevel = i;
			if (((texture.GetDescriptorTypes() & TRAP::Graphics::RendererAPI::DescriptorType::RenderTargetArraySlices) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined) ||
				((texture.GetDescriptorTypes() & TRAP::Graphics::RendererAPI::DescriptorType::RenderTargetDepthSlices) != TRAP::Graphics::RendererAPI::DescriptorType::Undefined))
			{
				for (u32 j = 0; j < depthOrArraySize; ++j)
				{
					rtvDesc.subresourceRange.layerCount = 1;
					rtvDesc.subresourceRange.baseArrayLayer = j;
					VkCall(vkCreateImageView(device.GetVkDevice(), &rtvDesc, nullptr,
											&sliceDescriptors[i * depthOrArraySize + j]));
				}
			}
			else
				VkCall(vkCreateImageView(device.GetVkDevice(), &rtvDesc, nullptr, &sliceDescriptors[i]));
		}

		return sliceDescriptors;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderTarget::VulkanRenderTarget(const RendererAPI::RenderTargetDesc& desc)
	: RenderTarget(desc), m_ID(CurrentRenderTargetID++)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanRenderTarget(): Vulkan Device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Creating RenderTarget");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	const TRAP::Ref<TRAP::Graphics::API::VulkanTexture> vkTexture = CreateRenderTargetTexture(desc, m_device->GetPhysicalDevice());
	m_texture = vkTexture;

	m_vkDescriptor = CreateMainImageView(*vkTexture, *m_device);
	m_vkSliceDescriptors = CreateSliceDescriptors(*vkTexture, *m_device);

	//Unlike DirectX 12, Vulkan textures start in undefined layout.
	//To keep in line with DirectX 12, we transition them to the specified layout
	//manually so app code doesn't have to worry about this
	//Render targets wont be created during runtime so this overhead will be minimal
	VulkanRenderer::UtilInitialTransition(m_texture, desc.StartState);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanRenderTarget::~VulkanRenderTarget()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanRenderTargetPrefix, "Destroying RenderTarget");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroyImageView(m_device->GetVkDevice(), m_vkDescriptor, nullptr);

	for(const auto& imageView : m_vkSliceDescriptors)
	{
		if(imageView != VK_NULL_HANDLE)
			vkDestroyImageView(m_device->GetVkDevice(), imageView, nullptr);
	}
}

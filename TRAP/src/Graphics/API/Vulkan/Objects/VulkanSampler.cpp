#include "TRAPPCH.h"
#include "VulkanSampler.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

namespace
{
	void CheckFormatProperties(const TRAP::Graphics::RendererAPI::SamplerDesc& samplerDesc,
	                           const TRAP::Graphics::API::VulkanDevice& device)
	{
		TRAP_ASSERT(TRAP::Graphics::API::VulkanRenderer::s_samplerYcbcrConversionExtension,
		            "VulkanSampler::CheckFormatProperties(): Sampler YCbCr Conversion Extension is not supported by this device!");

		const VkFormatProperties formatProps = device.GetPhysicalDevice().GetVkPhysicalDeviceFormatProperties(samplerDesc.SamplerConversionDesc.Format);
		if(samplerDesc.SamplerConversionDesc.ChromaOffsetX == TRAP::Graphics::RendererAPI::SampleLocation::Midpoint)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT,
			            "VulkanSampler::CheckFormatProperties(): Format does not support Midpoint Chroma Sampling!");
		}
		else if(samplerDesc.SamplerConversionDesc.ChromaOffsetX == TRAP::Graphics::RendererAPI::SampleLocation::Cosited)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT,
			            "VulkanSampler::CheckFormatProperties(): Format does not support Cosited Chroma Sampling!");
		}
	}

	void CreatePlanarImageSampler(const TRAP::Graphics::RendererAPI::SamplerDesc& samplerDesc,
	                              const TRAP::Graphics::API::VulkanDevice& device,
								  VkSamplerCreateInfo& samplerCreateInfo,
			                      VkSampler& sampler, VkSamplerYcbcrConversion& samplerYcbcrConversion)
	{
		const auto& conversionDesc = samplerDesc.SamplerConversionDesc;
		const VkFormat format = ImageFormatToVkFormat(conversionDesc.Format);

		CheckFormatProperties(samplerDesc, device);

		VkSamplerYcbcrConversionCreateInfo conversionInfo = TRAP::Graphics::API::VulkanInits::SamplerYcbcrConversionCreateInfo(format,
		                                                                                                                       static_cast<VkSamplerYcbcrModelConversion>(conversionDesc.Model),
		                                                                                                                       static_cast<VkSamplerYcbcrRange>(conversionDesc.Range),
																															   static_cast<VkChromaLocation>(conversionDesc.ChromaOffsetX),
																															   static_cast<VkChromaLocation>(conversionDesc.ChromaOffsetY),
																															   TRAP::Graphics::API::FilterTypeToVkFilter(conversionDesc.ChromaFilter),
																															   conversionDesc.ForceExplicitReconstruction);
		VkCall(vkCreateSamplerYcbcrConversion(device.GetVkDevice(), &conversionInfo, nullptr,
											  &samplerYcbcrConversion));

		const auto vkSamplerYcbcrConversionInfo = TRAP::Graphics::API::VulkanInits::SamplerYcbcrConversionInfo(samplerYcbcrConversion);
		samplerCreateInfo.pNext = &vkSamplerYcbcrConversionInfo;
		VkCall(vkCreateSampler(device.GetVkDevice(), &samplerCreateInfo, nullptr, &sampler));
	}

	void Init(const TRAP::Graphics::RendererAPI::SamplerDesc& samplerDesc,
	          const TRAP::Graphics::API::VulkanDevice& device,
			  VkSampler& sampler, VkSamplerYcbcrConversion& samplerYcbcrConversion)
	{
#ifdef VERBOSE_GRAPHICS_DEBUG
		TP_DEBUG(TRAP::Log::RendererVulkanSamplerPrefix, "Creating Sampler");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

		//Default sampler lod values
		//Used if not overriden by SetLogRange or not Linear mipmaps
		f32 minSamplerLod = 0;
		f32 maxSampledLod = samplerDesc.MipMapMode == TRAP::Graphics::RendererAPI::MipMapMode::Linear ? VK_LOD_CLAMP_NONE : 0;

		//User provided lods
		if(samplerDesc.SetLodRange)
		{
			minSamplerLod = samplerDesc.MinLod;
			maxSampledLod = samplerDesc.MaxLod;
		}

		VkSamplerCreateInfo info = TRAP::Graphics::API::VulkanInits::SamplerCreateInfo(TRAP::Graphics::API::FilterTypeToVkFilter(samplerDesc.MagFilter),
																                       TRAP::Graphics::API::FilterTypeToVkFilter(samplerDesc.MinFilter),
																                       TRAP::Graphics::API::MipMapModeToVkMipMapMode(samplerDesc.MipMapMode),
																                       TRAP::Graphics::API::AddressModeToVkAddressMode(samplerDesc.AddressU),
																                       TRAP::Graphics::API::AddressModeToVkAddressMode(samplerDesc.AddressV),
																                       TRAP::Graphics::API::AddressModeToVkAddressMode(samplerDesc.AddressW),
																                       samplerDesc.MipLodBias,
																                       minSamplerLod,
																                       maxSampledLod,
																                       samplerDesc.EnableAnisotropy ? samplerDesc.OverrideAnisotropyLevel : 0.0f,
																                       TRAP::Graphics::API::VkComparisonFuncTranslator[std::to_underlying(samplerDesc.CompareFunc)]);

		if(!TRAP::Graphics::API::ImageFormatIsPlanar(samplerDesc.SamplerConversionDesc.Format))
		{
			VkCall(vkCreateSampler(device.GetVkDevice(), &info, nullptr, &sampler));
		}
		else //Planar image format
			CreatePlanarImageSampler(samplerDesc, device, info, sampler, samplerYcbcrConversion);

		TRAP_ASSERT(sampler, "VulkanSampler::Init(): Vulkan Sampler is nullptr!");

#ifdef ENABLE_GRAPHICS_DEBUG
	if(!samplerDesc.Name.empty())
		TRAP::Graphics::API::VkSetObjectName(device.GetVkDevice(), std::bit_cast<u64>(sampler), VK_OBJECT_TYPE_SAMPLER, samplerDesc.Name);
#endif /*ENABLE_GRAPHICS_DEBUG*/
	}

	void Shutdown(const TRAP::Graphics::API::VulkanDevice& device, VkSampler& sampler, VkSamplerYcbcrConversion& samplerYcbcrConversion)
	{
#ifdef VERBOSE_GRAPHICS_DEBUG
		TP_DEBUG(TRAP::Log::RendererVulkanSamplerPrefix, "Destroying Sampler");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

		vkDestroySampler(device.GetVkDevice(), sampler, nullptr);
		sampler = VK_NULL_HANDLE;

		if(samplerYcbcrConversion != VK_NULL_HANDLE)
		{
			vkDestroySamplerYcbcrConversion(device.GetVkDevice(), samplerYcbcrConversion, nullptr);
			samplerYcbcrConversion = VK_NULL_HANDLE;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSampler::VulkanSampler(const RendererAPI::SamplerDesc& desc)
	: Sampler(desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanSampler(): Vulkan Device is nullptr");

	Init(m_samplerDesc, *m_device, m_vkSampler, m_vkSamplerYcbcrConversion);
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSampler::~VulkanSampler()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	Shutdown(*m_device, m_vkSampler, m_vkSamplerYcbcrConversion);
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSampler::UpdateAnisotropy(const f32 anisotropy)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	m_samplerDesc.OverrideAnisotropyLevel = anisotropy;

	Shutdown(*m_device, m_vkSampler, m_vkSamplerYcbcrConversion);
	Init(m_samplerDesc, *m_device, m_vkSampler, m_vkSamplerYcbcrConversion);
}

#include "TRAPPCH.h"
#include "VulkanSampler.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanSampler::VulkanSampler(const RendererAPI::SamplerDesc& desc)
	: Sampler(desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	TRAP_ASSERT(m_device, "VulkanSampler(): Vulkan Device is nullptr");

	Init();
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSampler::~VulkanSampler()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	Shutdown();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSampler::UpdateAnisotropy(const f32 anisotropy)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	m_samplerDesc.OverrideAnisotropyLevel = anisotropy;

	Shutdown();
	Init();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSampler::Init()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Creating Sampler");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	//Default sampler lod values
	//Used if not overriden by SetLogRange or not Linear mipmaps
	f32 minSamplerLod = 0;
	f32 maxSampledLod = m_samplerDesc.MipMapMode == RendererAPI::MipMapMode::Linear ? VK_LOD_CLAMP_NONE : 0;

	//User provided lods
	if(m_samplerDesc.SetLodRange)
	{
		minSamplerLod = m_samplerDesc.MinLod;
		maxSampledLod = m_samplerDesc.MaxLod;
	}

	VkSamplerCreateInfo info = VulkanInits::SamplerCreateInfo(FilterTypeToVkFilter(m_samplerDesc.MagFilter),
		                                                      FilterTypeToVkFilter(m_samplerDesc.MinFilter),
		                                                      MipMapModeToVkMipMapMode(m_samplerDesc.MipMapMode),
		                                                      AddressModeToVkAddressMode(m_samplerDesc.AddressU),
		                                                      AddressModeToVkAddressMode(m_samplerDesc.AddressV),
		                                                      AddressModeToVkAddressMode(m_samplerDesc.AddressW),
		                                                      m_samplerDesc.MipLodBias,
															  minSamplerLod,
															  maxSampledLod,
		                                                      m_samplerDesc.EnableAnisotropy ? m_samplerDesc.OverrideAnisotropyLevel : 0.0f,
		                                                      VkComparisonFuncTranslator[std::to_underlying(m_samplerDesc.CompareFunc)]);

	if(!TRAP::Graphics::API::ImageFormatIsPlanar(m_samplerDesc.SamplerConversionDesc.Format))
	{
		VkCall(vkCreateSampler(m_device->GetVkDevice(), &info, nullptr, &m_vkSampler));
		return;
	}

	const auto& conversionDesc = m_samplerDesc.SamplerConversionDesc;
	const VkFormat format = ImageFormatToVkFormat(conversionDesc.Format);

	//Check format props
	{
		TRAP_ASSERT(VulkanRenderer::s_samplerYcbcrConversionExtension, "VulkanSampler(): Sampler YCbCr Conversion Extension is not supported by this device!");

		const VkFormatProperties formatProps = m_device->GetPhysicalDevice()->GetVkPhysicalDeviceFormatProperties(format);
		if(conversionDesc.ChromaOffsetX == RendererAPI::SampleLocation::Midpoint)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT, "VulkanSampler(): Format does not support Midpoint Chroma Sampling!");
		}
		else if(conversionDesc.ChromaOffsetX == RendererAPI::SampleLocation::Cosited)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT, "VulkanSampler(): Format does not support Cosited Chroma Sampling!");
		}
	}

	VkSamplerYcbcrConversionCreateInfo conversionInfo{};
	conversionInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
	conversionInfo.pNext = nullptr;
	conversionInfo.format = format;
	conversionInfo.ycbcrModel = static_cast<VkSamplerYcbcrModelConversion>(conversionDesc.Model);
	conversionInfo.ycbcrRange = static_cast<VkSamplerYcbcrRange>(conversionDesc.Range);
	conversionInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
									VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	conversionInfo.xChromaOffset = static_cast<VkChromaLocation>(conversionDesc.ChromaOffsetX);
	conversionInfo.yChromaOffset = static_cast<VkChromaLocation>(conversionDesc.ChromaOffsetY);
	conversionInfo.chromaFilter = FilterTypeToVkFilter(conversionDesc.ChromaFilter);
	conversionInfo.forceExplicitReconstruction = conversionDesc.ForceExplicitReconstruction ? VK_TRUE : VK_FALSE;
	VkCall(vkCreateSamplerYcbcrConversion(m_device->GetVkDevice(), &conversionInfo, nullptr,
											&m_vkSamplerYcbcrConversion));

	m_vkSamplerYcbcrConversionInfo = {};
	m_vkSamplerYcbcrConversionInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
	m_vkSamplerYcbcrConversionInfo.pNext = nullptr;
	m_vkSamplerYcbcrConversionInfo.conversion = m_vkSamplerYcbcrConversion;
	info.pNext = &m_vkSamplerYcbcrConversionInfo;

	VkCall(vkCreateSampler(m_device->GetVkDevice(), &info, nullptr, &m_vkSampler));
	TRAP_ASSERT(m_vkSampler, "VulkanSampler(): Vulkan Sampler is nullptr!");
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::API::VulkanSampler::Shutdown()
{
#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Destroying Sampler");
#endif /*VERBOSE_GRAPHICS_DEBUG*/

	vkDestroySampler(m_device->GetVkDevice(), m_vkSampler, nullptr);

	if(m_vkSamplerYcbcrConversion != VK_NULL_HANDLE)
		vkDestroySamplerYcbcrConversion(m_device->GetVkDevice(), m_vkSamplerYcbcrConversion, nullptr);
}
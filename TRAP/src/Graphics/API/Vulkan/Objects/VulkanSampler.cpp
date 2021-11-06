#include "TRAPPCH.h"
#include "VulkanSampler.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "VulkanPhysicalDevice.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

TRAP::Graphics::API::VulkanSampler::VulkanSampler(const RendererAPI::SamplerDesc& desc)
	: m_device(dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer().get())->GetDevice()),
	  m_vkSampler(VK_NULL_HANDLE),
	  m_vkSamplerYcbcrConversion(),
	  m_vkSamplerYcbcrConversionInfo()
{
	TRAP_ASSERT(m_device, "device is nullptr");

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Creating Sampler");
#endif

	m_samplerDesc = desc;

	if(m_samplerDesc.MaxAnisotropy > RendererAPI::GPUSettings.MaxAnisotropy)
	{
		TP_ERROR(Log::RendererVulkanSamplerPrefix, "Sampler Anisotropy is greater than the maximum supported by the GPU! Clamping to GPU maximum");
		m_samplerDesc.MaxAnisotropy = RendererAPI::GPUSettings.MaxAnisotropy;
	}

	VkSamplerCreateInfo info = VulkanInits::SamplerCreateInfo(FilterTypeToVkFilter(m_samplerDesc.MagFilter),
		                                                      FilterTypeToVkFilter(m_samplerDesc.MinFilter),
		                                                      MipMapModeToVkMipMapMode(m_samplerDesc.MipMapMode),
		                                                      AddressModeToVkAddressMode(m_samplerDesc.AddressU),
		                                                      AddressModeToVkAddressMode(m_samplerDesc.AddressV),
		                                                      AddressModeToVkAddressMode(m_samplerDesc.AddressW),
		                                                      m_samplerDesc.MipLodBias,
		                                                      m_samplerDesc.MaxAnisotropy,
		                                                      VkComparisonFuncTranslator[static_cast<uint32_t>(m_samplerDesc.CompareFunc)]);

	if(!TRAP::Graphics::API::ImageFormatIsPlanar(m_samplerDesc.SamplerConversionDesc.Format))
	{
		VkCall(vkCreateSampler(m_device->GetVkDevice(), &info, nullptr, &m_vkSampler));
		return;
	}

	auto& conversionDesc = m_samplerDesc.SamplerConversionDesc;
	const VkFormat format = ImageFormatToVkFormat(conversionDesc.Format);

	//Check format props
	{
		TRAP_ASSERT(VulkanRenderer::s_samplerYcbcrConversionExtension);

		VkFormatProperties formatProps{};
		vkGetPhysicalDeviceFormatProperties(m_device->GetPhysicalDevice()->GetVkPhysicalDevice(), format,
											&formatProps);
		if(conversionDesc.ChromaOffsetX == RendererAPI::SampleLocation::Midpoint)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT);
		}
		else if(conversionDesc.ChromaOffsetX == RendererAPI::SampleLocation::Cosited)
		{
			TRAP_ASSERT(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT);
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
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSampler::~VulkanSampler()
{
	TRAP_ASSERT(m_vkSampler);

#ifdef VERBOSE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Destroying Sampler");
#endif

	vkDestroySampler(m_device->GetVkDevice(), m_vkSampler, nullptr);

	if(m_vkSamplerYcbcrConversion != VK_NULL_HANDLE)
		vkDestroySamplerYcbcrConversion(m_device->GetVkDevice(), m_vkSamplerYcbcrConversion, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

VkSampler TRAP::Graphics::API::VulkanSampler::GetVkSampler() const
{
	return m_vkSampler;
}

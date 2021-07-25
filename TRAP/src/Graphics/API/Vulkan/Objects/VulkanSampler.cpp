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

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Creating Sampler");
#endif

	VkSamplerCreateInfo info = VulkanInits::SamplerCreateInfo(FilterTypeToVkFilter(desc.MagFilter),
		                                                      FilterTypeToVkFilter(desc.MinFilter),
		                                                      MipMapModeToVkMipMapMode(desc.MipMapMode),
		                                                      AddressModeToVkAddressMode(desc.AddressU),
		                                                      AddressModeToVkAddressMode(desc.AddressV),
		                                                      AddressModeToVkAddressMode(desc.AddressW),
		                                                      desc.MipLodBias,
		                                                      desc.MaxAnisotropy,
		                                                      VkComparisonFuncTranslator[static_cast<uint32_t>(desc.CompareFunc)]);

	if(!TRAP::Graphics::API::ImageFormatIsPlanar(desc.SamplerConversionDesc.Format))
	{
		VkCall(vkCreateSampler(m_device->GetVkDevice(), &info, nullptr, &m_vkSampler));
		return;
	}

	auto& conversionDesc = desc.SamplerConversionDesc;
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

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Destroying Sampler");
#endif

	vkDestroySampler(m_device->GetVkDevice(), m_vkSampler, nullptr);

	if(m_vkSamplerYcbcrConversion != VK_NULL_HANDLE)
		vkDestroySamplerYcbcrConversion(m_device->GetVkDevice(), m_vkSamplerYcbcrConversion, nullptr);
}

//-------------------------------------------------------------------------------------------------------------------//

VkSampler& TRAP::Graphics::API::VulkanSampler::GetVkSampler()
{
	return m_vkSampler;
}
#include "TRAPPCH.h"
#include "VulkanSampler.h"

#include "VulkanDevice.h"
#include "VulkanInits.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Graphics::API::VulkanSampler::VulkanSampler(TRAP::Ref<VulkanDevice> device, const RendererAPI::SamplerDesc& desc)
	: m_device(std::move(device)), m_vkSampler(VK_NULL_HANDLE)
{
	TRAP_ASSERT(device, "device is nullptr");

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Creating Sampler");
#endif

	const VkSamplerCreateInfo info = VulkanInits::SamplerCreateInfo(FilterTypeToVkFilter(desc.MagFilter),
		FilterTypeToVkFilter(desc.MinFilter),
		MipMapModeToVkMipMapMode(desc.MipMapMode),
		AddressModeToVkAddressMode(desc.AddressU),
		AddressModeToVkAddressMode(desc.AddressV),
		AddressModeToVkAddressMode(desc.AddressW),
		desc.MipLodBias,
		desc.MaxAnisotropy,
		VkComparisonFuncTranslator[static_cast<uint32_t>(desc.CompareFunc)]);

	VkCall(vkCreateSampler(m_device->GetVkDevice(), &info, nullptr, &m_vkSampler));
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::VulkanSampler::~VulkanSampler()
{
	if(m_vkSampler)
	{
#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererVulkanSamplerPrefix, "Destroying Sampler");
#endif

		vkDestroySampler(m_device->GetVkDevice(), m_vkSampler, nullptr);
	}
}

//-------------------------------------------------------------------------------------------------------------------//

VkSampler& TRAP::Graphics::API::VulkanSampler::GetVkSampler()
{
	return m_vkSampler;
}
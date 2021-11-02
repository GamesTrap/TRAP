#ifndef TRAP_VULKANSAMPLER_H
#define TRAP_VULKANSAMPLER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Sampler.h"

namespace TRAP::Graphics::API
{
	class VulkanDevice;

	class VulkanSampler final : public Sampler
	{
	public:
		explicit VulkanSampler(const RendererAPI::SamplerDesc& desc);
		~VulkanSampler() override;

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		VulkanSampler(const VulkanSampler&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		VulkanSampler& operator=(const VulkanSampler&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		VulkanSampler(VulkanSampler&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		VulkanSampler& operator=(VulkanSampler&&) = default;

		VkSampler& GetVkSampler();

	private:
		TRAP::Ref<VulkanDevice> m_device;

		//Native handle of the underlying resource
		VkSampler m_vkSampler;
		VkSamplerYcbcrConversion m_vkSamplerYcbcrConversion;
		VkSamplerYcbcrConversionInfo m_vkSamplerYcbcrConversionInfo;
	};
}

#endif /*TRAP_VULKANSAMPLER_H*/
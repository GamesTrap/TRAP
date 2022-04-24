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
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="desc">Sampler description.</param>
		explicit VulkanSampler(const RendererAPI::SamplerDesc& desc);
		/// <summary>
		/// Destructor.
		/// </summary>
		~VulkanSampler() override;

		/// <summary>
		/// Copy constructor.
		/// </summary>
		VulkanSampler(const VulkanSampler&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSampler& operator=(const VulkanSampler&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSampler(VulkanSampler&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSampler& operator=(VulkanSampler&&) = default;

		/// <summary>
		/// Retrieve the Vulkan sampler handle.
		/// </summary>
		/// <returns>Vulkan sampler handle.</returns>
		VkSampler GetVkSampler() const;

	private:
		TRAP::Ref<VulkanDevice> m_device;

		//Native handle of the underlying resource
		VkSampler m_vkSampler;
		VkSamplerYcbcrConversion m_vkSamplerYcbcrConversion;
		VkSamplerYcbcrConversionInfo m_vkSamplerYcbcrConversionInfo;
	};
}

#endif /*TRAP_VULKANSAMPLER_H*/
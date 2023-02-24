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
		VulkanSampler(const VulkanSampler&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		VulkanSampler& operator=(const VulkanSampler&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		VulkanSampler(VulkanSampler&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		VulkanSampler& operator=(VulkanSampler&&) noexcept = default;

		/// <summary>
		/// Retrieve the Vulkan sampler handle.
		/// </summary>
		/// <returns>Vulkan sampler handle.</returns>
		[[nodiscard]] VkSampler GetVkSampler() const noexcept;

	private:
		void UpdateAnisotropy(float anisotropy) override;

		void Init();
		void Shutdown();

		TRAP::Ref<VulkanDevice> m_device;

		//Native handle of the underlying resource
		VkSampler m_vkSampler;
		VkSamplerYcbcrConversion m_vkSamplerYcbcrConversion;
		VkSamplerYcbcrConversionInfo m_vkSamplerYcbcrConversionInfo;
	};
}

#endif /*TRAP_VULKANSAMPLER_H*/
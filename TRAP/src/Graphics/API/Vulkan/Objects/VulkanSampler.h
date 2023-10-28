#ifndef TRAP_VULKANSAMPLER_H
#define TRAP_VULKANSAMPLER_H

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Objects/Sampler.h"
#include "Graphics/API/Vulkan/VulkanRenderer.h"

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
		[[nodiscard]] constexpr VkSampler GetVkSampler() const noexcept;

	private:
		void UpdateAnisotropy(f32 anisotropy) override;

		void Init();
		void Shutdown();

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		//Native handle of the underlying resource
		VkSampler m_vkSampler = VK_NULL_HANDLE;
		VkSamplerYcbcrConversion m_vkSamplerYcbcrConversion{};
		VkSamplerYcbcrConversionInfo m_vkSamplerYcbcrConversionInfo{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSampler TRAP::Graphics::API::VulkanSampler::GetVkSampler() const noexcept
{
	return m_vkSampler;
}

#endif /*TRAP_VULKANSAMPLER_H*/
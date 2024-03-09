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
		/// @brief Constructor.
		/// @param desc Sampler description.
		explicit VulkanSampler(const RendererAPI::SamplerDesc& desc);
		/// @brief Destructor.
		~VulkanSampler() override;

		/// @brief Copy constructor.
		consteval VulkanSampler(const VulkanSampler&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval VulkanSampler& operator=(const VulkanSampler&) noexcept = delete;
		/// @brief Move constructor.
		VulkanSampler(VulkanSampler&&) noexcept = default;
		/// @brief Move assignment operator.
		VulkanSampler& operator=(VulkanSampler&&) noexcept = default;

		/// @brief Retrieve the Vulkan sampler handle.
		/// @return Vulkan sampler handle.
		[[nodiscard]] constexpr VkSampler GetVkSampler() const noexcept;

	private:
		void UpdateAnisotropy(f32 anisotropy) override;

		TRAP::Ref<VulkanDevice> m_device = dynamic_cast<VulkanRenderer*>(RendererAPI::GetRenderer())->GetDevice();

		//Native handle of the underlying resource
		VkSampler m_vkSampler = VK_NULL_HANDLE;
		VkSamplerYcbcrConversion m_vkSamplerYcbcrConversion{};
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr VkSampler TRAP::Graphics::API::VulkanSampler::GetVkSampler() const noexcept
{
	return m_vkSampler;
}

#endif /*TRAP_VULKANSAMPLER_H*/

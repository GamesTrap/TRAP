#include "TRAPPCH.h"
#include "SwapChain.h"

#ifndef TRAP_HEADLESS_MODE
#include "Graphics/API/Vulkan/Objects/VulkanSwapChain.h"
#endif /*TRAP_HEADLESS_MODE*/

#include "Graphics/API/Vulkan/VulkanCommon.h"

#ifndef TRAP_HEADLESS_MODE
[[nodiscard]] TRAP::Ref<TRAP::Graphics::SwapChain> TRAP::Graphics::SwapChain::Create(RendererAPI::SwapChainDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSwapChain>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "SwapChain::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ImageFormat TRAP::Graphics::SwapChain::GetRecommendedSwapchainFormat(const bool HDR,
                                                                                                        const bool SRGB) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return API::VulkanGetRecommendedSwapchainFormat(HDR, SRGB);

	case RenderAPI::NONE:
		[[fallthrough]];
	default:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

#ifndef TRAP_HEADLESS_MODE

[[nodiscard]] const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargets() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_renderTargets;
}

#endif /*TRAP_HEADLESS_MODE*/
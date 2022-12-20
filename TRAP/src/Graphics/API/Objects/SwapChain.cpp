#include "TRAPPCH.h"
#include "SwapChain.h"

#include "Graphics/API/Vulkan/Objects/VulkanSwapChain.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Ref<TRAP::Graphics::SwapChain> TRAP::Graphics::SwapChain::Create(RendererAPI::SwapChainDesc& desc)
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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::SwapChain::GetRecommendedSwapchainFormat(const bool HDR,
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

TRAP::Graphics::SwapChain::SwapChain()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Creating SwapChain");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SwapChain::~SwapChain()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Destroying SwapChain");
#endif

	m_renderTargets.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargets() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_renderTargets;
}

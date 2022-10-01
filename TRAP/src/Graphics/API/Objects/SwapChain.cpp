#include "TRAPPCH.h"
#include "SwapChain.h"

#include "Graphics/API/Vulkan/Objects/VulkanSwapChain.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Ref<TRAP::Graphics::SwapChain> TRAP::Graphics::SwapChain::Create(RendererAPI::SwapChainDesc& desc)
{
	ZoneScopedC(tracy::Color::Red);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSwapChain>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::API::ImageFormat TRAP::Graphics::SwapChain::GetRecommendedSwapchainFormat(const bool HDR,
                                                                                          const bool SRGB)
{
	ZoneScopedC(tracy::Color::Red);

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
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Creating SwapChain");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SwapChain::~SwapChain()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Destroying SwapChain");
#endif

	m_renderTargetsMSAA.clear();
	m_renderTargets.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargets() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_renderTargets;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargetsMSAA() const
{
	ZoneScopedC(tracy::Color::Red);

	return m_renderTargetsMSAA;
}

#include "TRAPPCH.h"
#include "SwapChain.h"

#include "Graphics/API/Vulkan/Objects/VulkanSwapChain.h"
#include "Graphics/API/Vulkan/VulkanCommon.h"

TRAP::Ref<TRAP::Graphics::SwapChain> TRAP::Graphics::SwapChain::Create(RendererAPI::SwapChainDesc& desc)
{
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
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return API::VulkanGetRecommendedSwapchainFormat(HDR, SRGB);

	case RenderAPI::NONE:
	default:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SwapChain::SwapChain()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Creating SwapChain");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SwapChain::~SwapChain()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Destroying SwapChain");
#endif

	m_renderTargetsMSAAResolve.clear();
	m_renderTargets.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargets() const
{
	//If MSAA is enabled then we need to return the MSAA render targets
	if(!m_renderTargetsMSAAResolve.empty())
		return m_renderTargetsMSAAResolve;

	return m_renderTargets;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargetsMSAAResolve() const
{
	return m_renderTargetsMSAAResolve;
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargetsNonMSAA() const
{
	return m_renderTargets;
}

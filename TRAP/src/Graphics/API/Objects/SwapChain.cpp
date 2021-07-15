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

TRAP::Graphics::API::ImageFormat TRAP::Graphics::SwapChain::GetRecommendedSwapchainFormat(const bool HDR)
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return API::VulkanGetRecommendedSwapchainFormat(HDR);

	case RenderAPI::NONE:
	default:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SwapChain::~SwapChain()
{
	m_renderTargets.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

const std::vector<TRAP::Ref<TRAP::Graphics::RenderTarget>>& TRAP::Graphics::SwapChain::GetRenderTargets() const
{
	return m_renderTargets;
}
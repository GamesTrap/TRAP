#include "TRAPPCH.h"
#include "SwapChain.h"

#ifndef TRAP_HEADLESS_MODE
#include "Graphics/API/Vulkan/Objects/VulkanSwapChain.h"
#endif /*TRAP_HEADLESS_MODE*/

#include "Graphics/API/Vulkan/VulkanCommon.h"

#ifndef TRAP_HEADLESS_MODE

[[nodiscard]] TRAP::Ref<TRAP::Graphics::SwapChain> TRAP::Graphics::SwapChain::Create(RendererAPI::SwapChainDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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

TRAP::Graphics::SwapChain::SwapChain()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Creating SwapChain");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::SwapChain::~SwapChain()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSwapChainPrefix, "Destroying SwapChain");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

#endif /*TRAP_HEADLESS_MODE*/

//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ImageFormat TRAP::Graphics::GetRecommendedSwapchainFormat(const bool HDR,
                                                                                             const bool SRGB) noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return API::VulkanGetRecommendedSwapchainFormat(HDR, SRGB);

	case RenderAPI::NONE:
		return TRAP::Graphics::API::ImageFormat::Undefined;
	}

	TRAP_ASSERT(false, "GetRecommendedSwapchainFormat(): Unknown RenderAPI!");
	return TRAP::Graphics::API::ImageFormat::Undefined;
}

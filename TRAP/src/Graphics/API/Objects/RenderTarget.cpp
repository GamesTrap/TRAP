#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"
#include "Graphics/API/RendererAPI.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::~RenderTarget()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Destroying RenderTarget");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RenderTarget> TRAP::Graphics::RenderTarget::Create(const RenderTargetDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanRenderTarget>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "RenderTarget::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::RenderTarget::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_texture;
}

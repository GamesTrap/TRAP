#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::RenderTarget()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Creating RenderTarget");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::~RenderTarget()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Destroying RenderTarget");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RenderTarget> TRAP::Graphics::RenderTarget::Create(const RendererAPI::RenderTargetDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

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
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_texture;
}
#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::RenderTarget(const RendererAPI::RenderTargetDesc& desc)
	: m_clearValue(desc.ClearValue), m_arraySize(desc.ArraySize), m_depth(desc.Depth), m_width(desc.Width),
	  m_height(desc.Height), m_descriptors(desc.Descriptors),
	  m_mipLevels(TRAP::Math::Max(1U, desc.MipLevels)), m_sampleQuality(desc.SampleQuality), m_format(desc.Format),
	  m_sampleCount(desc.SampleCount)
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
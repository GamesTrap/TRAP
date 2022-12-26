#include "TRAPPCH.h"
#include "RenderTarget.h"

#include "Graphics/Textures/Texture.h"
#include "Graphics/API/Vulkan/Objects/VulkanRenderTarget.h"

TRAP::Graphics::RenderTarget::RenderTarget()
	: m_texture(nullptr), m_clearValue(), m_arraySize(),
	  m_depth(), m_width(), m_height(), m_descriptors(), m_mipLevels(), m_sampleQuality(), m_format(),
	  m_sampleCount()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Creating RenderTarget");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RenderTarget::~RenderTarget()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRenderTargetPrefix, "Destroying RenderTarget");
#endif
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

[[nodiscard]] uint32_t TRAP::Graphics::RenderTarget::GetWidth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RenderTarget::GetHeight() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RenderTarget::GetDepth() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RenderTarget::GetArraySize() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RenderTarget::GetMipLevels() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RenderTarget::GetSampleCount() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] uint32_t TRAP::Graphics::RenderTarget::GetSampleQuality() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::API::ImageFormat TRAP::Graphics::RenderTarget::GetImageFormat() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::ClearValue TRAP::Graphics::RenderTarget::GetClearValue() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_clearValue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::RenderTarget::GetDescriptorType() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_descriptors;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Texture> TRAP::Graphics::RenderTarget::GetTexture() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_texture;
}
#include "TRAPPCH.h"
#include "CommandSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanCommandSignature.h"

TRAP::Graphics::CommandSignature::CommandSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandSignaturePrefix, "Creating CommandSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandSignature::~CommandSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandSignaturePrefix, "Destroying CommandSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::CommandSignature> TRAP::Graphics::CommandSignature::Create(const RendererAPI::CommandSignatureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanCommandSignature>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "CommandSignature::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
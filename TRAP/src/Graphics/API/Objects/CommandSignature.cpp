#include "TRAPPCH.h"
#include "CommandSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanCommandSignature.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::CommandSignature> TRAP::Graphics::CommandSignature::Create(const RendererAPI::CommandSignatureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandSignature::CommandSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandSignaturePrefix, "Creating CommandSignature");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CommandSignature::~CommandSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererCommandSignaturePrefix, "Destroying CommandSignature");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

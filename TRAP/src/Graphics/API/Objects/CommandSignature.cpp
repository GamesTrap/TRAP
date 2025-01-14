#include "TRAPPCH.h"
#include "CommandSignature.h"

#include "Graphics/API/RendererAPI.h"
#include "Graphics/API/Vulkan/Objects/VulkanCommandSignature.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::CommandSignature> TRAP::Graphics::CommandSignature::Create(const CommandSignatureDesc& desc)
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

void TRAP::Graphics::CommandSignature::DebugLog(const std::string_view msg)
{
	TP_DEBUG(TRAP::Log::RendererCommandSignaturePrefix, msg);
}

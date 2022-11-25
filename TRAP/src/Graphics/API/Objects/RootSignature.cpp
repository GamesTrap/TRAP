#include "TRAPPCH.h"
#include "RootSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"

TRAP::Graphics::RootSignature::RootSignature()
	: m_pipelineType(), m_descriptorNameToIndexMap()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Creating RootSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RootSignature::~RootSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Destroying RootSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RootSignature::Create(const RendererAPI::RootSignatureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanRootSignature>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "RootSignature::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
#include "TRAPPCH.h"
#include "RootSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"

TRAP::Graphics::RootSignature::RootSignature()
	: m_pipelineType()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Creating RootSignature");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RootSignature::~RootSignature()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Destroying RootSignature");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] const TRAP::Graphics::RendererAPI::DescriptorIndexMap& TRAP::Graphics::RootSignature::GetDescriptorNameToIndexMap() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	return m_descriptorNameToIndexMap;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RootSignature::Create(const RendererAPI::RootSignatureDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

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
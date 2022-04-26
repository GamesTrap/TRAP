#include "TRAPPCH.h"
#include "RootSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"

TRAP::Graphics::RootSignature::RootSignature()
	: m_pipelineType(), m_descriptorNameToIndexMap()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Creating RootSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RootSignature::~RootSignature()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererRootSignaturePrefix, "Destroying RootSignature");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::RootSignature> TRAP::Graphics::RootSignature::Create(const RendererAPI::RootSignatureDesc& desc)
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanRootSignature>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}
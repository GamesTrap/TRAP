#include "TRAPPCH.h"
#include "RootSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanRootSignature.h"

TRAP::Graphics::RootSignature::RootSignature()
	: m_pipelineType(), m_descriptorNameToIndexMap(nullptr)
{}

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
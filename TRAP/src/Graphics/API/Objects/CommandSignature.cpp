#include "TRAPPCH.h"
#include "CommandSignature.h"

#include "Graphics/API/Vulkan/Objects/VulkanCommandSignature.h"

TRAP::Ref<TRAP::Graphics::CommandSignature> TRAP::Graphics::CommandSignature::Create(const RendererAPI::CommandSignatureDesc& desc)
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanCommandSignature>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}
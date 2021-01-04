#include "TRAPPCH.h"
#include "Sampler.h"

#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"

TRAP::Ref<TRAP::Graphics::Sampler> TRAP::Graphics::Sampler::Create(const RendererAPI::SamplerDesc& desc)
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanSampler>(desc);
		
	case RenderAPI::NONE:
		return nullptr;
		
	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}
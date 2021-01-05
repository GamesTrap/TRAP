#include "TRAPPCH.h"
#include "Pipeline.h"

#include "Graphics/API/Vulkan/Objects/VulkanPipeline.h"

TRAP::Ref<TRAP::Graphics::Pipeline> TRAP::Graphics::Pipeline::Create(const RendererAPI::PipelineDesc& desc)
{
	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanPipeline>(desc);
		
	case RenderAPI::NONE:
		return nullptr;
		
	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}
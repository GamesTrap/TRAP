#include "TRAPPCH.h"
#include "Pipeline.h"

#include "Graphics/API/Vulkan/Objects/VulkanPipeline.h"

TRAP::Graphics::Pipeline::Pipeline()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererPipelinePrefix, "Creating Pipeline");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Pipeline::~Pipeline()
{
	ZoneScopedC(tracy::Color::Red);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererPipelinePrefix, "Destroying Pipeline");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Pipeline> TRAP::Graphics::Pipeline::Create(const RendererAPI::PipelineDesc& desc)
{
	ZoneScopedC(tracy::Color::Red);

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
#include "TRAPPCH.h"
#include "Pipeline.h"

#include "Graphics/API/Vulkan/Objects/VulkanPipeline.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Pipeline> TRAP::Graphics::Pipeline::Create(const RendererAPI::PipelineDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanPipeline>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Pipeline::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Pipeline::~Pipeline()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererPipelinePrefix, "Destroying Pipeline");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Pipeline::Pipeline()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererPipelinePrefix, "Creating Pipeline");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

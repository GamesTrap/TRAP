#include "TRAPPCH.h"
#include "Pipeline.h"

#include "Graphics/API/Vulkan/Objects/VulkanPipeline.h"
#include "Graphics/API/RendererAPI.h"

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Pipeline> TRAP::Graphics::Pipeline::Create(const PipelineDesc& desc)
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

void TRAP::Graphics::Pipeline::DebugLog(const std::string_view msg)
{
	TP_DEBUG(TRAP::Log::RendererPipelinePrefix, msg);
}

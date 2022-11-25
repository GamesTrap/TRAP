#include "TRAPPCH.h"
#include "PipelineCache.h"

#include "Graphics/API/Vulkan/Objects/VulkanPipelineCache.h"
#include "FileSystem/FileSystem.h"

TRAP::Graphics::PipelineCache::PipelineCache()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererPipelineCachePrefix, "Creating PipelineCache");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::PipelineCache::~PipelineCache()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererPipelineCachePrefix, "Destroying PipelineCache");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::PipelineCache> TRAP::Graphics::PipelineCache::Create(const RendererAPI::PipelineCacheDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
		return TRAP::MakeRef<API::VulkanPipelineCache>(desc);

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "PipelineCache::Create(): Unknown RenderAPI");
		return nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::PipelineCache> TRAP::Graphics::PipelineCache::Create(const RendererAPI::PipelineCacheLoadDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Graphics::RendererAPI::PipelineCacheDesc cacheDesc{};
		cacheDesc.Flags = desc.Flags;
		if(!std::filesystem::exists(desc.Path))
			return TRAP::Graphics::PipelineCache::Create(cacheDesc); //Empty cache

		const auto data = TRAP::FileSystem::ReadFile(desc.Path);
		if(!data)
			return TRAP::Graphics::PipelineCache::Create(cacheDesc); //Empty cache

		cacheDesc.Data = *data;

		return TRAP::Graphics::PipelineCache::Create(cacheDesc); //Cache with data
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "PipelineCache::Create(): Unknown RenderAPI");
		return nullptr;
	}
}
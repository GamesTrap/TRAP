#include "TRAPPCH.h"
#include "Sampler.h"

#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"

std::unordered_map<TRAP::Graphics::RendererAPI::SamplerDesc, TRAP::Ref<TRAP::Graphics::Sampler>> TRAP::Graphics::Sampler::s_cachedSamplers;

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Sampler> TRAP::Graphics::Sampler::Create(RendererAPI::SamplerDesc desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	if(desc.EnableAnisotropy && desc.OverrideAnisotropyLevel > RendererAPI::GPUSettings.MaxAnisotropy)
	{
		TP_ERROR(Log::RendererSamplerPrefix, "Sampler Anisotropy is greater than the maximum supported by the GPU! Clamping to GPU maximum");
		desc.OverrideAnisotropyLevel = RendererAPI::GPUSettings.MaxAnisotropy;
	}

	//Inject engine set Anisotropy level
	bool injectedAnisotropy = false;
	if(desc.EnableAnisotropy && desc.OverrideAnisotropyLevel == 0.0f)
	{
		desc.OverrideAnisotropyLevel = NumericCast<f32>(std::to_underlying(RendererAPI::GetAnisotropyLevel()));
		injectedAnisotropy = true;
	}

	//Try to use cached Sampler
	if(s_cachedSamplers.contains(desc))
		return s_cachedSamplers[desc];

	TRAP_ASSERT(s_cachedSamplers.size() != RendererAPI::GPUSettings.MaxSamplerAllocationCount, "Sampler::Create(): Exceeded max simultaneously allowed samplers!");
	if(s_cachedSamplers.size() == RendererAPI::GPUSettings.MaxSamplerAllocationCount)
	{
		TP_ERROR(Log::RendererSamplerPrefix, "Exceeding max simultaneously allowed samplers! Returning first cached sampler");
		return s_cachedSamplers.begin()->second;
	}

	TRAP::Ref<Sampler> result = nullptr;

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		result = TRAP::MakeRef<API::VulkanSampler>(desc);
		break;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Sampler::Create(): Unknown RenderAPI");
		return nullptr;
	}

	result->m_usesEngineAnisotropyLevel = injectedAnisotropy;

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSamplerPrefix, "Caching Sampler");
#endif /*ENABLE_GRAPHICS_DEBUG*/
	s_cachedSamplers[desc] = result;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Sampler::~Sampler()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSamplerPrefix, "Destroying Sampler");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Sampler::Sampler(const RendererAPI::SamplerDesc& desc)
	: m_samplerDesc(desc)
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSamplerPrefix, "Creating Sampler");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Sampler::ClearCache() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Graphics) != ProfileSystems::None &&
	                                       (GetTRAPProfileSystems() & ProfileSystems::Verbose) != ProfileSystems::None);

	s_cachedSamplers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Sampler::UpdateSamplers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (GetTRAPProfileSystems() & ProfileSystems::Vulkan) != ProfileSystems::None);

	const f32 usedSamples = NumericCast<f32>(std::to_underlying(RenderCommand::GetAnisotropyLevel()));

	std::vector<Ref<Sampler>> samplersToUpdate{};
	for(const auto& [desc, sampler] : s_cachedSamplers)
	{
		//Ignore Samplers that dont use anisotropic filtering, or already use the engine anisotropy level
		if(!desc.EnableAnisotropy || !sampler->UsesEngineAnisotropyLevel() || sampler->GetAnisotropyLevel() == usedSamples)
			continue;

		samplersToUpdate.emplace_back(sampler);
	}

	if(samplersToUpdate.empty()) //Early exit
		return;

	//Wait for all GPU to finish processing
	RendererAPI::GetRenderer()->WaitIdle();

	for(const auto& sampler : samplersToUpdate)
	{
		s_cachedSamplers.erase(sampler->m_samplerDesc);

		sampler->UpdateAnisotropy(usedSamples);

		s_cachedSamplers[sampler->m_samplerDesc] = sampler;
	}
}

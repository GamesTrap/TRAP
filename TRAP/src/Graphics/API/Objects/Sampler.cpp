#include "TRAPPCH.h"
#include "Sampler.h"

#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"
#include "Utils/Utils.h"

std::unordered_map<TRAP::Graphics::RendererAPI::SamplerDesc, TRAP::Ref<TRAP::Graphics::Sampler>> TRAP::Graphics::Sampler::s_cachedSamplers;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Sampler::Sampler()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSamplerPrefix, "Creating Sampler");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::Sampler::~Sampler()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics);

#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSamplerPrefix, "Destroying Sampler");
#endif
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Ref<TRAP::Graphics::Sampler> TRAP::Graphics::Sampler::Create(RendererAPI::SamplerDesc desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(desc.EnableAnisotropy && desc.OverrideAnisotropyLevel > RendererAPI::GPUSettings.MaxAnisotropy)
	{
		TP_ERROR(Log::RendererSamplerPrefix, "Sampler Anisotropy is greater than the maximum supported by the GPU! Clamping to GPU maximum");
		desc.OverrideAnisotropyLevel = RendererAPI::GPUSettings.MaxAnisotropy;
	}

	//Inject engine set Anisotropy level
	bool injectedAnisotropy = false;
	if(desc.EnableAnisotropy && desc.OverrideAnisotropyLevel == 0.0f)
	{
		desc.OverrideAnisotropyLevel = static_cast<float>(RendererAPI::GetAnisotropyLevel());
		injectedAnisotropy = true;
	}

	//Try to use cached Sampler
	if(s_cachedSamplers.find(desc) != s_cachedSamplers.end())
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
#endif
	s_cachedSamplers[desc] = result;

	return result;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::FilterType TRAP::Graphics::Sampler::GetMinFilter() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MinFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::FilterType TRAP::Graphics::Sampler::GetMagFilter() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MagFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::MipMapMode TRAP::Graphics::Sampler::GetMipMapMode() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MipMapMode;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressU() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.AddressU;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressV() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.AddressV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressW() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.AddressW;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::Sampler::GetMipLodBias() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MipLodBias;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] float TRAP::Graphics::Sampler::GetAnisotropyLevel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	if(!m_samplerDesc.EnableAnisotropy)
		return 0.0f;

	return m_samplerDesc.OverrideAnisotropyLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] TRAP::Graphics::CompareMode TRAP::Graphics::Sampler::GetCompareFunc() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.CompareFunc;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] bool TRAP::Graphics::Sampler::UsesEngineAnisotropyLevel() const noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_usesEngineAnisotropyLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Sampler::ClearCache() noexcept
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_cachedSamplers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Sampler::UpdateSamplers()
{
	ZoneNamedC(__tracy, tracy::Color::Red, TRAP_PROFILE_SYSTEMS() & ProfileSystems::Vulkan);

	const float usedSamples = static_cast<float>(RenderCommand::GetAnisotropyLevel());

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

//-------------------------------------------------------------------------------------------------------------------//

std::size_t std::hash<TRAP::Graphics::RendererAPI::SamplerDesc>::operator()(const TRAP::Graphics::RendererAPI::SamplerDesc& desc) const noexcept
{
	std::size_t res = 0;

	TRAP::Utils::HashCombine
	(
		res,
		desc.MinFilter,
		desc.MagFilter,
		desc.MipMapMode,
		desc.AddressU,
		desc.AddressV,
		desc.AddressW,
		desc.MipLodBias,
		desc.EnableAnisotropy,
		desc.OverrideAnisotropyLevel,
		desc.CompareFunc
	);

	TRAP::Utils::HashCombine
	(
		res,
		desc.SamplerConversionDesc.Format,
		desc.SamplerConversionDesc.Model,
		desc.SamplerConversionDesc.Range,
		desc.SamplerConversionDesc.ChromaOffsetX,
		desc.SamplerConversionDesc.ChromaOffsetY,
		desc.SamplerConversionDesc.ChromaFilter,
		desc.SamplerConversionDesc.ForceExplicitReconstruction
	);

	return res;
}
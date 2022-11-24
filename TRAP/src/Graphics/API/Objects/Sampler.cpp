#include "TRAPPCH.h"
#include "Sampler.h"

#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"
#include "Utils/Utils.h"

std::unordered_map<TRAP::Graphics::SamplerDesc, TRAP::Ref<TRAP::Graphics::Sampler>> TRAP::Graphics::Sampler::s_cachedSamplers;

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

TRAP::Ref<TRAP::Graphics::Sampler> TRAP::Graphics::Sampler::Create(const SamplerDesc& desc)
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	//Try to use cached Sampler
	if(s_cachedSamplers.find(desc) != s_cachedSamplers.end())
		return s_cachedSamplers[desc];

	if(s_cachedSamplers.size() == RendererAPI::GPUSettings.MaxSamplerAllocationCount)
	{
		TP_ERROR(Log::RendererSamplerPrefix, "Exceeding max simultaneously allowed samplers! Returning first cached sampler");
		return s_cachedSamplers.begin()->second;
	}

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		const TRAP::Ref<Sampler> result = TRAP::MakeRef<API::VulkanSampler>(desc);

#ifdef ENABLE_GRAPHICS_DEBUG
		TP_DEBUG(Log::RendererSamplerPrefix, "Caching Sampler");
#endif
		s_cachedSamplers[desc] = result;

		return result;
	}

	case RenderAPI::NONE:
		return nullptr;

	default:
		TRAP_ASSERT(false, "Unknown RenderAPI");
		return nullptr;
	}
}


//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::FilterType TRAP::Graphics::Sampler::GetMinFilter() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MinFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::FilterType TRAP::Graphics::Sampler::GetMagFilter() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MagFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::MipMapMode TRAP::Graphics::Sampler::GetMipMapMode() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MipMapMode;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressU() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.AddressU;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressV() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.AddressV;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressW() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.AddressW;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Sampler::GetMipLodBias() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MipLodBias;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Sampler::GetMaxAnisotropy() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.MaxAnisotropy;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::CompareMode TRAP::Graphics::Sampler::GetCompareFunc() const
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	return m_samplerDesc.CompareFunc;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Sampler::ClearCache()
{
	ZoneNamedC(__tracy, tracy::Color::Red, (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Graphics) && (TRAP_PROFILE_SYSTEMS() & ProfileSystems::Verbose));

	s_cachedSamplers.clear();
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
		desc.MaxAnisotropy,
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
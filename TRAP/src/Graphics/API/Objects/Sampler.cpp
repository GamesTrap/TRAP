#include "TRAPPCH.h"
#include "Sampler.h"

#include "Graphics/API/Vulkan/Objects/VulkanSampler.h"
#include "Utils/Utils.h"

std::unordered_map<TRAP::Graphics::RendererAPI::SamplerDesc, TRAP::Ref<TRAP::Graphics::Sampler>, TRAP::Graphics::SamplerHash> TRAP::Graphics::Sampler::s_cachedSamplers;

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Ref<TRAP::Graphics::Sampler> TRAP::Graphics::Sampler::Create(const RendererAPI::SamplerDesc& desc)
{
	//Try to use cached Sampler
	if(s_cachedSamplers.find(desc) != s_cachedSamplers.end())
		return s_cachedSamplers[desc];

	switch(RendererAPI::GetRenderAPI())
	{
	case RenderAPI::Vulkan:
	{
		TRAP::Ref<Sampler> result = TRAP::MakeRef<API::VulkanSampler>(desc);

		result->m_samplerDesc = desc;

		TP_DEBUG(Log::SamplerPrefix, "Caching Sampler");
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

TRAP::Graphics::RendererAPI::FilterType TRAP::Graphics::Sampler::GetMinFilter() const
{
	return m_samplerDesc.MinFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::FilterType TRAP::Graphics::Sampler::GetMagFilter() const
{
	return m_samplerDesc.MagFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::MipMapMode TRAP::Graphics::Sampler::GetMipMapMode() const
{
	return m_samplerDesc.MipMapMode;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::AddressMode TRAP::Graphics::Sampler::GetAddressU() const
{
	return m_samplerDesc.AddressU;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::AddressMode TRAP::Graphics::Sampler::GetAddressV() const
{
	return m_samplerDesc.AddressV;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::AddressMode TRAP::Graphics::Sampler::GetAddressW() const
{
	return m_samplerDesc.AddressW;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Sampler::GetMipLodBias() const
{
	return m_samplerDesc.MipLodBias;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Sampler::GetMaxAnisotropy() const
{
	return m_samplerDesc.MaxAnisotropy;
}

//-------------------------------------------------------------------------------------------------------------------//

TRAP::Graphics::RendererAPI::CompareMode TRAP::Graphics::Sampler::GetCompareFunc() const
{
	return m_samplerDesc.CompareFunc;
}

//-------------------------------------------------------------------------------------------------------------------//

bool TRAP::Graphics::Sampler::GetForceMipLevel() const
{
	return m_samplerDesc.ForceMipLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

float TRAP::Graphics::Sampler::GetMipLevel() const
{
	if(GetForceMipLevel())
		return m_samplerDesc.MipLevel;

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------//

void TRAP::Graphics::Sampler::ClearCache()
{
	s_cachedSamplers.clear();
}

//-------------------------------------------------------------------------------------------------------------------//

std::size_t TRAP::Graphics::SamplerHash::operator()(const RendererAPI::SamplerDesc& desc) const noexcept
{
	std::size_t hash = 0;

	TRAP::Utils::HashCombine(hash,
		desc.MinFilter,
		desc.MagFilter,
		desc.MipMapMode,
		desc.AddressU,
		desc.AddressV,
		desc.AddressW,
		desc.MipLodBias,
		desc.MaxAnisotropy,
		desc.CompareFunc,

		desc.ForceMipLevel,
		desc.MipLevel
	);

	TRAP::Utils::HashCombine(hash,
		desc.SamplerConversionDesc.Format,
		desc.SamplerConversionDesc.Model,
		desc.SamplerConversionDesc.Range,
		desc.SamplerConversionDesc.ChromaOffsetX,
		desc.SamplerConversionDesc.ChromaOffsetY,
		desc.SamplerConversionDesc.ChromaFilter,
		desc.SamplerConversionDesc.ForceExplicitReconstruction
	);

	return hash;
}
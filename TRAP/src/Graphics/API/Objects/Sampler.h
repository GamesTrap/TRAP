#ifndef _TRAP_SAMPLER_H_
#define _TRAP_SAMPLER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Sampler
	{
	public:
		static TRAP::Ref<Sampler> Create(const RendererAPI::SamplerDesc& desc);

		virtual ~Sampler() = default;

		RendererAPI::FilterType GetMinFilter() const;
		RendererAPI::FilterType GetMagFilter() const;
		RendererAPI::MipMapMode GetMipMapMode() const;
		RendererAPI::AddressMode GetAddressU() const;
		RendererAPI::AddressMode GetAddressV() const;
		RendererAPI::AddressMode GetAddressW() const;
		float GetMipLodBias() const;
		float GetMaxAnisotropy() const;
		RendererAPI::CompareMode GetCompareFunc() const;

		//Whether sampler is forced to use a specific mip level or not
		bool GetForceMipLevel() const;
		//Returns the forced mip level, if mip level is not forced returns 0.0f
		float GetMipLevel() const;

		static void ClearCache();

	protected:
		Sampler() = default;

	private:
		static std::unordered_map<RendererAPI::SamplerDesc, TRAP::Ref<Sampler>> s_cachedSamplers;

		RendererAPI::SamplerDesc m_samplerDesc;
	};
}

namespace std
{
	template <>
 	struct hash<TRAP::Graphics::RendererAPI::SamplerDesc>
	{
		std::size_t operator()(const TRAP::Graphics::RendererAPI::SamplerDesc& desc) const noexcept;
	};
}

#endif /*_TRAP_SAMPLER_H_*/
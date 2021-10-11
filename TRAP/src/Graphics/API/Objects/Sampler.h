#ifndef _TRAP_SAMPLER_H_
#define _TRAP_SAMPLER_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	using SamplerDesc = TRAP::Graphics::RendererAPI::SamplerDesc;
	using FilterType = TRAP::Graphics::RendererAPI::FilterType;
	using AddressMode = TRAP::Graphics::RendererAPI::AddressMode;
	using CompareMode = TRAP::Graphics::RendererAPI::CompareMode;
	using MipMapMode = TRAP::Graphics::RendererAPI::MipMapMode;

	class Sampler
	{
	public:
		static TRAP::Ref<Sampler> Create(const SamplerDesc& desc);

		virtual ~Sampler();

		/// <summary>
		/// Default Copy Constructor.
		/// </summary>
		Sampler(const Sampler&) = default;
		/// <summary>
		/// Default Copy Assignment Operator.
		/// </summary>
		Sampler& operator=(const Sampler&) = default;
		/// <summary>
		/// Default Move Constructor.
		/// </summary>
		Sampler(Sampler&&) = default;
		/// <summary>
		/// Default Move Assignment Operator.
		/// </summary>
		Sampler& operator=(Sampler&&) = default;

		FilterType GetMinFilter() const;
		FilterType GetMagFilter() const;
		MipMapMode GetMipMapMode() const;
		AddressMode GetAddressU() const;
		AddressMode GetAddressV() const;
		AddressMode GetAddressW() const;
		float GetMipLodBias() const;
		float GetMaxAnisotropy() const;
		CompareMode GetCompareFunc() const;

		static void ClearCache();

	protected:
		Sampler();

		SamplerDesc m_samplerDesc;

	private:
		static std::unordered_map<SamplerDesc, TRAP::Ref<Sampler>> s_cachedSamplers;
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
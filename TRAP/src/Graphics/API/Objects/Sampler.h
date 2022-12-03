#ifndef TRAP_SAMPLER_H
#define TRAP_SAMPLER_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	using SamplerDesc = TRAP::Graphics::RendererAPI::SamplerDesc;
	using FilterType = TRAP::Graphics::RendererAPI::FilterType;
	using AddressMode = TRAP::Graphics::RendererAPI::AddressMode;
	using CompareMode = TRAP::Graphics::RendererAPI::CompareMode;
	using MipMapMode = TRAP::Graphics::RendererAPI::MipMapMode;
	using SampleCount = TRAP::Graphics::RendererAPI::SampleCount;

	class Sampler
	{
	public:
		/// <summary>
		/// Create a new sampler from the given description.
		/// </summary>
		/// <param name="desc">Sampler description.</param>
		/// <returns>Created sampler.</returns>
		static TRAP::Ref<Sampler> Create(SamplerDesc desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Sampler();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Sampler(const Sampler&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Sampler& operator=(const Sampler&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Sampler(Sampler&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Sampler& operator=(Sampler&&) = default;

		/// <summary>
		/// Retrieve the minification filter of the sampler.
		/// </summary>
		/// <returns>Minification filter.</returns>
		FilterType GetMinFilter() const;
		/// <summary>
		/// Retrieve the magnification filter of the sampler.
		/// </summary>
		/// <returns>Magnification filter.</returns>
		FilterType GetMagFilter() const;
		/// <summary>
		/// Retrieve the mip map mode of the sampler.
		/// </summary>
		/// <returns>Mip map mode.</returns>
		MipMapMode GetMipMapMode() const;
		/// <summary>
		/// Retrieve the address mode of the U coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		AddressMode GetAddressU() const;
		/// <summary>
		/// Retrieve the address mode of the V coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		AddressMode GetAddressV() const;
		/// <summary>
		/// Retrieve the address mode of the W coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		AddressMode GetAddressW() const;
		/// <summary>
		/// Retrieve the mip lod bias of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		float GetMipLodBias() const;
		/// <summary>
		/// Retrieve the max anisotropy of the sampler.
		/// </summary>
		/// <returns>Max anisotropy.</returns>
		float GetAnisotropyLevel() const;
		/// <summary>
		/// Retrieve the compare function of the sampler.
		/// </summary>
		/// <returns>Compare function.</returns>
		CompareMode GetCompareFunc() const;
		bool UsesEngineAnisotropyLevel() const;

		/// <summary>
		/// Clear all cached samplers.
		/// </summary>
		static void ClearCache();

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Sampler();

		SamplerDesc m_samplerDesc;
		bool m_usesEngineAnisotropyLevel = false;

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

#endif /*TRAP_SAMPLER_H*/
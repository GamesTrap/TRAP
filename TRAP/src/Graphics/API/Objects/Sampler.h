#ifndef TRAP_SAMPLER_H
#define TRAP_SAMPLER_H

#include "Graphics/RenderCommand.h"

namespace TRAP::Graphics
{
	class Sampler
	{
	public:
		/// <summary>
		/// Create a new sampler from the given description.
		/// </summary>
		/// <param name="desc">Sampler description.</param>
		/// <returns>Created sampler.</returns>
		[[nodiscard]] static TRAP::Ref<Sampler> Create(RendererAPI::SamplerDesc desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Sampler();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr Sampler(const Sampler&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Sampler& operator=(const Sampler&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		constexpr Sampler(Sampler&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Sampler& operator=(Sampler&&) noexcept = default;

		/// <summary>
		/// Retrieve the minification filter of the sampler.
		/// </summary>
		/// <returns>Minification filter.</returns>
		[[nodiscard]] constexpr FilterType GetMinFilter() const noexcept;
		/// <summary>
		/// Retrieve the magnification filter of the sampler.
		/// </summary>
		/// <returns>Magnification filter.</returns>
		[[nodiscard]] constexpr FilterType GetMagFilter() const noexcept;
		/// <summary>
		/// Retrieve the mip map mode of the sampler.
		/// </summary>
		/// <returns>Mip map mode.</returns>
		[[nodiscard]] constexpr MipMapMode GetMipMapMode() const noexcept;
		/// <summary>
		/// Retrieve the address mode of the U coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] constexpr AddressMode GetAddressU() const noexcept;
		/// <summary>
		/// Retrieve the address mode of the V coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] constexpr AddressMode GetAddressV() const noexcept;
		/// <summary>
		/// Retrieve the address mode of the W coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] constexpr AddressMode GetAddressW() const noexcept;
		/// <summary>
		/// Retrieve the mip lod bias of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] constexpr float GetMipLodBias() const noexcept;
		/// <summary>
		/// Retrieve the max anisotropy of the sampler.
		/// </summary>
		/// <returns>Max anisotropy.</returns>
		[[nodiscard]] constexpr float GetAnisotropyLevel() const noexcept;
		/// <summary>
		/// Retrieve the compare function of the sampler.
		/// </summary>
		/// <returns>Compare function.</returns>
		[[nodiscard]] constexpr CompareMode GetCompareFunc() const noexcept;
		/// <summary>
		/// Retrieve whether the sampler uses the engines anisotropy level or not.
		/// </summary>
		/// <returns>True if engine set anisotropy level is used, false otherwise.</returns>
		[[nodiscard]] constexpr bool UsesEngineAnisotropyLevel() const noexcept;

		/// <summary>
		/// Clear all cached samplers.
		/// </summary>
		static void ClearCache() noexcept;

		/// <summary>
		/// Updates all samplers currently in use.
		/// Only call this between Frames!
		/// </summary>
		static void UpdateSamplers();

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		Sampler();

		virtual void UpdateAnisotropy(float anisotropy) = 0;

		RendererAPI::SamplerDesc m_samplerDesc{};
		bool m_usesEngineAnisotropyLevel = false;

	private:
		static std::unordered_map<RendererAPI::SamplerDesc, TRAP::Ref<Sampler>> s_cachedSamplers;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::FilterType TRAP::Graphics::Sampler::GetMinFilter() const noexcept
{
	return m_samplerDesc.MinFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::FilterType TRAP::Graphics::Sampler::GetMagFilter() const noexcept
{
	return m_samplerDesc.MagFilter;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::MipMapMode TRAP::Graphics::Sampler::GetMipMapMode() const noexcept
{
	return m_samplerDesc.MipMapMode;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressU() const noexcept
{
	return m_samplerDesc.AddressU;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressV() const noexcept
{
	return m_samplerDesc.AddressV;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::AddressMode TRAP::Graphics::Sampler::GetAddressW() const noexcept
{
	return m_samplerDesc.AddressW;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::Sampler::GetMipLodBias() const noexcept
{
	return m_samplerDesc.MipLodBias;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr float TRAP::Graphics::Sampler::GetAnisotropyLevel() const noexcept
{
	if(!m_samplerDesc.EnableAnisotropy)
		return 0.0f;

	return m_samplerDesc.OverrideAnisotropyLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::CompareMode TRAP::Graphics::Sampler::GetCompareFunc() const noexcept
{
	return m_samplerDesc.CompareFunc;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr bool TRAP::Graphics::Sampler::UsesEngineAnisotropyLevel() const noexcept
{
	return m_usesEngineAnisotropyLevel;
}

//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template <>
 	struct hash<TRAP::Graphics::RendererAPI::SamplerDesc>
	{
		[[nodiscard]] constexpr std::size_t operator()(const TRAP::Graphics::RendererAPI::SamplerDesc& desc) const noexcept
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
	};
}

#endif /*TRAP_SAMPLER_H*/
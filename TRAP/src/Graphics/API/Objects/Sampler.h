#ifndef TRAP_SAMPLER_H
#define TRAP_SAMPLER_H

#include "Graphics/RenderCommand.h"

namespace TRAP::Graphics
{
	class Sampler
	{
	public:
		/// @brief Create a new sampler from the given description.
		/// @param desc Sampler description.
		/// @return Created sampler.
		[[nodiscard]] static TRAP::Ref<Sampler> Create(RendererAPI::SamplerDesc desc);

		/// @brief Destructor.
		virtual ~Sampler();

		/// @brief Copy constructor.
		constexpr Sampler(const Sampler&) noexcept = default;
		/// @brief Copy assignment operator.
		Sampler& operator=(const Sampler&) noexcept = default;
		/// @brief Move constructor.
		constexpr Sampler(Sampler&&) noexcept = default;
		/// @brief Move assignment operator.
		Sampler& operator=(Sampler&&) noexcept = default;

		/// @brief Retrieve the minification filter of the sampler.
		/// @return Minification filter.
		[[nodiscard]] constexpr FilterType GetMinFilter() const noexcept;
		/// @brief Retrieve the magnification filter of the sampler.
		/// @return Magnification filter.
		[[nodiscard]] constexpr FilterType GetMagFilter() const noexcept;
		/// @brief Retrieve the mip map mode of the sampler.
		/// @return Mip map mode.
		[[nodiscard]] constexpr MipMapMode GetMipMapMode() const noexcept;
		/// @brief Retrieve the address mode of the U coordinate of the sampler.
		/// @return Address mode.
		[[nodiscard]] constexpr AddressMode GetAddressU() const noexcept;
		/// @brief Retrieve the address mode of the V coordinate of the sampler.
		/// @return Address mode.
		[[nodiscard]] constexpr AddressMode GetAddressV() const noexcept;
		/// @brief Retrieve the address mode of the W coordinate of the sampler.
		/// @return Address mode.
		[[nodiscard]] constexpr AddressMode GetAddressW() const noexcept;
		/// @brief Retrieve the mip lod bias of the sampler.
		/// @return Address mode.
		[[nodiscard]] constexpr f32 GetMipLodBias() const noexcept;
		/// @brief Retrieve the max anisotropy of the sampler.
		/// @return Max anisotropy.
		[[nodiscard]] constexpr f32 GetAnisotropyLevel() const noexcept;
		/// @brief Retrieve the compare function of the sampler.
		/// @return Compare function.
		[[nodiscard]] constexpr CompareMode GetCompareFunc() const noexcept;
		/// @brief Retrieve whether the sampler uses the engines anisotropy level or not.
		/// @return True if engine set anisotropy level is used, false otherwise.
		[[nodiscard]] constexpr bool UsesEngineAnisotropyLevel() const noexcept;

		/// @brief Clear all cached samplers.
		static void ClearCache() noexcept;

		/// @brief Updates all samplers currently in use.
		/// @warning Only call this function between frames!
		static void UpdateSamplers();

	protected:
		/// @brief Constructor.
		explicit Sampler(const RendererAPI::SamplerDesc& desc);

		/// @brief Update the anisotropy value of the sampler.
		/// @param anisotropy New anisotropy to use.
		virtual void UpdateAnisotropy(f32 anisotropy) = 0;

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

[[nodiscard]] constexpr f32 TRAP::Graphics::Sampler::GetMipLodBias() const noexcept
{
	return m_samplerDesc.MipLodBias;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr f32 TRAP::Graphics::Sampler::GetAnisotropyLevel() const noexcept
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
		[[nodiscard]] constexpr usize operator()(const TRAP::Graphics::RendererAPI::SamplerDesc& desc) const noexcept
		{
			usize res = 0;

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
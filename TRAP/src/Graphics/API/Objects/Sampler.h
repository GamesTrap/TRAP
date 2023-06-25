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
		constexpr virtual ~Sampler();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		Sampler(const Sampler&) noexcept = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		Sampler& operator=(const Sampler&) noexcept = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		Sampler(Sampler&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		Sampler& operator=(Sampler&&) noexcept = default;

		/// <summary>
		/// Retrieve the minification filter of the sampler.
		/// </summary>
		/// <returns>Minification filter.</returns>
		[[nodiscard]] FilterType GetMinFilter() const noexcept;
		/// <summary>
		/// Retrieve the magnification filter of the sampler.
		/// </summary>
		/// <returns>Magnification filter.</returns>
		[[nodiscard]] FilterType GetMagFilter() const noexcept;
		/// <summary>
		/// Retrieve the mip map mode of the sampler.
		/// </summary>
		/// <returns>Mip map mode.</returns>
		[[nodiscard]] MipMapMode GetMipMapMode() const noexcept;
		/// <summary>
		/// Retrieve the address mode of the U coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] AddressMode GetAddressU() const noexcept;
		/// <summary>
		/// Retrieve the address mode of the V coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] AddressMode GetAddressV() const noexcept;
		/// <summary>
		/// Retrieve the address mode of the W coordinate of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] AddressMode GetAddressW() const noexcept;
		/// <summary>
		/// Retrieve the mip lod bias of the sampler.
		/// </summary>
		/// <returns>Address mode.</returns>
		[[nodiscard]] float GetMipLodBias() const noexcept;
		/// <summary>
		/// Retrieve the max anisotropy of the sampler.
		/// </summary>
		/// <returns>Max anisotropy.</returns>
		[[nodiscard]] float GetAnisotropyLevel() const noexcept;
		/// <summary>
		/// Retrieve the compare function of the sampler.
		/// </summary>
		/// <returns>Compare function.</returns>
		[[nodiscard]] CompareMode GetCompareFunc() const noexcept;
		/// <summary>
		/// Retrieve whether the sampler uses the engines anisotropy level or not.
		/// </summary>
		/// <returns>True if engine set anisotropy level is used, false otherwise.</returns>
		[[nodiscard]] bool UsesEngineAnisotropyLevel() const noexcept;

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

		RendererAPI::SamplerDesc m_samplerDesc;
		bool m_usesEngineAnisotropyLevel = false;

	private:
		static std::unordered_map<RendererAPI::SamplerDesc, TRAP::Ref<Sampler>> s_cachedSamplers;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::Graphics::Sampler::~Sampler()
{
#ifdef ENABLE_GRAPHICS_DEBUG
	TP_DEBUG(Log::RendererSamplerPrefix, "Destroying Sampler");
#endif /*ENABLE_GRAPHICS_DEBUG*/
}

//-------------------------------------------------------------------------------------------------------------------//

namespace std
{
	template <>
 	struct hash<TRAP::Graphics::RendererAPI::SamplerDesc>
	{
		std::size_t operator()(const TRAP::Graphics::RendererAPI::SamplerDesc& desc) const noexcept;
	};
}

#endif /*TRAP_SAMPLER_H*/
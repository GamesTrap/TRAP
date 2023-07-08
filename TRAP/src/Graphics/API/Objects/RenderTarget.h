#ifndef TRAP_RENDERTARGET_H
#define TRAP_RENDERTARGET_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class Texture;

	class RenderTarget
	{
	public:
		/// <summary>
		/// Create a new render target from the given description.
		/// </summary>
		/// <param name="desc">Render target description.</param>
		/// <returns>Created render target.</returns>
		[[nodiscard]] static TRAP::Ref<RenderTarget> Create(const RendererAPI::RenderTargetDesc& desc);

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~RenderTarget();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		constexpr RenderTarget(const RenderTarget&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		constexpr RenderTarget& operator=(const RenderTarget&) = delete;
		/// <summary>
		/// Move constructor.
		/// </summary>
		RenderTarget(RenderTarget&&) noexcept = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

		/// <summary>
		/// Retrieve the width of the render target.
		/// </summary>
		/// <returns>Render target width.</returns>
		[[nodiscard]] constexpr uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the render target.
		/// </summary>
		/// <returns>Render target height.</returns>
		[[nodiscard]] constexpr uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the depth of the render target.
		/// </summary>
		/// <returns>Render target depth.</returns>
		[[nodiscard]] constexpr uint32_t GetDepth() const noexcept;
		/// <summary>
		/// Retrieve the array size of the render target.
		/// </summary>
		/// <returns>Render target array size.</returns>
		[[nodiscard]] constexpr uint32_t GetArraySize() const noexcept;
		/// <summary>
		/// Retrieve the mip levels of the render target.
		/// </summary>
		/// <returns>Render target mip levels.</returns>
		[[nodiscard]] constexpr uint32_t GetMipLevels() const noexcept;
		/// <summary>
		/// Retrieve the sample count of the render target.
		/// </summary>
		/// <returns>Render target sample count.</returns>
		[[nodiscard]] constexpr RendererAPI::SampleCount GetSampleCount() const noexcept;
		/// <summary>
		/// Retrieve the sample quality of the render target.
		/// </summary>
		/// <returns>Render target sample quality.</returns>
		[[nodiscard]] constexpr uint32_t GetSampleQuality() const noexcept;
		/// <summary>
		/// Retrieve the image format of the render target.
		/// </summary>
		/// <returns>Render target image format.</returns>
		[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat GetImageFormat() const noexcept;
		/// <summary>
		/// Retrieve the clear color of the render target.
		/// </summary>
		/// <returns>Render target clear color.</returns>
		[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ClearValue GetClearValue() const noexcept;
		/// <summary>
		/// Retrieve the descriptor type of the render target.
		/// </summary>
		/// <returns>Render target descriptor type.</returns>
		[[nodiscard]] constexpr RendererAPI::DescriptorType GetDescriptorType() const noexcept;

		/// <summary>
		/// Retrieve the texture base of the render target.
		/// </summary>
		/// <returns>Render target texture base.</returns>
		[[nodiscard]] Ref<TRAP::Graphics::Texture> GetTexture() const noexcept;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		RenderTarget();

		TRAP::Ref<TRAP::Graphics::Texture> m_texture = nullptr;

		RendererAPI::ClearValue m_clearValue = RendererAPI::Color{1.0, 1.0, 1.0, 1.0};
		uint32_t m_arraySize = 0;
		uint32_t m_depth = 0;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		RendererAPI::DescriptorType m_descriptors = RendererAPI::DescriptorType::Undefined;
		uint32_t m_mipLevels = 0;
		uint32_t m_sampleQuality = 0;
		TRAP::Graphics::API::ImageFormat m_format = TRAP::Graphics::API::ImageFormat::Undefined;
		RendererAPI::SampleCount m_sampleCount = RendererAPI::SampleCount::One;
	};
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::RenderTarget::GetWidth() const noexcept
{
	return m_width;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::RenderTarget::GetHeight() const noexcept
{
	return m_height;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::RenderTarget::GetDepth() const noexcept
{
	return m_depth;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::RenderTarget::GetArraySize() const noexcept
{
	return m_arraySize;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::RenderTarget::GetMipLevels() const noexcept
{
	return m_mipLevels;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::SampleCount TRAP::Graphics::RenderTarget::GetSampleCount() const noexcept
{
	return m_sampleCount;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr uint32_t TRAP::Graphics::RenderTarget::GetSampleQuality() const noexcept
{
	return m_sampleQuality;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::API::ImageFormat TRAP::Graphics::RenderTarget::GetImageFormat() const noexcept
{
	return m_format;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::ClearValue TRAP::Graphics::RenderTarget::GetClearValue() const noexcept
{
	return m_clearValue;
}

//-------------------------------------------------------------------------------------------------------------------//

[[nodiscard]] constexpr TRAP::Graphics::RendererAPI::DescriptorType TRAP::Graphics::RenderTarget::GetDescriptorType() const noexcept
{
	return m_descriptors;
}

#endif /*TRAP_RENDERTARGET_H*/
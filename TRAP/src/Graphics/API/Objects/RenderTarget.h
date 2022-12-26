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
		RenderTarget(const RenderTarget&) = delete;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		RenderTarget& operator=(const RenderTarget&) = delete;
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
		[[nodiscard]] uint32_t GetWidth() const noexcept;
		/// <summary>
		/// Retrieve the height of the render target.
		/// </summary>
		/// <returns>Render target height.</returns>
		[[nodiscard]] uint32_t GetHeight() const noexcept;
		/// <summary>
		/// Retrieve the depth of the render target.
		/// </summary>
		/// <returns>Render target depth.</returns>
		[[nodiscard]] uint32_t GetDepth() const noexcept;
		/// <summary>
		/// Retrieve the array size of the render target.
		/// </summary>
		/// <returns>Render target array size.</returns>
		[[nodiscard]] uint32_t GetArraySize() const noexcept;
		/// <summary>
		/// Retrieve the mip levels of the render target.
		/// </summary>
		/// <returns>Render target mip levels.</returns>
		[[nodiscard]] uint32_t GetMipLevels() const noexcept;
		/// <summary>
		/// Retrieve the sample count of the render target.
		/// </summary>
		/// <returns>Render target sample count.</returns>
		[[nodiscard]] RendererAPI::SampleCount GetSampleCount() const noexcept;
		/// <summary>
		/// Retrieve the sample quality of the render target.
		/// </summary>
		/// <returns>Render target sample quality.</returns>
		[[nodiscard]] uint32_t GetSampleQuality() const noexcept;
		/// <summary>
		/// Retrieve the image format of the render target.
		/// </summary>
		/// <returns>Render target image format.</returns>
		[[nodiscard]] TRAP::Graphics::API::ImageFormat GetImageFormat() const noexcept;
		/// <summary>
		/// Retrieve the clear color of the render target.
		/// </summary>
		/// <returns>Render target clear color.</returns>
		[[nodiscard]] TRAP::Graphics::RendererAPI::ClearValue GetClearValue() const noexcept;
		/// <summary>
		/// Retrieve the descriptor type of the render target.
		/// </summary>
		/// <returns>Render target descriptor type.</returns>
		[[nodiscard]] RendererAPI::DescriptorType GetDescriptorType() const noexcept;

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

		TRAP::Ref<TRAP::Graphics::Texture> m_texture;

		RendererAPI::ClearValue m_clearValue;
		uint32_t m_arraySize;
		uint32_t m_depth;
		uint32_t m_width;
		uint32_t m_height;
		RendererAPI::DescriptorType m_descriptors;
		uint32_t m_mipLevels;
		uint32_t m_sampleQuality;
		TRAP::Graphics::API::ImageFormat m_format;
		RendererAPI::SampleCount m_sampleCount;
	};
}

#endif /*TRAP_RENDERTARGET_H*/
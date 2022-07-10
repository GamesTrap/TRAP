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
		static TRAP::Ref<RenderTarget> Create(const RendererAPI::RenderTargetDesc& desc);

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
		RenderTarget(RenderTarget&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RenderTarget& operator=(RenderTarget&&) = default;

		/// <summary>
		/// Retrieve the width of the render target.
		/// </summary>
		/// <returns>Render target width.</returns>
		uint32_t GetWidth() const;
		/// <summary>
		/// Retrieve the height of the render target.
		/// </summary>
		/// <returns>Render target height.</returns>
		uint32_t GetHeight() const;
		/// <summary>
		/// Retrieve the depth of the render target.
		/// </summary>
		/// <returns>Render target depth.</returns>
		uint32_t GetDepth() const;
		/// <summary>
		/// Retrieve the array size of the render target.
		/// </summary>
		/// <returns>Render target array size.</returns>
		uint32_t GetArraySize() const;
		/// <summary>
		/// Retrieve the mip levels of the render target.
		/// </summary>
		/// <returns>Render target mip levels.</returns>
		uint32_t GetMipLevels() const;
		/// <summary>
		/// Retrieve the sample count of the render target.
		/// </summary>
		/// <returns>Render target sample count.</returns>
		RendererAPI::SampleCount GetSampleCount() const;
		/// <summary>
		/// Retrieve the sample quality of the render target.
		/// </summary>
		/// <returns>Render target sample quality.</returns>
		uint32_t GetSampleQuality() const;
		/// <summary>
		/// Retrieve the image format of the render target.
		/// </summary>
		/// <returns>Render target image format.</returns>
		TRAP::Graphics::API::ImageFormat GetImageFormat() const;
		/// <summary>
		/// Retrieve the clear color of the render target.
		/// </summary>
		/// <returns>Render target clear color.</returns>
		TRAP::Math::Vec4 GetClearColor() const;
		/// <summary>
		/// Retrieve the clear depth value of the render target.
		/// </summary>
		/// <returns>Render target clear depth value.</returns>
		float GetClearDepth() const;
		/// <summary>
		/// Retrieve the clear stencil value of the render target.
		/// </summary>
		/// <returns>Render target clear stencil value.</returns>
		uint32_t GetClearStencil() const;
		/// <summary>
		/// Retrieve the descriptor type of the render target.
		/// </summary>
		/// <returns>Render target descriptor type.</returns>
		RendererAPI::DescriptorType GetDescriptorType() const;

		/// <summary>
		/// Retrieve the texture base of the render target.
		/// </summary>
		/// <returns>Render target texture base.</returns>
		TRAP::Graphics::Texture* GetTexture() const;

	protected:
		/// <summary>
		/// Constructor.
		/// </summary>
		RenderTarget();

		TRAP::Scope<TRAP::Graphics::Texture> m_texture;

		TRAP::Math::Vec4 m_clearColor;
		float m_clearDepth;
		uint32_t m_clearStencil;
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
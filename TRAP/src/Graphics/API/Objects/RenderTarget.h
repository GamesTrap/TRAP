#ifndef TRAP_RENDERTARGET_H
#define TRAP_RENDERTARGET_H

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class TextureBase;

	class RenderTarget
	{
	public:
		static TRAP::Ref<RenderTarget> Create(const RendererAPI::RenderTargetDesc& desc);

		virtual ~RenderTarget();

		/// <summary>
		/// Copy constructor.
		/// </summary>
		RenderTarget(const RenderTarget&) = default;
		/// <summary>
		/// Copy assignment operator.
		/// </summary>
		RenderTarget& operator=(const RenderTarget&) = default;
		/// <summary>
		/// Move constructor.
		/// </summary>
		RenderTarget(RenderTarget&&) = default;
		/// <summary>
		/// Move assignment operator.
		/// </summary>
		RenderTarget& operator=(RenderTarget&&) = default;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetArraySize() const;
		uint32_t GetMipLevels() const;
		RendererAPI::SampleCount GetSampleCount() const;
		uint32_t GetSampleQuality() const;
		TRAP::Graphics::API::ImageFormat GetImageFormat() const;
		TRAP::Math::Vec4 GetClearColor() const;
		float GetClearDepth() const;
		uint32_t GetClearStencil() const;
		RendererAPI::DescriptorType GetDescriptorType() const;

		TRAP::Ref<TRAP::Graphics::TextureBase> GetTexture() const;

	protected:
		RenderTarget();

		TRAP::Ref<TRAP::Graphics::TextureBase> m_texture;

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
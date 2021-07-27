#ifndef _TRAP_RENDERTARGET_H_
#define _TRAP_RENDERTARGET_H_

#include "Graphics/API/RendererAPI.h"

namespace TRAP::Graphics
{
	class TextureBase;

	class RenderTarget
	{
	public:
		static TRAP::Ref<RenderTarget> Create(const RendererAPI::RenderTargetDesc& desc);

		virtual ~RenderTarget() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;
		virtual uint32_t GetArraySize() const = 0;
		virtual uint32_t GetMipLevels() const = 0;
		virtual RendererAPI::SampleCount GetSampleCount() const = 0;
		virtual uint32_t GetSampleQuality() const = 0;
		virtual TRAP::Graphics::API::ImageFormat GetImageFormat() const = 0;
		virtual TRAP::Math::Vec4 GetClearColor() const = 0;
		virtual float GetClearDepth() const = 0;
		virtual uint32_t GetClearStencil() const = 0;
		virtual RendererAPI::DescriptorType GetDescriptorType() const = 0;

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

#endif /*_TRAP_RENDERTARGET_H_*/